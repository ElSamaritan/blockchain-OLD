// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <list>
#include <atomic>

#include <boost/functional/hash.hpp>

#include <System/Context.h>
#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/Event.h>
#include <System/Timer.h>
#include <System/TcpConnection.h>
#include <System/TcpListener.h>

#include <Xi/Concurrent/RecursiveLock.h>
#include <Xi/Config/NetworkType.h>

#include "CryptoNoteCore/OnceInInterval.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
#include "Common/CommandLine.h"
#include "Logging/LoggerRef.h"

#include "ConnectionContext.h"
#include "LevinProtocol.h"
#include "NetNodeCommon.h"
#include "NetNodeConfig.h"
#include "P2pProtocolDefinitions.h"
#include "PeerListManager.h"
#include "P2p/P2pPenalty.h"

namespace System {
class TcpConnection;
}

namespace CryptoNote {
class LevinProtocol;
class ISerializer;

struct P2pMessage {
  enum Type { COMMAND, REPLY, NOTIFY };

  P2pMessage(Type type, uint32_t command, const BinaryArray& buffer, int32_t returnCode = 0)
      : type(type), command(command), buffer(buffer), returnCode(returnCode) {}

  P2pMessage(P2pMessage&& msg)
      : type(msg.type), command(msg.command), buffer(std::move(msg.buffer)), returnCode(msg.returnCode) {}

  size_t size() { return buffer.size(); }

  Type type;
  uint32_t command;
  const BinaryArray buffer;
  int32_t returnCode;
};

struct P2pConnectionContext : public CryptoNoteConnectionContext {
 public:
  using Clock = std::chrono::steady_clock;
  using TimePoint = Clock::time_point;

  System::Context<void>* context;
  PeerIdType peerId;
  System::TcpConnection connection;

  P2pConnectionContext(System::Dispatcher& dispatcher, Logging::ILogger& log, System::TcpConnection&& conn)
      : context(nullptr),
        peerId(0),
        connection(std::move(conn)),
        logger(log, "node_server"),
        queueEvent(dispatcher),
        stopped(false) {}

  P2pConnectionContext(P2pConnectionContext&& ctx)
      : CryptoNoteConnectionContext(std::move(ctx)),
        context(ctx.context),
        peerId(ctx.peerId),
        connection(std::move(ctx.connection)),
        logger(ctx.logger.getLogger(), "node_server"),
        queueEvent(std::move(ctx.queueEvent)),
        stopped(std::move(ctx.stopped)) {}

  bool pushMessage(P2pMessage&& msg);
  std::vector<P2pMessage> popBuffer();
  void interrupt();

  uint64_t writeDuration(TimePoint now) const;

 private:
  Logging::LoggerRef logger;
  TimePoint writeOperationStartTime;
  System::Event queueEvent;
  std::vector<P2pMessage> writeQueue;
  size_t writeQueueSize = 0;
  bool stopped;
};

class NodeServer : public IP2pEndpoint {
 public:
  static void init_options(boost::program_options::options_description& desc);

  NodeServer(System::Dispatcher& dispatcher, ::Xi::Config::Network::Type network,
             CryptoNote::CryptoNoteProtocolHandler& payload_handler, Logging::ILogger& log);

  [[nodiscard]] bool run();
  [[nodiscard]] bool init(const NetNodeConfig& config);
  [[nodiscard]] bool deinit();
  bool sendStopSignal();
  uint32_t get_this_peer_port() { return m_listeningPort; }
  CryptoNote::CryptoNoteProtocolHandler& get_payload_object();

  [[nodiscard]] bool serialize(ISerializer& s);

  // debug functions
  bool log_peerlist();
  bool log_connections();
  virtual uint64_t get_connections_count() override;
  size_t get_outgoing_connections_count();

  CryptoNote::PeerlistManager& getPeerlistManager() { return m_peerlist; }

 private:
  int handleCommand(const LevinProtocol::Command& cmd, BinaryArray& buff_out, P2pConnectionContext& context,
                    bool& handled);

  //----------------- commands handlers ----------------------------------------------
  int handle_handshake(int command, COMMAND_HANDSHAKE::request& arg, COMMAND_HANDSHAKE::response& rsp,
                       P2pConnectionContext& context);
  int handle_timed_sync(int command, COMMAND_TIMED_SYNC::request& arg, COMMAND_TIMED_SYNC::response& rsp,
                        P2pConnectionContext& context);
  int handle_ping(int command, COMMAND_PING::request& arg, COMMAND_PING::response& rsp, P2pConnectionContext& context);
#ifdef ALLOW_DEBUG_COMMANDS
  int handle_get_stat_info(int command, COMMAND_REQUEST_STAT_INFO::request& arg,
                           COMMAND_REQUEST_STAT_INFO::response& rsp, P2pConnectionContext& context);
  int handle_get_network_state(int command, COMMAND_REQUEST_NETWORK_STATE::request& arg,
                               COMMAND_REQUEST_NETWORK_STATE::response& rsp, P2pConnectionContext& context);
  int handle_get_peer_id(int command, COMMAND_REQUEST_PEER_ID::request& arg, COMMAND_REQUEST_PEER_ID::response& rsp,
                         P2pConnectionContext& context);
  bool check_trust(const proof_of_trust& tr);
#endif

  [[nodiscard]] bool init_config();
  [[nodiscard]] bool make_default_config();
  [[nodiscard]] bool store_config();
  void initUpnp();

  bool handshake(CryptoNote::LevinProtocol& proto, P2pConnectionContext& context, bool just_take_peerlist = false);
  bool timedSync();
  bool handleTimedSyncResponse(const BinaryArray& in, P2pConnectionContext& context);
  void forEachConnection(std::function<void(P2pConnectionContext&)> action);

  void on_connection_new(P2pConnectionContext& context);
  void on_connection_close(P2pConnectionContext& context);

  //----------------- i_p2p_endpoint -------------------------------------------------------------
  virtual void relay_notify_to_all(int command, const BinaryArray& data_buff,
                                   const net_connection_id* excludeConnection) override;
  virtual bool invoke_notify_to_peer(int command, const BinaryArray& req_buff,
                                     const CryptoNoteConnectionContext& context) override;
  virtual void for_each_connection(
      std::function<void(CryptoNote::CryptoNoteConnectionContext&, PeerIdType)> f) override;
  virtual void externalRelayNotifyToAll(int command, const BinaryArray& data_buff,
                                        const net_connection_id* excludeConnection) override;

  virtual bool report_failure(const uint32_t ip, P2pPenalty penality) override;
  virtual void report_success(const uint32_t ip) override;
  virtual bool is_ip_address_blocked(const uint32_t ip) override;

  //-----------------------------------------------------------------------------------------------
  bool handle_command_line(const boost::program_options::variables_map& vm);
  bool handleConfig(const NetNodeConfig& config);
  bool append_net_address(std::vector<NetworkAddress>& nodes, const std::string& addr);
  bool idle_worker();
  bool handle_remote_peerlist(const std::list<PeerlistEntry>& peerlist, time_t local_time,
                              const CryptoNoteConnectionContext& context);
  bool get_local_node_data(basic_node_data& node_data);

  bool merge_peerlist_with_local(const std::list<PeerlistEntry>& bs);
  bool fix_time_delta(std::list<PeerlistEntry>& local_peerlist, time_t local_time, int64_t& delta);

  bool connections_maker();
  bool make_new_connection_from_peerlist(bool use_white_list);
  bool try_to_connect_and_handshake_with_new_peer(const NetworkAddress& na, bool just_take_peerlist = false,
                                                  uint64_t last_seen_stamp = 0, bool white = true);
  bool is_peer_used(const PeerlistEntry& peer);
  bool is_addr_connected(const NetworkAddress& peer);
  bool try_ping(basic_node_data& node_data, P2pConnectionContext& context);
  bool make_expected_connections_count(bool white_list, size_t expected_connections);
  bool is_priority_node(const NetworkAddress& na);

  bool connect_to_peerlist(const std::vector<NetworkAddress>& peers);

  bool parse_peers_and_add_to_container(const boost::program_options::variables_map& vm,
                                        const command_line::arg_descriptor<std::vector<std::string>>& arg,
                                        std::vector<NetworkAddress>& container);

  // debug functions
  std::string print_connections_container();

  typedef std::unordered_map<boost::uuids::uuid, P2pConnectionContext, boost::hash<boost::uuids::uuid>>
      ConnectionContainer;
  typedef ConnectionContainer::iterator ConnectionIterator;
  ConnectionContainer m_connections;

  void acceptLoop();
  void connectionHandler(const boost::uuids::uuid& connectionId, P2pConnectionContext& connection);
  void writeHandler(P2pConnectionContext& ctx);
  void onIdle();
  void timedSyncLoop();
  void timeoutLoop();

  template <typename T>
  void safeInterrupt(T& obj);

  struct config {
    network_config m_net_config;
    uint64_t m_peer_id;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(m_net_config)
    KV_MEMBER(m_peer_id)
    KV_END_SERIALIZATION
  };

  config m_config;
  std::string m_config_folder;

  bool m_have_address;
  bool m_first_connection_maker_call;
  uint16_t m_listeningPort;
  uint16_t m_external_port;
  uint32_t m_ip_address;
  bool m_allow_local_ip;
  bool m_hide_my_port;
  std::string m_p2p_state_filename;

  System::Dispatcher& m_dispatcher;
  System::ContextGroup m_workingContextGroup;
  System::Event m_stopEvent;
  System::Timer m_idleTimer;
  System::Timer m_timeoutTimer;
  System::TcpListener m_listener;
  Logging::LoggerRef logger;
  std::atomic<bool> m_stop;

  CryptoNoteProtocolHandler& m_payload_handler;
  PeerlistManager m_peerlist;

  // OnceInInterval m_peer_handshake_idle_maker_interval;
  OnceInInterval m_connections_maker_interval;
  OnceInInterval m_peerlist_store_interval;
  System::Timer m_timedSyncTimer;

  std::string m_bind_ip;
  std::string m_port;
#ifdef ALLOW_DEBUG_COMMANDS
  uint64_t m_last_stat_request_time;
#endif
  std::vector<NetworkAddress> m_priority_peers;
  std::vector<NetworkAddress> m_exclusive_peers;
  std::vector<NetworkAddress> m_seed_nodes;
  std::list<PeerlistEntry> m_command_line_peers;
  uint64_t m_peer_livetime;
  boost::uuids::uuid m_network_id;

  /* ----------------------------------------------- Node Blocking ------------------------------------------------- */
 private:
  /*!
   * \brief block_host Blocks an ip address for a given timespan
   * \param address_ip The address to block
   * \param seconds The timespan to reject any connection from now
   * \return True if succeeded, otherwise false.
   *
   * The ip gets added to the blocked hosts list and can only connect after the timespan, from now, has passed. Further
   * all connection to peers from the same ip get closed.
   */
  bool block_host(const uint32_t address_ip, std::chrono::seconds seconds);

  /*!
   * \brief unblock_host Removes an ip from the blocked list and resets the penalty score.
   * \param address_ip The address to unblock.
   * \return True if the host was successfully unblocked.
   */
  bool unblock_host(const uint32_t address_ip);

  /*!
   * \brief add_host_fail Adds a host failure and updates its penalty score, conditionally bans it.
   * \param address_ip The address causing the failure.
   * \param penalty The type of failure caused.
   * \return True if the penalty was applied successfully.
   */
  bool add_host_fail(const uint32_t address_ip, P2pPenalty penalty);

  /*!
   * \brief add_host_success Reduces the penalty score for an ip address by one.
   * \param address_ip The peers ip addres.
   */
  void add_host_success(const uint32_t address_ip);

  /*!
   * \brief evaluate_blocked_connection Reevalutes the block status of a given ip address.
   * \param address_ip The connecting peers ip address
   * \return True if the connection should be blocked otherwise false
   */
  bool evaluate_blocked_connection(const uint32_t address_ip);

 public:
  /*!
   * \brief blockedPeers Returns a summary of all currently blocked peers.
   *
   * \note blocked peers are only reevaluated if they connect again. This list may contain peers which could connect
   * because their block timespan already passed.
   */
  std::map<uint32_t, int64_t> blockedPeers() const;

  /*!
   * \brief peerPenalties Returns a summary of the penalties score for each peer that has at least a score of one.
   */
  std::map<uint32_t, uint64_t> peerPenalties() const;

  /*!
   * \brief banIps Adds ip addresses to the block list or renew them if already present.
   * \param ips Ip addresses to block.
   * \return The count of newly blocked ip addresses, others are renewed.
   */
  size_t banIps(const std::vector<uint32_t>& ips);

  /*!
   * \brief unbanIps Removes all ip addresses from the block list and resets their penalty score.
   * \param ips Ip addresses to unban.
   * \return The count of successfully removed ip addresses.
   */
  size_t unbanIps(const std::vector<uint32_t>& ips);

  /*!
   * \brief unbanAllIps Removes all blocked peers and resets their penalty score.
   * \return The count of successfully removed peers.
   */
  size_t unbanAllIps();

  /*!
   * \brief isBlocked Checks if an ip is banned.
   * \param ip The ip to check.
   * \return True if the ip is blocked, otherwise false.
   */
  bool isBlocked(uint32_t ip) const;

  /*!
   * \brief resetPenalties Deletes all penalties assigned to peers.
   * \return The count of removed peer penalties.
   *
   * \attention This will not unban any peer.
   */
  size_t resetPenalties();

  /*!
   * \brief isAutoBlockEnabled Enables blocking ips autonomiously if they reach a certain penalty score.
   * \return True if auto blocking should be applied, otherwise false.
   */
  bool isAutoBlockEnabled() const;

  /*!
   * \brief setAutoBlockEnabled Enables/Disables autonomiously blocking using the penalty score.
   * \param isEnabled True if auto blocking should be applied, otherwise false.
   */
  void setAutoBlockEnabled(const bool isEnabled);

 private:
  mutable Xi::Concurrent::RecursiveLock m_block_access;
  std::map<uint32_t, int64_t> m_blocked_hosts;
  std::map<uint32_t, uint64_t> m_host_fails_score;
  uint64_t m_fails_before_block;
  std::chrono::seconds m_block_time;
  std::atomic<bool> m_auto_block;
};
}  // namespace CryptoNote
