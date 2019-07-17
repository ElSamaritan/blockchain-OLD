// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "NetNode.h"

#include <algorithm>
#include <fstream>
#include <cinttypes>
#include <thread>
#include <future>

#include <boost/foreach.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/utility/value_init.hpp>

#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>

#include <System/Context.h>
#include <System/ContextGroupTimeout.h>
#include <System/EventLock.h>
#include <System/InterruptedException.h>
#include <System/Ipv4Address.h>
#include <System/Ipv4Resolver.h>
#include <System/TcpListener.h>
#include <System/TcpConnector.h>

#include <Xi/Global.hh>
#include <Xi/FileSystem.h>
#include <Xi/Config.h>
#include <Xi/Version/Version.h>
#include <Xi/Crypto/Random/Random.hh>

#include "Common/StdInputStream.h"
#include "Common/StdOutputStream.h"
#include "Common/Util.h"
#include "crypto/crypto.h"

#include "P2p/ConnectionContext.h"
#include "P2p/LevinProtocol.h"
#include "P2p/P2pProtocolDefinitions.h"
#include "P2p/UPNPErrorMessage.h"

#include "Serialization/BinaryInputStreamSerializer.h"
#include "Serialization/BinaryOutputStreamSerializer.h"
#include "Serialization/SerializationOverloads.h"

using namespace Common;
using namespace Logging;
using namespace CryptoNote;

namespace {

size_t get_random_index_with_fixed_probability(size_t max_index) {
  // divide by zero workaround
  if (!max_index)
    return 0;
  size_t x = 0;
  XI_RETURN_EC_IF_NOT(
      Xi::Crypto::Random::generate(Xi::asByteSpan(&x, sizeof(size_t))) == Xi::Crypto::Random::RandomError::Success, 0);
  x = x % (max_index + 1);
  return (x * x * x) / (max_index * max_index);  // parabola \/
}

void addPortMapping(Logging::LoggerRef& logger, const std::string& appname, uint16_t intport, uint16_t extport) {
  // Add UPnP port mapping
  logger(Info) << "Attempting to add IGD port mapping.";
  int result;
  UPNPDev* deviceList = upnpDiscover(1000, NULL, NULL, 0, 0, 2, &result);
  UPNPUrls urls;
  IGDdatas igdData;
  char lanAddress[64];
  result = UPNP_GetValidIGD(deviceList, &urls, &igdData, lanAddress, sizeof lanAddress);
  freeUPNPDevlist(deviceList);
  if (result != 0) {
    if (result == 1) {
      auto intportstring = std::to_string(intport);
      auto extportstring = std::to_string(extport);
      int upnpPortMappingResult =
          UPNP_AddPortMapping(urls.controlURL, igdData.first.servicetype, extportstring.c_str(), intportstring.c_str(),
                              lanAddress, appname.c_str(), "TCP", 0, "0");
      if (upnpPortMappingResult != UPNPCOMMAND_SUCCESS) {
        logger(Error) << "UPNP_AddPortMapping failed: " << get_upnp_error_message(upnpPortMappingResult);
      } else {
        logger(Info) << "Added IGD port mapping.";
      }
    } else if (result == 2) {
      logger(Info) << "IGD was found but reported as not connected.";
    } else if (result == 3) {
      logger(Info) << "UPnP device was found but not recognized as IGD.";
    } else {
      logger(Error) << "UPNP_GetValidIGD returned an unknown result code.";
    }

    FreeUPNPUrls(&urls);
  } else {
    logger(Info) << "No IGD was found.";
  }
}

bool parse_peer_from_string(NetworkAddress& pe, const std::string& node_addr) {
  return Common::parseIpAddressAndPort(pe.ip, pe.port, node_addr);
}

}  // namespace

namespace CryptoNote {
namespace {

std::string print_peerlist_to_string(const std::list<PeerlistEntry>& pl) {
  time_t now_time = 0;
  time(&now_time);
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(8) << std::hex << std::noshowbase;
  for (const auto& pe : pl) {
    ss << pe.id << "\t" << pe.address << " \tlast_seen: " << Common::timeIntervalToString(now_time - pe.last_seen)
       << std::endl;
  }
  return ss.str();
}

}  // namespace

//-----------------------------------------------------------------------------------
// P2pConnectionContext implementation
//-----------------------------------------------------------------------------------

bool P2pConnectionContext::pushMessage(P2pMessage&& msg) {
  writeQueueSize += msg.size();

  if (writeQueueSize > Xi::Config::P2P::maximumWriteBufferSize()) {
    logger(Debugging) << *this << "Write queue overflows. Interrupt connection";
    interrupt();
    return false;
  }

  writeQueue.push_back(std::move(msg));
  queueEvent.set();
  return true;
}

std::vector<P2pMessage> P2pConnectionContext::popBuffer() {
  writeOperationStartTime = TimePoint();

  while (writeQueue.empty() && !stopped) {
    queueEvent.wait();
  }

  std::vector<P2pMessage> msgs(std::move(writeQueue));
  writeQueue.clear();
  writeQueueSize = 0;
  writeOperationStartTime = Clock::now();
  queueEvent.clear();
  return msgs;
}

uint64_t P2pConnectionContext::writeDuration(TimePoint now) const {  // in milliseconds
  return writeOperationStartTime == TimePoint()
             ? 0
             : std::chrono::duration_cast<std::chrono::milliseconds>(now - writeOperationStartTime).count();
}

void P2pConnectionContext::interrupt() {
  logger(Debugging) << *this << "Interrupt connection";
  assert(context != nullptr);
  stopped = true;
  queueEvent.set();
  context->interrupt();
}

template <typename Command, typename Handler>
int invokeAdaptor(const BinaryArray& reqBuf, BinaryArray& resBuf, P2pConnectionContext& ctx, Handler handler) {
  typedef typename Command::request Request;
  typedef typename Command::response Response;
  int command = Command::ID;

  Request req = boost::value_initialized<Request>();

  auto decodeResult = LevinProtocol::decode(reqBuf, req);
  if (decodeResult.isError()) {
    throw std::runtime_error("Failed to load_from_binary in command " + std::to_string(command) +
                             " with error= " + decodeResult.error().message());
  }

  Response res = boost::value_initialized<Response>();
  int ret = handler(command, req, res, ctx);
  resBuf = LevinProtocol::encode(res);
  return ret;
}

NodeServer::NodeServer(System::Dispatcher& dispatcher, const Xi::Config::Network::Configuration& network,
                       CryptoNote::CryptoNoteProtocolHandler& payload_handler, Logging::ILogger& log)
    : m_dispatcher(dispatcher),
      m_workingContextGroup(dispatcher),
      m_payload_handler(payload_handler),
      m_allow_local_ip(false),
      m_hide_my_port(false),
      m_network_id(network.identifier()),
      logger(log, "NodeServer"),
      m_stopEvent(m_dispatcher),
      m_idleTimer(m_dispatcher),
      m_timedSyncTimer(m_dispatcher),
      m_timeoutTimer(m_dispatcher),
      m_stop(false),
      // intervals
      // m_peer_handshake_idle_maker_interval(CryptoNote::P2P_DEFAULT_HANDSHAKE_INTERVAL),
      m_connections_maker_interval(1),
      m_peerlist_store_interval(60 * 30, false),
      m_fails_before_block{20} {
}

bool NodeServer::serialize(ISerializer& s) {
  uint8_t version = 2;
  XI_RETURN_EC_IF_NOT(s(version, "version"), false);

  if (version < 1 || version > 2) {
    return false;
  }

  XI_RETURN_EC_IF_NOT(s(m_peerlist, "peerlist"), false);
  XI_RETURN_EC_IF_NOT(s(m_config.m_peer_id, "peer_id"), false);

  if (version > 1) {
    const auto currentTimestamp = time(nullptr);

    uint64_t blockedCount = m_blocked_hosts.size();
    XI_RETURN_EC_IF_NOT(s(blockedCount, "blocked_hosts_count"), false);
    if (s.type() == ISerializer::INPUT) {
      for (uint64_t i = 0; i < blockedCount; ++i) {
        uint32_t ip;
        int64_t timestamp;

        XI_RETURN_EC_IF_NOT(s(ip, ""), false);
        XI_RETURN_EC_IF_NOT(s(timestamp, ""), false);

        if (timestamp > currentTimestamp) {
          m_blocked_hosts[ip] = timestamp;
          logger(Logging::Trace) << "Imported blocked host: " << Common::ipAddressToString(ip);
        }
      }
    } else {
      for (const auto& blocked_host : m_blocked_hosts) {
        uint32_t ip = blocked_host.first;
        int64_t timestamp = blocked_host.second;
        XI_RETURN_EC_IF_NOT(s(ip, ""), false);
        XI_RETURN_EC_IF_NOT(s(timestamp, ""), false);
      }
    }

    uint64_t failsScoreCount = m_host_fails_score.size();
    XI_RETURN_EC_IF_NOT(s(failsScoreCount, "host_fails_score_count"), false);
    if (s.type() == ISerializer::INPUT) {
      for (uint64_t i = 0; i < failsScoreCount; ++i) {
        uint32_t ip;
        uint64_t score;

        XI_RETURN_EC_IF_NOT(s(ip, ""), false);
        XI_RETURN_EC_IF_NOT(s(score, ""), false);

        if (score > 2) {
          m_host_fails_score[ip] = score - 2;
          logger(Logging::Trace) << "Imported host fails score: " << Common::ipAddressToString(ip)
                                 << ", score=" << score - 2;
        }
      }
    } else {
      for (auto& host_fails_score : m_host_fails_score) {
        uint32_t ip = host_fails_score.first;
        uint64_t score = host_fails_score.second;

        XI_RETURN_EC_IF_NOT(s(ip, ""), false);
        XI_RETURN_EC_IF_NOT(s(score, ""), false);
      }
    }
  }
  return true;
}

#define INVOKE_HANDLER(CMD, Handler)                                                         \
  case CMD::ID: {                                                                            \
    ret = invokeAdaptor<CMD>(cmd.buf, out, ctx, boost::bind(Handler, this, _1, _2, _3, _4)); \
    break;                                                                                   \
  }

int NodeServer::handleCommand(const LevinProtocol::Command& cmd, BinaryArray& out, P2pConnectionContext& ctx,
                              bool& handled) {
  int ret = 0;
  handled = true;

  if (cmd.isResponse && cmd.command == COMMAND_TIMED_SYNC::ID) {
    if (!handleTimedSyncResponse(cmd.buf, ctx)) {
      // invalid response, close connection
      ctx.m_state = CryptoNoteConnectionContext::state_shutdown;
    }
    return 0;
  }

  switch (cmd.command) {
    INVOKE_HANDLER(COMMAND_HANDSHAKE, &NodeServer::handle_handshake)
    INVOKE_HANDLER(COMMAND_TIMED_SYNC, &NodeServer::handle_timed_sync)
    INVOKE_HANDLER(COMMAND_PING, &NodeServer::handle_ping)
#ifdef ALLOW_DEBUG_COMMANDS
    INVOKE_HANDLER(COMMAND_REQUEST_STAT_INFO, &NodeServer::handle_get_stat_info)
    INVOKE_HANDLER(COMMAND_REQUEST_NETWORK_STATE, &NodeServer::handle_get_network_state)
    INVOKE_HANDLER(COMMAND_REQUEST_PEER_ID, &NodeServer::handle_get_peer_id)
#endif
    default: {
      handled = false;
      ret = m_payload_handler.handleCommand(cmd.isNotify, cmd.command, cmd.buf, out, ctx, handled);
    }
  }

  return ret;
}

#undef INVOKE_HANDLER

bool NodeServer::init_config() {
  try {
    std::string state_file_path = m_config_folder + "/" + m_p2p_state_filename;
    bool loaded = false;

    try {
      std::ifstream p2p_data;
      p2p_data.open(state_file_path, std::ios_base::binary | std::ios_base::in);

      if (!p2p_data.fail()) {
        StdInputStream inputStream(p2p_data);
        BinaryInputStreamSerializer a(inputStream);
        XI_RETURN_EC_IF_NOT(CryptoNote::serialize(*this, a), false);
        loaded = true;
      }
    } catch (const std::exception& e) {
      logger(Error) << "Failed to load config from file '" << state_file_path << "': " << e.what();
    }

    if (!loaded) {
      XI_RETURN_EC_IF_NOT(make_default_config(), false);
    }

    // at this moment we have hardcoded config
    m_config.m_net_config.handshake_interval =
        static_cast<uint32_t>(std::chrono::milliseconds{Xi::Config::P2P::handshakeInterval()}.count());
    m_config.m_net_config.connections_count = Xi::Config::P2P::connectionsCount();
    m_config.m_net_config.packet_max_size = Xi::Config::P2P::maximumPackageSize();
    m_config.m_net_config.config_id = 0;  // initial config
    m_config.m_net_config.connection_timeout =
        static_cast<uint32_t>(std::chrono::milliseconds{Xi::Config::P2P::connectionTimeout()}.count());
    m_config.m_net_config.ping_connection_timeout =
        static_cast<uint32_t>(std::chrono::milliseconds{Xi::Config::P2P::pingTimeout()}.count());
    m_config.m_net_config.send_peerlist_sz = Xi::Config::P2P::handshakePeersCount();

    m_first_connection_maker_call = true;
  } catch (const std::exception& e) {
    logger(Error) << "init_config failed: " << e.what();
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------------
void NodeServer::for_each_connection(std::function<void(CryptoNoteConnectionContext&, PeerIdType)> f) {
  for (auto& ctx : m_connections) {
    f(ctx.second, ctx.second.peerId);
  }
}

//-----------------------------------------------------------------------------------
void NodeServer::externalRelayNotifyToAll(int command, const BinaryArray& data_buff,
                                          const net_connection_id* excludeConnection) {
  m_dispatcher.remoteSpawn(
      [this, command, data_buff, excludeConnection] { relay_notify_to_all(command, data_buff, excludeConnection); });
}

bool CryptoNote::NodeServer::report_failure(const uint32_t ip, CryptoNote::P2pPenalty penality) {
  return add_host_fail(ip, penality);
}
void NodeServer::report_success(const uint32_t ip) {
  add_host_success(ip);
}

bool NodeServer::is_ip_address_blocked(const uint32_t ip) {
  return evaluate_blocked_connection(ip);
}

//-----------------------------------------------------------------------------------
bool NodeServer::make_default_config() {
  XI_RETURN_EC_IF_NOT(Xi::Crypto::Random::generate(Xi::asByteSpan(&m_config.m_peer_id, sizeof(PeerIdType))) ==
                          Xi::Crypto::Random::RandomError::Success,
                      false);
  logger(Info) << "Generated new peer ID: " << m_config.m_peer_id;
  return true;
}

//-----------------------------------------------------------------------------------

bool NodeServer::handleConfig(const NetNodeConfig& config) {
  m_bind_ip = config.getBindIp();
  m_port = std::to_string(config.getBindPort());
  m_external_port = config.getExternalPort();
  m_allow_local_ip = config.getAllowLocalIp();
  m_block_time = config.getBlockDuration();
  m_auto_block = config.getAutoBlock();

  auto peers = config.getPeers();
  std::copy(peers.begin(), peers.end(), std::back_inserter(m_command_line_peers));

  auto exclusiveNodes = config.getExclusiveNodes();
  std::copy(exclusiveNodes.begin(), exclusiveNodes.end(), std::back_inserter(m_exclusive_peers));

  auto priorityNodes = config.getPriorityNodes();
  std::copy(priorityNodes.begin(), priorityNodes.end(), std::back_inserter(m_priority_peers));

  auto seedNodes = config.getSeedNodes();
  std::copy(seedNodes.begin(), seedNodes.end(), std::back_inserter(m_seed_nodes));

  m_hide_my_port = config.getHideMyPort();
  return true;
}

bool NodeServer::append_net_address(std::vector<NetworkAddress>& nodes, const std::string& addr) {
  size_t pos = addr.find_last_of(':');
  if (!(std::string::npos != pos && addr.length() - 1 != pos && 0 != pos)) {
    logger(Error) << "Failed to parse seed address from string: '" << addr << '\'';
    return false;
  }

  std::string host = addr.substr(0, pos);

  try {
    auto port = Common::fromString<uint16_t>(addr.substr(pos + 1));

    System::Ipv4Resolver resolver(m_dispatcher);
    auto addrHost = resolver.resolve(host);
    nodes.push_back(NetworkAddress{hostToNetwork(addrHost.getValue()), port});

    logger(Trace) << "Added seed node: " << nodes.back() << " (" << host << ")";

  } catch (const std::exception& e) {
    logger(Error, BRIGHT_YELLOW) << "Failed to resolve host name '" << host << "': " << e.what();
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------------

bool NodeServer::init(const NetNodeConfig& config) {
  const auto seedNodes = config.network().seeds();
  for (const auto& seed : seedNodes) {
    append_net_address(m_seed_nodes, seed);
  }

  if (!handleConfig(config)) {
    logger(Error) << "Failed to handle command line";
    return false;
  }
  m_config_folder = config.getConfigFolder();
  m_p2p_state_filename = config.getP2pStateFilename();

  if (!init_config()) {
    logger(Error) << "Failed to init config.";
    return false;
  }

  if (!m_peerlist.init(m_allow_local_ip)) {
    logger(Error) << "Failed to init peerlist.";
    return false;
  }

  for (auto& p : m_command_line_peers)
    m_peerlist.append_with_peer_white(p);

  // only in case if we really sure that we have external visible ip
  m_have_address = true;
  m_ip_address = 0;

#ifdef ALLOW_DEBUG_COMMANDS
  m_last_stat_request_time = 0;
#endif

  // configure self
  // m_net_server.get_config_object().m_pcommands_handler = this;
  // m_net_server.get_config_object().m_invoke_timeout = CryptoNote::P2P_DEFAULT_INVOKE_TIMEOUT;

  // try to bind
  logger(Info) << "Binding on " << m_bind_ip << ":" << m_port;
  m_listeningPort = Common::fromString<uint16_t>(m_port);

  m_listener =
      System::TcpListener(m_dispatcher, System::Ipv4Address(m_bind_ip), static_cast<uint16_t>(m_listeningPort));

  logger(Info) << "Net service binded on " << m_bind_ip << ":" << m_listeningPort;

  if (m_external_port)
    logger(Info) << "External port defined as " << m_external_port;

  addPortMapping(logger, config.appIdentifier(), m_listeningPort, m_external_port ? m_external_port : m_listeningPort);

  return true;
}
//-----------------------------------------------------------------------------------

CryptoNote::CryptoNoteProtocolHandler& NodeServer::get_payload_object() {
  return m_payload_handler;
}
//-----------------------------------------------------------------------------------

bool NodeServer::run() {
  m_currentState.store(Initializing, std::memory_order_release);
  logger(Info) << "Starting node_server";

  m_workingContextGroup.spawn(std::bind(&NodeServer::acceptLoop, this));
  m_workingContextGroup.spawn(std::bind(&NodeServer::onIdle, this));
  m_workingContextGroup.spawn(std::bind(&NodeServer::timedSyncLoop, this));
  m_workingContextGroup.spawn(std::bind(&NodeServer::timeoutLoop, this));
  m_currentState.store(Running, std::memory_order_release);

  m_stopEvent.wait();

  m_currentState.store(ShuttingDown, std::memory_order_release);
  logger(Info) << "Stopping NodeServer and its " << m_connections.size() << " connections...";
  safeInterrupt(m_workingContextGroup);
  m_workingContextGroup.wait();

  logger(Info) << "NodeServer loop stopped";
  m_currentState.store(Stopped, std::memory_order_release);
  return true;
}

std::future<void> NodeServer::runAsync() {
  return std::async(std::launch::async, [this]() {
    if (!this->run()) {
      throw std::runtime_error{"Node server returned none success code."};
    }
  });
}

//-----------------------------------------------------------------------------------

uint64_t NodeServer::get_connections_count() {
  return m_connections.size();
}
//-----------------------------------------------------------------------------------

bool NodeServer::deinit() {
  return store_config();
}

//-----------------------------------------------------------------------------------

bool NodeServer::store_config() {
  try {
    Xi::FileSystem::ensureDirectoryExists(m_config_folder).throwOnError();

    std::string state_file_path = m_config_folder + "/" + m_p2p_state_filename;
    std::ofstream p2p_data;
    p2p_data.open(state_file_path, std::ios_base::binary | std::ios_base::out | std::ios::trunc);
    if (p2p_data.fail()) {
      logger(Fatal) << "Failed to save config to file " << state_file_path;
      return false;
    };

    StdOutputStream stream(p2p_data);
    BinaryOutputStreamSerializer a(stream);
    return CryptoNote::serialize(*this, a);
  } catch (const std::exception& e) {
    logger(Fatal) << "store_config failed: " << e.what();
  }

  return false;
}
//-----------------------------------------------------------------------------------

bool NodeServer::sendStopSignal() {
  m_stop = true;

  m_dispatcher.remoteSpawn([this] {
    m_stopEvent.set();
    m_payload_handler.stop();
  });

  logger(Info, BRIGHT_YELLOW)
      << "Stop signal sent, please only EXIT or CTRL+C one time to avoid stalling the shutdown process.";
  return true;
}

uint32_t NodeServer::get_this_peer_port() {
  return m_listeningPort;
}

//-----------------------------------------------------------------------------------
bool NodeServer::handshake(CryptoNote::LevinProtocol& proto, P2pConnectionContext& context, bool just_take_peerlist) {
  COMMAND_HANDSHAKE::request arg;
  COMMAND_HANDSHAKE::response rsp;
  get_local_node_data(arg.node_data);
  m_payload_handler.get_payload_sync_data(arg.payload_data);

  auto handshakeResult = proto.invoke(COMMAND_HANDSHAKE::ID, arg, rsp);
  if (handshakeResult.isError()) {
    logger(Logging::Debugging)
        << context
        << "A daemon on the network has departed. MSG: Failed to invoke COMMAND_HANDSHAKE, closing connection. ERR: "
        << handshakeResult.error().message();
    return false;
  }

  context.version = rsp.node_data.version;

  if (rsp.node_data.network_id != m_network_id) {
    report_failure(context.m_remote_ip, P2pPenalty::WrongNetworkId);
    logger(Logging::Debugging) << context << "COMMAND_HANDSHAKE Failed, wrong network! (" << rsp.node_data.network_id
                               << "), closing connection.";
    return false;
  }

  if (rsp.node_data.version < Xi::Config::P2P::minimumVersion()) {
    logger(Logging::Debugging) << context << "COMMAND_HANDSHAKE Failed, peer is wrong version! ("
                               << std::to_string(rsp.node_data.version) << "), closing connection.";
    report_failure(context.m_remote_ip, P2pPenalty::DeprecatedVersion);
    return false;
  } else if ((rsp.node_data.version - Xi::Config::P2P::currentVersion()) >=
             Xi::Config::P2P::upgradeNotificationWindow()) {
    logger(Logging::Warning) << context << "COMMAND_HANDSHAKE Warning, your software may be out of date. Please visit: "
                             << m_payload_handler.updateDownloadUrl() << " for the latest version.";
  }

  if (!handle_remote_peerlist(rsp.local_peerlist, rsp.node_data.local_time, context)) {
    logger(Logging::Error) << context
                           << "COMMAND_HANDSHAKE: failed to handle_remote_peerlist(...), closing connection.";
    report_failure(context.m_remote_ip, P2pPenalty::HandshakeFailure);
    return false;
  }

  if (just_take_peerlist) {
    return true;
  }

  if (!m_payload_handler.process_payload_sync_data(rsp.payload_data, context, true)) {
    logger(Logging::Error)
        << context << "COMMAND_HANDSHAKE invoked, but process_payload_sync_data returned false, dropping connection.";
    report_failure(context.m_remote_ip, P2pPenalty::HandshakeFailure);
    return false;
  }

  context.peerId = rsp.node_data.peer_id;
  m_peerlist.set_peer_just_seen(rsp.node_data.peer_id, context.m_remote_ip, context.m_remote_port);

  if (rsp.node_data.peer_id == m_config.m_peer_id) {
    logger(Logging::Trace) << context << "Connection to self detected, dropping connection";
    return false;
  }

  logger(Logging::Debugging) << context << "COMMAND_HANDSHAKE INVOKED OK";
  return true;
}

bool NodeServer::timedSync() {
  COMMAND_TIMED_SYNC::request arg = boost::value_initialized<COMMAND_TIMED_SYNC::request>();
  m_payload_handler.get_payload_sync_data(arg.payload_data);
  auto cmdBuf = LevinProtocol::encode<COMMAND_TIMED_SYNC::request>(arg);

  forEachConnection([&](P2pConnectionContext& conn) {
    if (conn.peerId && (conn.m_state == CryptoNoteConnectionContext::state_normal ||
                        conn.m_state == CryptoNoteConnectionContext::state_idle)) {
      conn.pushMessage(P2pMessage(P2pMessage::COMMAND, COMMAND_TIMED_SYNC::ID, cmdBuf));
    }
  });

  return true;
}

bool NodeServer::handleTimedSyncResponse(const BinaryArray& in, P2pConnectionContext& context) {
  COMMAND_TIMED_SYNC::response rsp;
  auto decodeResult = LevinProtocol::decode<COMMAND_TIMED_SYNC::response>(in, rsp);

  if (decodeResult.isError()) {
    logger(Logging::Error) << context
                           << "COMMAND_TIMED_SYNC: failed to decode blob, error: " << decodeResult.error().message();
    report_failure(context.m_remote_ip, P2pPenalty::HandshakeFailure);
    return false;
  }

  if (!handle_remote_peerlist(rsp.local_peerlist, rsp.local_time, context)) {
    logger(Logging::Error) << context
                           << "COMMAND_TIMED_SYNC: failed to handle_remote_peerlist(...), closing connection.";
    report_failure(context.m_remote_ip, P2pPenalty::HandshakeFailure);
    return false;
  }

  if (!context.m_is_income) {
    m_peerlist.set_peer_just_seen(context.peerId, context.m_remote_ip, context.m_remote_port);
  }

  if (!m_payload_handler.process_payload_sync_data(rsp.payload_data, context, false)) {
    return false;
  }

  return true;
}

void NodeServer::forEachConnection(std::function<void(P2pConnectionContext&)> action) {
  // create copy of connection ids because the list can be changed during action
  std::vector<boost::uuids::uuid> connectionIds;
  connectionIds.reserve(m_connections.size());
  for (const auto& c : m_connections) {
    connectionIds.push_back(c.first);
  }

  for (const auto& connId : connectionIds) {
    auto it = m_connections.find(connId);
    if (it != m_connections.end()) {
      action(it->second);
    }
  }
}

//-----------------------------------------------------------------------------------
bool NodeServer::is_peer_used(const PeerlistEntry& peer) {
  if (m_config.m_peer_id == peer.id)
    return true;  // dont make connections to ourself

  for (const auto& kv : m_connections) {
    const auto& cntxt = kv.second;
    if (cntxt.peerId == peer.id ||
        (!cntxt.m_is_income && peer.address.ip == cntxt.m_remote_ip && peer.address.port == cntxt.m_remote_port)) {
      return true;
    }
  }
  return false;
}
//-----------------------------------------------------------------------------------

bool NodeServer::is_addr_connected(const NetworkAddress& peer) {
  for (const auto& conn : m_connections) {
    if (!conn.second.m_is_income && peer.ip == conn.second.m_remote_ip && peer.port == conn.second.m_remote_port) {
      return true;
    }
  }
  return false;
}

bool NodeServer::try_to_connect_and_handshake_with_new_peer(const NetworkAddress& na, bool just_take_peerlist,
                                                            uint64_t last_seen_stamp, bool white) {
  if (is_ip_address_blocked(na.ip)) {
    logger(Debugging) << "Peer is blocked: " << na;
    return false;
  }

  logger(Debugging) << "Connecting to " << na << " (white=" << white << ", last_seen: "
                    << (last_seen_stamp ? Common::timeIntervalToString(time(NULL) - last_seen_stamp) : "never")
                    << ")...";

  try {
    System::TcpConnection connection;

    try {
      System::Context<System::TcpConnection> connectionContext(m_dispatcher, [&] {
        System::TcpConnector connector(m_dispatcher);
        return connector.connect(System::Ipv4Address(Common::ipAddressToString(na.ip)), static_cast<uint16_t>(na.port));
      });

      System::Context<> timeoutContext(m_dispatcher, [&] {
        System::Timer(m_dispatcher).sleep(std::chrono::milliseconds(m_config.m_net_config.connection_timeout));
        logger(Debugging) << "Connection to " << na << " timed out, interrupt it";
        safeInterrupt(connectionContext);
      });

      connection = std::move(connectionContext.get());
    } catch (System::InterruptedException&) {
      logger(Debugging) << "Connection timed out";
      return false;
    } catch (std::exception& e) {
      logger(Debugging) << "Connection to " << Common::ipAddressToString(na.ip) << " failed: " << e.what();
      throw e;
    }

    P2pConnectionContext ctx(m_dispatcher, logger.getLogger(), std::move(connection));

    ctx.m_connection_id = boost::uuids::random_generator()();
    ctx.m_remote_ip = na.ip;
    ctx.m_remote_port = na.port;
    ctx.m_is_income = false;
    ctx.m_is_light_node = true;
    ctx.m_started = time(nullptr);

    try {
      System::Context<bool> handshakeContext(m_dispatcher, [&] {
        CryptoNote::LevinProtocol proto(ctx.connection);
        return handshake(proto, ctx, just_take_peerlist);
      });

      System::Context<> timeoutContext(m_dispatcher, [&] {
        // Here we use connection_timeout * 3, one for this handshake, and two for back ping from peer.
        System::Timer(m_dispatcher).sleep(std::chrono::milliseconds(m_config.m_net_config.connection_timeout * 3));
        logger(Debugging) << "Handshake with " << na << " timed out, interrupt it";
        safeInterrupt(handshakeContext);
      });

      if (!handshakeContext.get()) {
        logger(Debugging) << "Failed to HANDSHAKE with peer " << na;
        return false;
      }
    } catch (System::InterruptedException&) {
      logger(Debugging) << "Handshake timed out";
      return false;
    }

    if (just_take_peerlist) {
      logger(Logging::Debugging) << ctx << "CONNECTION HANDSHAKED OK AND CLOSED.";
      return true;
    }

    PeerlistEntry pe_local = boost::value_initialized<PeerlistEntry>();
    pe_local.address = na;
    pe_local.id = ctx.peerId;
    pe_local.last_seen = time(nullptr);
    pe_local.is_light_node = ctx.m_is_light_node;
    m_peerlist.append_with_peer_white(pe_local);

    if (m_stop) {
      throw System::InterruptedException();
    }

    auto iter = m_connections.emplace(ctx.m_connection_id, std::move(ctx)).first;
    const boost::uuids::uuid& connectionId = iter->first;
    P2pConnectionContext& connectionContext = iter->second;

    m_workingContextGroup.spawn(
        std::bind(&NodeServer::connectionHandler, this, std::cref(connectionId), std::ref(connectionContext)));

    return true;
  } catch (System::InterruptedException&) {
    logger(Debugging) << "Connection process interrupted";
    throw;
  } catch (const std::exception& e) {
    logger(Debugging) << "Connection to " << na << " failed: " << e.what();
  }

  return false;
}

//-----------------------------------------------------------------------------------
bool NodeServer::make_new_connection_from_peerlist(bool use_white_list) {
  size_t local_peers_count = use_white_list ? m_peerlist.get_white_peers_count() : m_peerlist.get_gray_peers_count();
  if (!local_peers_count)
    return false;  // no peers

  size_t max_random_index = std::min<uint64_t>(local_peers_count - 1, 20);

  std::set<size_t> tried_peers;

  size_t try_count = 0;
  size_t rand_count = 0;
  while (rand_count < (max_random_index + 1) * 3 && try_count < 10 && !m_stop) {
    ++rand_count;
    size_t random_index = get_random_index_with_fixed_probability(max_random_index);
    if (!(random_index < local_peers_count)) {
      logger(Error) << "random_starter_index < peers_local.size() failed!!";
      return false;
    }

    if (tried_peers.count(random_index))
      continue;

    tried_peers.insert(random_index);
    PeerlistEntry pe = boost::value_initialized<PeerlistEntry>();
    bool r = use_white_list ? m_peerlist.get_white_peer_by_index(pe, random_index)
                            : m_peerlist.get_gray_peer_by_index(pe, random_index);
    if (!(r)) {
      logger(Error) << "Failed to get random peer from peerlist(white:" << use_white_list << ")";
      return false;
    }

    ++try_count;

    if (is_peer_used(pe))
      continue;

    logger(Debugging) << "Selected peer: " << pe.id << " " << pe.address << " [white=" << use_white_list
                      << "] last_seen: "
                      << (pe.last_seen ? Common::timeIntervalToString(time(NULL) - pe.last_seen) : "never");

    if (!try_to_connect_and_handshake_with_new_peer(pe.address, false, pe.last_seen, use_white_list))
      continue;

    return true;
  }
  return false;
}
//-----------------------------------------------------------------------------------

bool NodeServer::connections_maker() {
  if (!connect_to_peerlist(m_exclusive_peers)) {
    return false;
  }

  if (!m_exclusive_peers.empty()) {
    return true;
  }

  if (!m_peerlist.get_white_peers_count() && m_seed_nodes.size()) {
    size_t try_count = 0;

    size_t current_index = 0;
    XI_RETURN_EC_IF_NOT(Xi::Crypto::Random::generate(Xi::asByteSpan(&current_index, sizeof(size_t))) ==
                            Xi::Crypto::Random::RandomError::Success,
                        false);
    current_index = current_index % m_seed_nodes.size();

    while (true) {
      if (!isBlocked(m_seed_nodes[current_index].ip) &&
          try_to_connect_and_handshake_with_new_peer(m_seed_nodes[current_index], true))
        break;

      if (++try_count > m_seed_nodes.size()) {
        logger(Error) << "Failed to connect to any of seed peers, continuing without seeds";
        break;
      }
      if (++current_index >= m_seed_nodes.size())
        current_index = 0;
    }
  }

  if (!connect_to_peerlist(m_priority_peers))
    return false;

  size_t expected_white_connections =
      (m_config.m_net_config.connections_count * Xi::Config::P2P::whiteListPreferenceThreshold()) / 100;

  size_t conn_count = get_outgoing_connections_count();
  if (conn_count < m_config.m_net_config.connections_count) {
    if (conn_count < expected_white_connections) {
      // start from white list
      if (!make_expected_connections_count(true, expected_white_connections))
        return false;
      // and then do grey list
      if (!make_expected_connections_count(false, m_config.m_net_config.connections_count))
        return false;
    } else {
      // start from grey list
      if (!make_expected_connections_count(false, m_config.m_net_config.connections_count))
        return false;
      // and then do white list
      if (!make_expected_connections_count(true, m_config.m_net_config.connections_count))
        return false;
    }
  }

  return true;
}
//-----------------------------------------------------------------------------------

bool NodeServer::make_expected_connections_count(bool white_list, size_t expected_connections) {
  size_t conn_count = get_outgoing_connections_count();
  // add new connections from white peers
  while (conn_count < expected_connections) {
    if (m_stopEvent.get())
      return false;

    if (!make_new_connection_from_peerlist(white_list))
      break;
    conn_count = get_outgoing_connections_count();
  }
  return true;
}

//-----------------------------------------------------------------------------------
size_t NodeServer::get_outgoing_connections_count() {
  size_t count = 0;
  for (const auto& cntxt : m_connections) {
    if (!cntxt.second.m_is_income)
      ++count;
  }
  return count;
}

//-----------------------------------------------------------------------------------
bool NodeServer::idle_worker() {
  try {
    m_connections_maker_interval.call(std::bind(&NodeServer::connections_maker, this));
    m_peerlist_store_interval.call(std::bind(&NodeServer::store_config, this));
  } catch (std::exception& e) {
    logger(Debugging) << "exception in idle_worker: " << e.what();
  }
  return true;
}

//-----------------------------------------------------------------------------------
bool NodeServer::fix_time_delta(std::list<PeerlistEntry>& local_peerlist, time_t local_time, int64_t& delta) {
  // fix time delta
  time_t now = 0;
  time(&now);
  delta = now - local_time;

  BOOST_FOREACH (PeerlistEntry& be, local_peerlist) {
    if (be.last_seen > uint64_t(local_time)) {
      logger(Error) << "FOUND FUTURE peerlist for entry " << be.address << " last_seen: " << be.last_seen
                    << ", local_time(on remote node):" << local_time;
      return false;
    }
    be.last_seen += delta;
  }
  return true;
}

//-----------------------------------------------------------------------------------

bool NodeServer::handle_remote_peerlist(const std::list<PeerlistEntry>& peerlist, time_t local_time,
                                        const CryptoNoteConnectionContext& context) {
  int64_t delta = 0;
  std::list<PeerlistEntry> peerlist_ = peerlist;
  if (!fix_time_delta(peerlist_, local_time, delta))
    return false;
  logger(Logging::Trace) << context << "REMOTE PEERLIST: TIME_DELTA: " << delta
                         << ", remote peerlist size=" << peerlist_.size();
  logger(Logging::Trace) << context << "REMOTE PEERLIST: " << print_peerlist_to_string(peerlist_);
  return m_peerlist.merge_peerlist(peerlist_);
}
//-----------------------------------------------------------------------------------

bool NodeServer::get_local_node_data(basic_node_data& node_data) {
  node_data.version = Xi::Config::P2P::currentVersion();
  time_t local_time;
  time(&local_time);
  node_data.local_time = local_time;
  node_data.peer_id = m_config.m_peer_id;
  if (!m_hide_my_port)
    node_data.my_port = m_external_port ? m_external_port : m_listeningPort;
  else
    node_data.my_port = 0;
  node_data.network_id = m_network_id;
  return true;
}
//-----------------------------------------------------------------------------------
#ifdef ALLOW_DEBUG_COMMANDS

bool NodeServer::check_trust(const proof_of_trust& tr) {
  uint64_t local_time = time(NULL);
  uint64_t time_delata = local_time > tr.time ? local_time - tr.time : tr.time - local_time;

  if (time_delata > 24 * 60 * 60) {
    logger(Error) << "check_trust failed to check time conditions, local_time=" << local_time
                  << ", proof_time=" << tr.time;
    return false;
  }

  if (m_last_stat_request_time >= tr.time) {
    logger(Error) << "check_trust failed to check time conditions, last_stat_request_time=" << m_last_stat_request_time
                  << ", proof_time=" << tr.time;
    return false;
  }

  if (m_config.m_peer_id != tr.peer_id) {
    logger(Error) << "check_trust failed: peer_id mismatch (passed " << tr.peer_id << ", expected "
                  << m_config.m_peer_id << ")";
    return false;
  }

  Crypto::PublicKey pk;
  Common::podFromHex(Xi::Config::P2P::trustedPublicKey(), pk);
  Crypto::Hash h = get_proof_of_trust_hash(tr);
  if (!Crypto::check_signature(h, pk, tr.sign)) {
    logger(Error) << "check_trust failed: sign check failed";
    return false;
  }

  // update last request time
  m_last_stat_request_time = tr.time;
  return true;
}
//-----------------------------------------------------------------------------------

int NodeServer::handle_get_stat_info(int command, COMMAND_REQUEST_STAT_INFO::request& arg,
                                     COMMAND_REQUEST_STAT_INFO::response& rsp, P2pConnectionContext& context) {
  if (!check_trust(arg.tr)) {
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }
  rsp.connections_count = get_connections_count();
  rsp.incoming_connections_count = rsp.connections_count - get_outgoing_connections_count();
  rsp.version = PROJECT_VERSION_LONG;
  rsp.os_version = Tools::get_os_version_string();
  rsp.payload_info = m_payload_handler.getStatistics();
  return 1;
}
//-----------------------------------------------------------------------------------

int NodeServer::handle_get_network_state(int command, COMMAND_REQUEST_NETWORK_STATE::request& arg,
                                         COMMAND_REQUEST_NETWORK_STATE::response& rsp, P2pConnectionContext& context) {
  if (!check_trust(arg.tr)) {
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  for (const auto& cntxt : m_connections) {
    connection_entry ce;
    ce.adr.ip = cntxt.second.m_remote_ip;
    ce.adr.port = cntxt.second.m_remote_port;
    ce.id = cntxt.second.peerId;
    ce.is_income = cntxt.second.m_is_income;
    rsp.connections_list.push_back(ce);
  }

  m_peerlist.get_peerlist_full(rsp.local_peerlist_gray, rsp.local_peerlist_white);
  rsp.my_id = m_config.m_peer_id;
  rsp.local_time = time(NULL);
  return 1;
}
//-----------------------------------------------------------------------------------

int NodeServer::handle_get_peer_id(int command, COMMAND_REQUEST_PEER_ID::request& arg,
                                   COMMAND_REQUEST_PEER_ID::response& rsp, P2pConnectionContext& context) {
  rsp.my_id = m_config.m_peer_id;
  return 1;
}
#endif

//-----------------------------------------------------------------------------------

void NodeServer::relay_notify_to_all(int command, const BinaryArray& data_buff,
                                     const net_connection_id* excludeConnection) {
  net_connection_id excludeId = excludeConnection ? *excludeConnection : boost::value_initialized<net_connection_id>();

  forEachConnection([&](P2pConnectionContext& conn) {
    if (conn.peerId && conn.m_connection_id != excludeId &&
        (conn.m_state == CryptoNoteConnectionContext::state_normal ||
         conn.m_state == CryptoNoteConnectionContext::state_synchronizing)) {
      conn.pushMessage(P2pMessage(P2pMessage::NOTIFY, command, data_buff));
    }
  });
}

//-----------------------------------------------------------------------------------
bool NodeServer::invoke_notify_to_peer(int command, const BinaryArray& buffer,
                                       const CryptoNoteConnectionContext& context) {
  auto it = m_connections.find(context.m_connection_id);
  if (it == m_connections.end()) {
    return false;
  }

  it->second.pushMessage(P2pMessage(P2pMessage::NOTIFY, command, buffer));

  return true;
}

//-----------------------------------------------------------------------------------
bool NodeServer::try_ping(basic_node_data& node_data, P2pConnectionContext& context) {
  if (!node_data.my_port) {
    return false;
  }

  uint32_t actual_ip = context.m_remote_ip;
  if (!m_peerlist.is_ip_allowed(actual_ip)) {
    return false;
  }

  auto ip = Common::ipAddressToString(actual_ip);
  auto port = node_data.my_port;
  auto peerId = node_data.peer_id;

  try {
    COMMAND_PING::request req;
    COMMAND_PING::response rsp;
    System::Context<Xi::Result<void>> pingContext(m_dispatcher, [&]() -> Xi::Result<void> {
      XI_ERROR_TRY();
      System::TcpConnector connector(m_dispatcher);
      auto connection = connector.connect(System::Ipv4Address(ip), static_cast<uint16_t>(port));
      return LevinProtocol(connection).invoke(COMMAND_PING::ID, req, rsp);
      XI_ERROR_CATCH();
    });

    System::Context<> timeoutContext(m_dispatcher, [&] {
      System::Timer(m_dispatcher).sleep(std::chrono::milliseconds(m_config.m_net_config.connection_timeout * 2));
      logger(Debugging) << context << "Back ping timed out" << ip << ":" << port;
      safeInterrupt(pingContext);
    });

    auto pingResult = pingContext.get();
    if (pingResult.isError()) {
      logger(Debugging) << context << "ping faild \" from" << ip << ":" << port << ", hsh_peer_id=" << peerId
                        << ", error=" << pingResult.error().message();
      return false;
    }

    if (rsp.status != PING_OK_RESPONSE_STATUS_TEXT || peerId != rsp.peer_id) {
      logger(Debugging) << context << "Back ping invoke wrong response \"" << rsp.status << "\" from" << ip << ":"
                        << port << ", hsh_peer_id=" << peerId << ", rsp.peer_id=" << rsp.peer_id;
      return false;
    }
  } catch (std::exception& e) {
    logger(Debugging) << context << "Back ping connection to " << ip << ":" << port << " failed: " << e.what();
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------------
int NodeServer::handle_timed_sync(int command, COMMAND_TIMED_SYNC::request& arg, COMMAND_TIMED_SYNC::response& rsp,
                                  P2pConnectionContext& context) {
  XI_UNUSED(command);
  if (!m_payload_handler.process_payload_sync_data(arg.payload_data, context, false)) {
    logger(Logging::Error) << context << "Failed to process_payload_sync_data(), dropping connection";
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  // fill response
  rsp.local_time = time(NULL);
  m_peerlist.get_peerlist_head(rsp.local_peerlist);
  m_payload_handler.get_payload_sync_data(rsp.payload_data);
  logger(Logging::Trace) << context << "COMMAND_TIMED_SYNC";
  return 1;
}
//-----------------------------------------------------------------------------------

int NodeServer::handle_handshake(int command, COMMAND_HANDSHAKE::request& arg, COMMAND_HANDSHAKE::response& rsp,
                                 P2pConnectionContext& context) {
  XI_UNUSED(command);
  context.version = arg.node_data.version;

  if (arg.node_data.network_id != m_network_id) {
    add_host_fail(context.m_remote_ip, P2pPenalty::WrongNetworkId);
    logger(Logging::Debugging) << context << "WRONG NETWORK AGENT CONNECTED! id=" << arg.node_data.network_id;
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  if (arg.node_data.version < Xi::Config::P2P::minimumVersion()) {
    logger(Logging::Debugging) << context << "UNSUPPORTED NETWORK AGENT VERSION CONNECTED! version="
                               << std::to_string(arg.node_data.version);
    add_host_fail(context.m_remote_ip, P2pPenalty::DeprecatedVersion);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  } else if (arg.node_data.version > Xi::Config::P2P::currentVersion()) {
    logger(Logging::Warning) << context << "Our software may be out of date. Please visit: "
                             << m_payload_handler.updateDownloadUrl() << " for the latest version.";
  }

  if (!context.m_is_income) {
    add_host_fail(context.m_remote_ip, P2pPenalty::IncomeViolation);
    logger(Logging::Error) << context << "COMMAND_HANDSHAKE came not from incoming connection";
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  if (context.peerId) {
    logger(Logging::Error) << context
                           << "COMMAND_HANDSHAKE came, but seems that connection already have associated peer_id "
                              "(double COMMAND_HANDSHAKE?)";
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  if (!m_payload_handler.process_payload_sync_data(arg.payload_data, context, true)) {
    add_host_fail(context.m_remote_ip, P2pPenalty::HandshakeFailure);
    logger(Logging::Error)
        << context << "COMMAND_HANDSHAKE came, but process_payload_sync_data returned false, dropping connection.";
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }
  // associate peer_id with this connection
  context.peerId = arg.node_data.peer_id;
  context.m_is_light_node = arg.payload_data.is_light_node;

  if (arg.node_data.peer_id != m_config.m_peer_id && arg.node_data.my_port) {
    PeerIdType peer_id_l = arg.node_data.peer_id;
    uint16_t port_l = arg.node_data.my_port;

    if (try_ping(arg.node_data, context)) {
      // called only(!) if success pinged, update local peerlist
      PeerlistEntry pe;
      pe.address.ip = context.m_remote_ip;
      pe.address.port = port_l;
      pe.last_seen = time(nullptr);
      pe.is_light_node = context.m_is_light_node;
      pe.id = peer_id_l;
      m_peerlist.append_with_peer_white(pe);

      logger(Logging::Trace) << context << "BACK PING SUCCESS, " << Common::ipAddressToString(context.m_remote_ip)
                             << ":" << port_l << " added to whitelist";
    }
  }

  // fill response
  m_peerlist.get_peerlist_head(rsp.local_peerlist);
  get_local_node_data(rsp.node_data);
  m_payload_handler.get_payload_sync_data(rsp.payload_data);

  logger(Logging::Debugging) << "COMMAND_HANDSHAKE";
  return 1;
}
//-----------------------------------------------------------------------------------

int NodeServer::handle_ping(int command, COMMAND_PING::request& arg, COMMAND_PING::response& rsp,
                            P2pConnectionContext& context) {
  XI_UNUSED(command, arg);
  logger(Logging::Trace) << context << "COMMAND_PING";
  rsp.status = PING_OK_RESPONSE_STATUS_TEXT;
  rsp.peer_id = m_config.m_peer_id;
  return 1;
}
//-----------------------------------------------------------------------------------

bool NodeServer::log_peerlist() {
  std::list<PeerlistEntry> pl_wite;
  std::list<PeerlistEntry> pl_gray;
  m_peerlist.get_peerlist_full(pl_gray, pl_wite);
  logger(Info) << ENDL << "Peerlist white:" << ENDL << print_peerlist_to_string(pl_wite) << ENDL
               << "Peerlist gray:" << ENDL << print_peerlist_to_string(pl_gray);
  return true;
}
//-----------------------------------------------------------------------------------

bool NodeServer::log_connections() {
  logger(Info) << "Connections: \r\n" << print_connections_container();
  return true;
}
//-----------------------------------------------------------------------------------

std::string NodeServer::print_connections_container() {
  std::stringstream ss;

  for (const auto& cntxt : m_connections) {
    ss << Common::ipAddressToString(cntxt.second.m_remote_ip) << ":" << cntxt.second.m_remote_port << " \t\tpeer_id "
       << cntxt.second.peerId << " \t\tconn_id " << cntxt.second.m_connection_id
       << (cntxt.second.m_is_income ? " INCOMING" : " OUTGOING") << std::endl;
  }

  return ss.str();
}
//-----------------------------------------------------------------------------------

void NodeServer::on_connection_new(P2pConnectionContext& context) {
  m_payload_handler.onConnectionOpened(context);
}
//-----------------------------------------------------------------------------------

void NodeServer::on_connection_close(P2pConnectionContext& context) {
  logger(Trace) << context << "CLOSE CONNECTION";
  m_payload_handler.onConnectionClosed(context);
}

bool NodeServer::is_priority_node(const NetworkAddress& na) {
  return (std::find(m_priority_peers.begin(), m_priority_peers.end(), na) != m_priority_peers.end()) ||
         (std::find(m_exclusive_peers.begin(), m_exclusive_peers.end(), na) != m_exclusive_peers.end());
}

bool NodeServer::connect_to_peerlist(const std::vector<NetworkAddress>& peers) {
  for (const auto& na : peers) {
    if (!is_addr_connected(na) && !isBlocked(na.ip)) {
      try_to_connect_and_handshake_with_new_peer(na);
    }
  }

  return true;
}

bool NodeServer::parse_peers_and_add_to_container(const boost::program_options::variables_map& vm,
                                                  const command_line::arg_descriptor<std::vector<std::string>>& arg,
                                                  std::vector<NetworkAddress>& container) {
  std::vector<std::string> perrs = command_line::get_arg(vm, arg);

  for (const std::string& pr_str : perrs) {
    NetworkAddress na;
    if (!parse_peer_from_string(na, pr_str)) {
      logger(Error) << "Failed to parse address from string: " << pr_str;
      return false;
    }
    container.push_back(na);
  }

  return true;
}

void NodeServer::acceptLoop() {
  while (!m_stop) {
    try {
      P2pConnectionContext ctx(m_dispatcher, logger.getLogger(), m_listener.accept());
      ctx.m_connection_id = boost::uuids::random_generator()();
      ctx.m_is_income = true;
      ctx.m_started = time(nullptr);

      auto addressAndPort = ctx.connection.getPeerAddressAndPort();
      ctx.m_remote_ip = hostToNetwork(addressAndPort.first.getValue());
      ctx.m_remote_port = addressAndPort.second;
      ctx.m_is_light_node = true;

      if (!isBlocked(ctx.m_remote_ip)) {
        auto iter = m_connections.emplace(ctx.m_connection_id, std::move(ctx)).first;
        const boost::uuids::uuid& connectionId = iter->first;
        P2pConnectionContext& connection = iter->second;

        m_workingContextGroup.spawn(
            std::bind(&NodeServer::connectionHandler, this, std::cref(connectionId), std::ref(connection)));
      } else {
        logger(Debugging) << ctx << " tried to connect but is banned.";
      }
    } catch (System::InterruptedException&) {
      logger(Debugging) << "acceptLoop() is interrupted";
      break;
    } catch (const std::exception& e) {
      logger(Debugging) << "Exception in acceptLoop: " << e.what();
    }
  }

  logger(Debugging) << "acceptLoop finished";
}

void NodeServer::onIdle() {
  logger(Debugging) << "onIdle started";

  while (!m_stop) {
    try {
      idle_worker();
      m_idleTimer.sleep(std::chrono::seconds(1));
    } catch (System::InterruptedException&) {
      logger(Debugging) << "onIdle() is interrupted";
      break;
    } catch (std::exception& e) {
      logger(Warning) << "Exception in onIdle: " << e.what();
    }
  }

  logger(Debugging) << "onIdle finished";
}

void NodeServer::timeoutLoop() {
  try {
    while (!m_stop) {
      m_timeoutTimer.sleep(std::chrono::seconds(10));
      auto now = P2pConnectionContext::Clock::now();

      for (auto& kv : m_connections) {
        auto& ctx = kv.second;
        if (ctx.writeDuration(now) >
            static_cast<uint64_t>(std::chrono::milliseconds{Xi::Config::P2P::invokeTimeout()}.count())) {
          logger(Debugging) << ctx << "write operation timed out, stopping connection";
          safeInterrupt(ctx);
        }
      }
    }
  } catch (System::InterruptedException&) {
    logger(Debugging) << "timeoutLoop() is interrupted";
  } catch (std::exception& e) {
    logger(Warning) << "Exception in timeoutLoop: " << e.what();
  } catch (...) {
    logger(Warning) << "Unknown exception in timeoutLoop";
  }
}
bool NodeServer::block_host(const uint32_t address_ip, std::chrono::seconds seconds) {
  XI_CONCURRENT_RLOCK(m_block_access);
  m_blocked_hosts[address_ip] = time(nullptr) + seconds.count();

  // drop any connection to that IP
  std::list<boost::uuids::uuid> conns;
  forEachConnection([&](P2pConnectionContext& cntxt) {
    if (cntxt.m_remote_ip == address_ip) {
      conns.push_back(cntxt.m_connection_id);
    }
    return true;
  });
  for (const auto& c_id : conns) {
    auto c = m_connections.find(c_id);
    if (c != m_connections.end())
      c->second.m_state = CryptoNoteConnectionContext::state_shutdown;
  }

  logger(Info) << "Host " << Common::ipAddressToString(address_ip) << " blocked.";
  return true;
}

bool NodeServer::unblock_host(const uint32_t address_ip) {
  XI_CONCURRENT_RLOCK(m_block_access);
  auto i = m_blocked_hosts.find(address_ip);
  if (i == m_blocked_hosts.end())
    return false;
  m_blocked_hosts.erase(i);
  auto penaltySearch = m_host_fails_score.find(address_ip);
  if (penaltySearch != m_host_fails_score.end()) {
    m_host_fails_score.erase(penaltySearch);
  }
  logger(Info) << "Host " << Common::ipAddressToString(address_ip) << " unblocked.";
  return true;
}

bool NodeServer::add_host_fail(const uint32_t address_ip, P2pPenalty penalty) {
  const auto penalityWeight = p2pPenaltyWeight(penalty);
  if (penalityWeight < 1) {
    return false;
  }

  XI_CONCURRENT_RLOCK(m_block_access);
  if (m_host_fails_score.find(address_ip) == m_host_fails_score.end()) {
    m_host_fails_score[address_ip] = penalityWeight;
  } else {
    m_host_fails_score[address_ip] += penalityWeight;
  }
  uint64_t failScore = m_host_fails_score[address_ip];
  logger(Debugging) << "Host " << Common::ipAddressToString(address_ip) << ", fail score=" << failScore
                    << ", penalty: " << p2pPeanaltyMessage(penalty);
  if (failScore > m_fails_before_block) {
    auto it = m_host_fails_score.find(address_ip);
    if (it == m_host_fails_score.end()) {
      logger(Debugging) << "Internal error (add_host_fail)" << failScore;
      return false;
    }
    it->second = m_fails_before_block / 2;
    if (isAutoBlockEnabled()) {
      block_host(address_ip, m_block_time);
      return true;
    } else {
      logger(Debugging) << "Auto blocking disabled, skipping host block.";
      return false;
    }
  }
  return false;
}

void NodeServer::add_host_success(const uint32_t address_ip) {
  XI_CONCURRENT_RLOCK(m_block_access);
  auto search_block = m_host_fails_score.find(address_ip);
  if (search_block == m_host_fails_score.end()) {
    return;
  }
  if (search_block->second <= 1) {
    m_host_fails_score.erase(search_block);
    logger(Debugging) << "Host " << Common::ipAddressToString(address_ip) << " has no fail score anymore.";
  } else {
    m_host_fails_score[address_ip] -= 1;
    logger(Debugging) << "Host " << Common::ipAddressToString(address_ip)
                      << " fail score=" << m_host_fails_score[address_ip];
  }
}

bool NodeServer::evaluate_blocked_connection(const uint32_t address_ip) {
  XI_CONCURRENT_RLOCK(m_block_access);
  auto search = m_blocked_hosts.find(address_ip);
  if (search == m_blocked_hosts.end()) {
    return false;
  } else if (search->second > time(nullptr)) {
    return true;
  } else {
    unblock_host(address_ip);
    return false;
  }
}

std::map<uint32_t, int64_t> NodeServer::blockedPeers() const {
  XI_CONCURRENT_RLOCK(m_block_access);
  return m_blocked_hosts;
}

std::map<uint32_t, uint64_t> NodeServer::peerPenalties() const {
  XI_CONCURRENT_RLOCK(m_block_access);
  return m_host_fails_score;
}

size_t NodeServer::banIps(const std::vector<uint32_t>& ips) {
  XI_CONCURRENT_RLOCK(m_block_access);
  size_t count = 0;
  for (const auto& ip : ips) {
    auto search = m_blocked_hosts.find(ip);
    if (search == m_blocked_hosts.end()) {
      count += 1;
    }
    block_host(ip, m_block_time);
  }
  return count;
}

size_t NodeServer::unbanIps(const std::vector<uint32_t>& ips) {
  XI_CONCURRENT_RLOCK(m_block_access);
  size_t count = 0;
  for (const auto& ip : ips) {
    auto search = m_blocked_hosts.find(ip);
    if (search != m_blocked_hosts.end()) {
      unblock_host(ip);
      count += 1;
    }
  }
  return count;
}

size_t NodeServer::unbanAllIps() {
  XI_CONCURRENT_RLOCK(m_block_access);
  size_t count = m_blocked_hosts.size();
  while (!m_blocked_hosts.empty()) {
    unblock_host(m_blocked_hosts.begin()->first);
  }
  return count;
}

bool NodeServer::isBlocked(uint32_t ip) const {
  XI_CONCURRENT_RLOCK(m_block_access);
  auto search = m_blocked_hosts.find(ip);
  if (search != m_blocked_hosts.end()) {
    return search->second > time(nullptr);
  } else {
    return false;
  }
}

size_t NodeServer::resetPenalties() {
  XI_CONCURRENT_RLOCK(m_block_access);
  size_t count = m_host_fails_score.size();
  m_host_fails_score.clear();
  return count;
}

bool NodeServer::isAutoBlockEnabled() const {
  return m_auto_block.load();
}

void NodeServer::setAutoBlockEnabled(const bool isEnabled) {
  m_auto_block.store(isEnabled);
}

void NodeServer::timedSyncLoop() {
  try {
    for (;;) {
      m_timedSyncTimer.sleep(std::chrono::seconds(Xi::Config::P2P::handshakeInterval()));
      timedSync();
    }
  } catch (System::InterruptedException&) {
    logger(Debugging) << "timedSyncLoop() is interrupted";
  } catch (std::exception& e) {
    logger(Warning) << "Exception in timedSyncLoop: " << e.what();
  }

  logger(Debugging) << "timedSyncLoop finished";
}

void NodeServer::connectionHandler(const boost::uuids::uuid& connectionId, P2pConnectionContext& ctx) {
  // This inner context is necessary in order to stop connection handler at any moment
  System::Context<> context(m_dispatcher, [this, &connectionId, &ctx] {
    System::Context<> writeContext(m_dispatcher, std::bind(&NodeServer::writeHandler, this, std::ref(ctx)));

    try {
      on_connection_new(ctx);

      LevinProtocol proto(ctx.connection);
      LevinProtocol::Command cmd;

      if (is_ip_address_blocked(ctx.m_remote_ip)) {
        ctx.m_state = CryptoNoteConnectionContext::state_shutdown;
        logger(Debugging) << ctx << " tried to connect but is still blocked.";
      }

      while (ctx.m_state != CryptoNoteConnectionContext::state_shutdown) {
        if (ctx.m_state == CryptoNoteConnectionContext::state_sync_required) {
          ctx.m_state = CryptoNoteConnectionContext::state_synchronizing;
          m_payload_handler.start_sync(ctx);
        } else if (ctx.m_state == CryptoNoteConnectionContext::state_pool_sync_required) {
          ctx.m_state = CryptoNoteConnectionContext::state_normal;
          m_payload_handler.requestMissingPoolTransactions(ctx);
        }

        if (!proto.readCommand(cmd)) {
          break;
        }

        BinaryArray response;
        bool handled = false;
        auto retcode = handleCommand(cmd, response, ctx, handled);

        // send response
        if (cmd.needReply()) {
          if (!handled) {
            retcode = static_cast<int32_t>(LevinError::ERROR_CONNECTION_HANDLER_NOT_DEFINED);
            response.clear();
          }

          ctx.pushMessage(P2pMessage(P2pMessage::REPLY, cmd.command, std::move(response), retcode));
        }

        if (is_ip_address_blocked(ctx.m_remote_ip)) {
          ctx.m_state = CryptoNoteConnectionContext::state_shutdown;
          logger(Debugging) << ctx << " was blocked while handling the connection, dropping connection.";
        }
      }
    } catch (System::InterruptedException&) {
      logger(Debugging) << ctx << "connectionHandler() inner context is interrupted";
    } catch (std::exception& e) {
      logger(Debugging) << ctx << "Exception in connectionHandler: " << e.what();
    }

    safeInterrupt(ctx);
    safeInterrupt(writeContext);
    writeContext.wait();

    on_connection_close(ctx);
    m_connections.erase(connectionId);
  });

  ctx.context = &context;

  try {
    context.get();
  } catch (System::InterruptedException&) {
    logger(Debugging) << "connectionHandler() is interrupted";
  } catch (std::exception& e) {
    logger(Warning) << "connectionHandler() throws exception: " << e.what();
  } catch (...) {
    logger(Warning) << "connectionHandler() throws unknown exception";
  }
}

void NodeServer::writeHandler(P2pConnectionContext& ctx) {
  logger(Debugging) << ctx << "writeHandler started";

  try {
    LevinProtocol proto(ctx.connection);

    for (;;) {
      auto msgs = ctx.popBuffer();
      if (msgs.empty()) {
        break;
      }

      for (const auto& msg : msgs) {
        logger(Debugging) << ctx << "msg " << msg.type << ':' << msg.command;
        switch (msg.type) {
          case P2pMessage::COMMAND:
            proto.sendMessage(msg.command, msg.buffer, true);
            break;
          case P2pMessage::NOTIFY:
            proto.sendMessage(msg.command, msg.buffer, false);
            break;
          case P2pMessage::REPLY:
            proto.sendReply(msg.command, msg.buffer, msg.returnCode);
            break;
          default:
            assert(false);
        }
      }
    }
  } catch (System::InterruptedException&) {
    // connection stopped
    logger(Debugging) << ctx << "writeHandler() is interrupted";
  } catch (std::exception& e) {
    logger(Debugging) << ctx << "error during write: " << e.what();
    safeInterrupt(ctx);  // stop connection on write error
  }

  logger(Debugging) << ctx << "writeHandler finished";
}

template <typename T>
void NodeServer::safeInterrupt(T& obj) {
  try {
    obj.interrupt();
  } catch (std::exception& e) {
    logger(Warning) << "interrupt() throws exception: " << e.what();
  } catch (...) {
    logger(Warning) << "interrupt() throws unknown exception";
  }
}

NodeServer::State NodeServer::currentState() const {
  return m_currentState.load(std::memory_order_consume);
}

}  // namespace CryptoNote
