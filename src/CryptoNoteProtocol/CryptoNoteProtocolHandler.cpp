// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "CryptoNoteProtocolHandler.h"

#include <future>
#include <random>

#include <boost/scope_exit.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <Xi/Global.hh>
#include <Xi/Config/Ascii.h>
#include <Xi/Config.h>
#include <Xi/Config/WalletConfig.h>
#include <System/Dispatcher.h>
#include <Common/FormatTools.h>
#include <Serialization/SerializationOverloads.h>

#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/Transactions/ITransactionPool.h"
#include "P2p/LevinProtocol.h"

using namespace Logging;
using namespace Common;

namespace CryptoNote {

namespace {

template <class t_parametr>
bool post_notify(IP2pEndpoint& p2p, typename t_parametr::request& arg, CryptoNoteConnectionContext& context) {
  if (p2p.invoke_notify_to_peer(t_parametr::ID, LevinProtocol::encode(arg), context)) {
    return true;
  } else {
    if (p2p.report_failure(context.m_remote_ip, P2pPenalty::NoResponse)) {
      context.m_state = CryptoNoteConnectionContext::state_shutdown;
    }
    return false;
  }
}

template <class t_parametr>
void relay_post_notify(IP2pEndpoint& p2p, typename t_parametr::request& arg,
                       const net_connection_id* excludeConnection = nullptr) {
  p2p.externalRelayNotifyToAll(t_parametr::ID, LevinProtocol::encode(arg), excludeConnection);
}

}  // namespace

[[nodiscard]] static inline bool serialize(NOTIFY_NEW_TRANSACTIONS_request& request, ISerializer& s) {
  std::vector<std::string> transactions;
  if (s.type() == ISerializer::INPUT) {
    XI_RETURN_EC_IF_NOT(s(transactions, "txs"), false);
    request.transactions.reserve(transactions.size());
    std::transform(transactions.begin(), transactions.end(), std::back_inserter(request.transactions),
                   [](const std::string& s) { return BinaryArray(s.begin(), s.end()); });
    return true;
  } else {
    transactions.reserve(request.transactions.size());
    std::transform(request.transactions.begin(), request.transactions.end(), std::back_inserter(transactions),
                   [](const BinaryArray& s) { return std::string(s.begin(), s.end()); });
    XI_RETURN_EC_IF_NOT(s(transactions, "txs"), false);
    return true;
  }
}

[[nodiscard]] static inline bool serialize(NOTIFY_RESPONSE_GET_OBJECTS_request& request, ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(request.transactions, "txs"), false);
  XI_RETURN_EC_IF_NOT(s(request.blocks, "blocks"), false);
  XI_RETURN_EC_IF_NOT(s(request.missed_ids, "missed_ids"), false);
  XI_RETURN_EC_IF_NOT(s(request.current_blockchain_height, "current_blockchain_height"), false);
  return true;
}

[[nodiscard]] static inline bool serialize(NOTIFY_NEW_LITE_BLOCK_request& request, ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(request.current_blockchain_height, "h"), false);
  XI_RETURN_EC_IF_NOT(s(request.hop, "hop"), false);

  std::string blockTemplate;
  if (s.type() == ISerializer::INPUT) {
    XI_RETURN_EC_IF_NOT(s(blockTemplate, "blockTemplate"), false);
    request.block.blockTemplate.reserve(blockTemplate.size());
    std::copy(blockTemplate.begin(), blockTemplate.end(), std::back_inserter(request.block.blockTemplate));
  } else {
    blockTemplate.reserve(request.block.blockTemplate.size());
    std::copy(request.block.blockTemplate.begin(), request.block.blockTemplate.end(),
              std::back_inserter(blockTemplate));
    XI_RETURN_EC_IF_NOT(s(blockTemplate, "blockTemplate"), false);
  }
  return true;
}

[[nodiscard]] static inline bool serialize(NOTIFY_MISSING_TXS_REQUEST_ENTRY::request& request, ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(request.current_blockchain_height, "height"), false);
  XI_RETURN_EC_IF_NOT(s(request.missing_txs, "txs"), false);
  return true;
}

[[nodiscard]] static inline bool serialize(NOTIFY_MISSING_TXS_RESPONSE_ENTRY::request& request, ISerializer& s) {
  std::vector<std::string> transactions;
  if (s.type() == ISerializer::INPUT) {
    XI_RETURN_EC_IF_NOT(s(transactions, "txs"), false);
    request.transactions.reserve(transactions.size());
    std::transform(transactions.begin(), transactions.end(), std::back_inserter(request.transactions),
                   [](const std::string& s) { return BinaryArray(s.begin(), s.end()); });
  } else {
    transactions.reserve(request.transactions.size());
    std::transform(request.transactions.begin(), request.transactions.end(), std::back_inserter(transactions),
                   [](const BinaryArray& s) { return std::string(s.begin(), s.end()); });
    XI_RETURN_EC_IF_NOT(s(transactions, "txs"), false);
  }
  return true;
}

CryptoNoteProtocolHandler::CryptoNoteProtocolHandler(const Currency& currency, System::Dispatcher& dispatcher,
                                                     ICore& rcore, IP2pEndpoint* p_net_layout, Logging::ILogger& log)
    : m_dispatcher(dispatcher),
      m_core(rcore),
      m_currency(currency),
      m_p2p(p_net_layout),
      m_synchronized(false),
      m_stop(false),
      m_observedHeight(BlockHeight::fromIndex(0)),
      m_blockchainHeight(BlockHeight::fromIndex(0)),
      m_peersCount(0),
      m_suspiciousGuard{log},
      m_logger(log, "protocol") {
  if (!m_p2p) {
    m_p2p = &m_p2p_stub;
  }
}

size_t CryptoNoteProtocolHandler::getPeerCount() const {
  return m_peersCount;
}

void CryptoNoteProtocolHandler::set_p2p_endpoint(IP2pEndpoint* p2p) {
  if (p2p)
    m_p2p = p2p;
  else
    m_p2p = &m_p2p_stub;
}

bool CryptoNoteProtocolHandler::isLightNode() const {
  return m_core.isPruned();
}

void CryptoNoteProtocolHandler::onConnectionOpened(CryptoNoteConnectionContext& context) {
  XI_UNUSED(context);
}

void CryptoNoteProtocolHandler::onConnectionClosed(CryptoNoteConnectionContext& context) {
  bool updated = false;
  {
    std::lock_guard<std::mutex> lock(m_observedHeightMutex);
    auto prevHeight = m_observedHeight;
    recalculateMaxObservedHeight(context);
    if (prevHeight != m_observedHeight) {
      updated = true;
    }
  }

  if (updated) {
    m_logger(Trace) << "Observed height updated: " << m_observedHeight.native();
    m_observerManager.notify(&ICryptoNoteProtocolObserver::lastKnownBlockHeightUpdated, m_observedHeight);
  }

  if (context.m_state != CryptoNoteConnectionContext::state_befor_handshake) {
    m_peersCount--;
    m_observerManager.notify(&ICryptoNoteProtocolObserver::peerCountUpdated, m_peersCount.load());
  }
}

void CryptoNoteProtocolHandler::stop() {
  m_stop = true;
}

bool CryptoNoteProtocolHandler::start_sync(CryptoNoteConnectionContext& context) {
  m_logger(Logging::Trace) << context << "Starting synchronization";

  if (context.m_state == CryptoNoteConnectionContext::state_synchronizing) {
    assert(context.m_needed_objects.empty());
    assert(context.m_requested_objects.empty());

    NOTIFY_REQUEST_CHAIN::request r = boost::value_initialized<NOTIFY_REQUEST_CHAIN::request>();
    r.block_hashes = m_core.buildSparseChain();
    m_logger(Logging::Trace) << context << "-->>NOTIFY_REQUEST_CHAIN: m_block_ids.size()=" << r.block_hashes.size();
    post_notify<NOTIFY_REQUEST_CHAIN>(*m_p2p, r, context);
  }

  return true;
}

CoreStatistics CryptoNoteProtocolHandler::getStatistics() {
  return m_core.getCoreStatistics();
}

void CryptoNoteProtocolHandler::log_connections() {
  std::stringstream ss;

  ss << std::setw(25) << std::left << "Remote Host" << std::setw(20) << "Peer ID" << std::setw(25)
     << "Recv/Sent (inactive,sec)" << std::setw(25) << "State" << std::setw(20) << "Lifetime(seconds)" << ENDL;

  m_p2p->for_each_connection([&](const CryptoNoteConnectionContext& cntxt, PeerIdType peer_id) {
    ss << std::setw(25) << std::left
       << std::string(cntxt.m_is_income ? "[INCOMING]" : "[OUTGOING]") + Common::ipAddressToString(cntxt.m_remote_ip) +
              ":" + std::to_string(cntxt.m_remote_port)
       << std::setw(20) << std::hex
       << peer_id
       // << std::setw(25) << std::to_string(cntxt.m_recv_cnt) + "(" + std::to_string(time(NULL) - cntxt.m_last_recv) +
       // ")" + "/" + std::to_string(cntxt.m_send_cnt) + "(" + std::to_string(time(NULL) - cntxt.m_last_send) + ")"
       << std::setw(25) << get_protocol_state_string(cntxt.m_state) << std::setw(20)
       << std::to_string(time(NULL) - cntxt.m_started) << ENDL;
  });
  m_logger(Info) << "Connections: " << ENDL << ss.str();
}

std::string CryptoNoteProtocolHandler::updateDownloadUrl() const {
  return m_currency.general().downloadUrl();
}

BlockHeight CryptoNoteProtocolHandler::get_current_blockchain_height() {
  return BlockHeight::fromIndex(m_core.getTopBlockIndex());
}

bool CryptoNoteProtocolHandler::process_payload_sync_data(const CORE_SYNC_DATA& hshd,
                                                          CryptoNoteConnectionContext& context, bool is_initial) {
  if (context.m_state == CryptoNoteConnectionContext::state_befor_handshake && !is_initial)
    return true;
  context.m_is_light_node = hshd.is_light_node;

  if (context.m_state == CryptoNoteConnectionContext::state_synchronizing) {
  } else if (m_core.hasBlock(hshd.top_id)) {
    if (is_initial) {
      on_connection_synchronized();
      context.m_state = CryptoNoteConnectionContext::state_pool_sync_required;
    } else {
      context.m_state = CryptoNoteConnectionContext::state_normal;
    }
  } else if (context.m_is_light_node) {
    m_logger(Logging::Debugging) << context << " LightNode connected, will not sync.";
    context.m_state = CryptoNoteConnectionContext::state_normal;
  } else {
    const auto currentHeight = get_current_blockchain_height();
    const auto remoteHeight = hshd.current_height;

    /* Find the difference between the remote and the local height */
    auto diff = remoteHeight - currentHeight;

    /* Find out how many days behind/ahead we are from the remote height */
    uint64_t days = static_cast<uint64_t>(std::abs(diff.native())) / (24 * 60 * 60 / m_currency.coin().blockTime());

    std::stringstream ss;

    ss << "Your " << m_core.currency().coin().name() << " node is syncing with the network ";

    /* We're behind the remote node */
    if (diff >= BlockOffset::fromNative(0)) {
      ss << "(" << Common::get_sync_percentage(currentHeight, remoteHeight) << "% complete) ";

      ss << "You are " << diff.native() << " blocks (" << days << " days) behind ";
    }
    /* We're ahead of the remote node, no need to print percentages */
    else {
      ss << "You are " << std::abs(diff.native()) << " blocks (" << days << " days) ahead ";
    }

    ss << "the current peer you're connected to.";

    auto logLevel = Logging::Trace;
    /* Log at different levels depending upon if we're ahead, behind, and if it's
      a newly formed connection */
    if (diff >= BlockOffset::fromNative(0)) {
      if (is_initial) {
        logLevel = Logging::Info;
      } else {
        logLevel = Logging::Debugging;
      }
    }
    m_logger(logLevel, Logging::CYAN) << context << ss.str();

    m_logger(Logging::Debugging) << "Remote top block height: " << hshd.current_height.native()
                                 << ", id: " << hshd.top_id;
    // let the socket to send response to handshake, but request callback, to let send request data after response
    m_logger(Logging::Trace) << context << "requesting synchronization";
    context.m_state = CryptoNoteConnectionContext::state_sync_required;
  }

  updateObservedHeight(hshd.current_height, context);
  context.m_remote_blockchain_height = hshd.current_height;

  if (is_initial) {
    m_peersCount++;
    m_observerManager.notify(&ICryptoNoteProtocolObserver::peerCountUpdated, m_peersCount.load());
  }

  return true;
}

bool CryptoNoteProtocolHandler::get_payload_sync_data(CORE_SYNC_DATA& hshd) {
  hshd.top_id = m_core.getTopBlockHash();
  hshd.current_height = BlockHeight::fromIndex(m_core.getTopBlockIndex());
  hshd.is_light_node = m_core.isPruned();
  return true;
}

template <typename Command, typename Handler>
int notifyAdaptor(const BinaryArray& reqBuf, CryptoNoteConnectionContext& ctx, Handler handler) {
  typedef typename Command::request Request;
  int command = Command::ID;

  Request req = boost::value_initialized<Request>();
  auto decodeResult = LevinProtocol::decode(reqBuf, req);
  if (decodeResult.isError()) {
    throw std::runtime_error("Failed to load_from_binary in command " + std::to_string(command) +
                             " with error= " + decodeResult.error().message());
  }

  return handler(command, req, ctx);
}

#define HANDLE_NOTIFY(CMD, Handler)                                                                                  \
  case CMD::ID: {                                                                                                    \
    ret = notifyAdaptor<CMD>(                                                                                        \
        in, ctx, [this](int a1, CMD::request& a2, CryptoNoteConnectionContext& a3) { return Handler(a1, a2, a3); }); \
    break;                                                                                                           \
  }

int CryptoNoteProtocolHandler::handleCommand(bool is_notify, int command, const BinaryArray& in, BinaryArray& out,
                                             CryptoNoteConnectionContext& ctx, bool& handled) {
  XI_UNUSED(is_notify, out);
  int ret = 0;
  handled = true;

  switch (command) {
    HANDLE_NOTIFY(NOTIFY_NEW_TRANSACTIONS, handle_notify_new_transactions)
    HANDLE_NOTIFY(NOTIFY_REQUEST_GET_OBJECTS, handle_request_get_objects)
    HANDLE_NOTIFY(NOTIFY_RESPONSE_GET_OBJECTS, handle_response_get_objects)
    HANDLE_NOTIFY(NOTIFY_REQUEST_CHAIN, handle_request_chain)
    HANDLE_NOTIFY(NOTIFY_RESPONSE_CHAIN_ENTRY, handle_response_chain_entry)
    HANDLE_NOTIFY(NOTIFY_REQUEST_TX_POOL, handleRequestTxPool)
    HANDLE_NOTIFY(NOTIFY_NEW_LITE_BLOCK, handle_notify_new_lite_block)
    HANDLE_NOTIFY(NOTIFY_MISSING_TXS_REQUEST_ENTRY, handle_notify_missing_txs_request)
    HANDLE_NOTIFY(NOTIFY_MISSING_TXS_RESPONSE_ENTRY, handle_notify_missing_txs_response)

    default:
      handled = false;
  }

  return ret;
}

#undef HANDLE_NOTIFY

#define P2P_DROP_AND_LOG_RETURN(STR)                             \
  m_logger(Logging::Debugging) << context << STR;                \
  context.m_state = CryptoNoteConnectionContext::state_shutdown; \
  return false;

int CryptoNoteProtocolHandler::handle_notify_new_transactions(int command, NOTIFY_NEW_TRANSACTIONS::request& arg,
                                                              CryptoNoteConnectionContext& context) {
  XI_UNUSED(command);
  m_logger(Logging::Trace) << context << "NOTIFY_NEW_TRANSACTIONS";

  if (context.m_state != CryptoNoteConnectionContext::state_normal)
    return 1;

  if (m_suspiciousGuard.pushAndInspect(context, arg)) {
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::SuspiciousRequestSequence);
    P2P_DROP_AND_LOG_RETURN("suspicious sequence of notify transactions requests");
  }

  for (auto tx_blob_it = arg.transactions.begin(); tx_blob_it != arg.transactions.end();) {
    auto txParseResult = CachedTransaction::fromBinaryArray(*tx_blob_it);
    if (txParseResult.isError()) {
      m_logger(Logging::Debugging) << context << "Invalid transaction blob, dropping connection.";
      m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
      context.m_state = CryptoNoteConnectionContext::state_shutdown;
      return 1;
    }
    const auto txHash = txParseResult.value().getTransactionHash();
    if (m_core.hasTransaction(txHash)) {
      tx_blob_it = arg.transactions.erase(tx_blob_it);
    } else {
      auto pushTxResult = m_core.transactionPool().pushTransaction(*tx_blob_it);
      if (pushTxResult.isError()) {
        m_logger(Logging::Debugging) << context << "Tx verification failed: " << pushTxResult.error().message();
        tx_blob_it = arg.transactions.erase(tx_blob_it);
      } else {
        ++tx_blob_it;
      }
    }
  }

  if (arg.transactions.size()) {
    // TODO: add announce usage here
    m_p2p->report_success(context.m_remote_ip);
    relay_post_notify<NOTIFY_NEW_TRANSACTIONS>(*m_p2p, arg, &context.m_connection_id);
  }

  return true;
}

int CryptoNoteProtocolHandler::handle_request_get_objects(int command, NOTIFY_REQUEST_GET_OBJECTS::request& arg,
                                                          CryptoNoteConnectionContext& context) {
  XI_UNUSED(command);
  m_logger(Logging::Trace) << context << "NOTIFY_REQUEST_GET_OBJECTS";

  NOTIFY_RESPONSE_GET_OBJECTS::request rsp;
  if (isLightNode()) {
    m_logger(Logging::Debugging) << context << "NOTIFY_RESPONSE_GET_OBJECTS: This node is light node.";
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  if (!arg.txs.empty()) {
    m_logger(Logging::Debugging) << context
                                 << "NOTIFY_RESPONSE_GET_OBJECTS: request.txs.empty() != true, dropping connection";
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  if (arg.blocks.empty()) {
    m_logger(Logging::Debugging) << context
                                 << "NOTIFY_RESPONSE_GET_OBJECTS: request.block.empty() == true, dropping connection";
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  if (arg.blocks.size() > Xi::Config::Network::blocksP2pSynchronizationMaxBatchSize()) {
    m_logger(Logging::Debugging) << context << "NOTIFY_RESPONSE_GET_OBJECTS: request.block.size() > "
                                 << Xi::Config::Network::blocksP2pSynchronizationMaxBatchSize()
                                 << " dropping connection";
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  rsp.current_blockchain_height = BlockHeight::fromIndex(m_core.getTopBlockIndex());
  std::vector<RawBlock> rawBlocks;
  m_core.getBlocks(arg.blocks, rawBlocks, rsp.missed_ids, Xi::Config::Network::blocksP2pSynchronizationMaxBlobSize());

  rsp.blocks = std::move(rawBlocks);

  m_logger(Logging::Trace) << context << "-->>NOTIFY_RESPONSE_GET_OBJECTS: blocks.size()=" << rsp.blocks.size()
                           << ", txs.size()=" << rsp.transactions.size()
                           << ", rsp.m_current_blockchain_height=" << rsp.current_blockchain_height.native()
                           << ", missed_ids.size()=" << rsp.missed_ids.size();

  if (m_suspiciousGuard.pushAndInspect(context, arg, rsp)) {
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::SuspiciousRequestSequence);
    P2P_DROP_AND_LOG_RETURN("suspicious sequence of objects requests");
  }

  if (rsp.missed_ids.size() < rsp.blocks.size() / 4) {
    m_p2p->report_success(context.m_remote_ip);
  }

  post_notify<NOTIFY_RESPONSE_GET_OBJECTS>(*m_p2p, rsp, context);
  return 1;
}

int CryptoNoteProtocolHandler::handle_response_get_objects(int command, NOTIFY_RESPONSE_GET_OBJECTS::request& arg,
                                                           CryptoNoteConnectionContext& context) {
  XI_UNUSED(command);
  m_logger(Logging::Trace) << context << "NOTIFY_RESPONSE_GET_OBJECTS";

  if (context.m_last_response_height > arg.current_blockchain_height) {
    m_logger(Logging::Error) << context << "sent wrong NOTIFY_HAVE_OBJECTS: arg.m_current_blockchain_height="
                             << arg.current_blockchain_height.native()
                             << " < m_last_response_height=" << context.m_last_response_height.native()
                             << ", dropping connection";
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidResponse);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  context.m_remote_blockchain_height = arg.current_blockchain_height;
  updateObservedHeight(arg.current_blockchain_height, context);
  std::vector<BlockTemplate> blockTemplates;
  std::vector<CachedBlock> cachedBlocks;
  blockTemplates.resize(arg.blocks.size());
  cachedBlocks.reserve(arg.blocks.size());

  std::vector<RawBlock> rawBlocks = std::move(arg.blocks);

  for (size_t index = 0; index < rawBlocks.size(); ++index) {
    if (!fromBinaryArray(blockTemplates[index], rawBlocks[index].blockTemplate)) {
      m_logger(Logging::Error) << context << "sent wrong block: failed to parse and validate block: \r\n"
                               << toHex(rawBlocks[index].blockTemplate) << "\r\n dropping connection";
      m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidResponse);
      context.m_state = CryptoNoteConnectionContext::state_shutdown;
      return 1;
    }

    cachedBlocks.emplace_back(blockTemplates[index]);
    if (index == 1) {
      if (m_core.hasBlock(cachedBlocks.back().getBlockHash())) {  // TODO
        context.m_state = CryptoNoteConnectionContext::state_idle;
        context.m_needed_objects.clear();
        context.m_requested_objects.clear();
        m_logger(Logging::Debugging) << context << "Connection set to idle state.";
        return 1;
      }
    }

    auto req_it = context.m_requested_objects.find(cachedBlocks.back().getBlockHash());
    if (req_it == context.m_requested_objects.end()) {
      m_logger(Logging::Error) << context << "sent wrong NOTIFY_RESPONSE_GET_OBJECTS: block with id="
                               << Common::podToHex(cachedBlocks.back().getBlockHash())
                               << " wasn't requested, dropping connection";
      m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidResponse);
      context.m_state = CryptoNoteConnectionContext::state_shutdown;
      return 1;
    }

    if (cachedBlocks.back().getBlock().transactionHashes.size() != rawBlocks[index].transactions.size()) {
      m_logger(Logging::Error) << context << "sent wrong NOTIFY_RESPONSE_GET_OBJECTS: block with id="
                               << Common::podToHex(cachedBlocks.back().getBlockHash()) << ", transactionHashes.size()="
                               << cachedBlocks.back().getBlock().transactionHashes.size()
                               << " mismatch with block_complete_entry.m_txs.size()="
                               << rawBlocks[index].transactions.size() << ", dropping connection";
      m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidResponse);
      context.m_state = CryptoNoteConnectionContext::state_shutdown;
      return 1;
    }

    context.m_requested_objects.erase(req_it);
  }

  if (context.m_requested_objects.size()) {
    m_logger(Logging::Error) << context << "returned not all requested objects (context.m_requested_objects.size()="
                             << context.m_requested_objects.size() << "), dropping connection";
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidResponse);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  {
    int result = processObjects(context, std::move(rawBlocks), std::move(cachedBlocks));
    if (result != 0) {
      return result;
    }
  }

  m_logger(Debugging) << "Local blockchain updated, new index = " << m_core.getTopBlockIndex();
  m_p2p->report_success(context.m_remote_ip);
  if (!m_stop && context.m_state == CryptoNoteConnectionContext::state_synchronizing) {
    request_missing_objects(context, true);
  }

  return 1;
}

int CryptoNoteProtocolHandler::processObjects(CryptoNoteConnectionContext& context, std::vector<RawBlock>&& rawBlocks,
                                              const std::vector<CachedBlock>& cachedBlocks) {
  assert(rawBlocks.size() == cachedBlocks.size());
  for (size_t index = 0; index < rawBlocks.size(); ++index) {
    if (m_stop) {
      break;
    }

    auto addResult = m_core.addBlock(cachedBlocks[index], std::move(rawBlocks[index]));
    if (addResult == error::AddBlockErrorCondition::BLOCK_VALIDATION_FAILED ||
        addResult == error::AddBlockErrorCondition::TRANSACTION_VALIDATION_FAILED ||
        addResult == error::AddBlockErrorCondition::DESERIALIZATION_FAILED) {
      m_logger(Logging::Debugging) << context
                                   << "Block verification failed, dropping connection: " << addResult.message();
      reportFailureIfSynced(context, P2pPenalty::BlockValidationFailure);
      context.m_state = CryptoNoteConnectionContext::state_shutdown;
      return 1;
    } else if (addResult == error::AddBlockErrorCondition::BLOCK_REJECTED) {
      m_logger(Logging::Debugging) << context
                                   << "Block received at sync phase was marked as orphaned: " << addResult.message();
      reportFailureIfSynced(context, P2pPenalty::BlockValidationFailure);
      return 1;
    } else if (addResult == error::AddBlockErrorCode::ALREADY_EXISTS) {
      m_logger(Logging::Debugging) << context
                                   << "Block already exists, switching to idle state: " << addResult.message();
      context.m_state = CryptoNoteConnectionContext::state_idle;
      context.m_needed_objects.clear();
      context.m_requested_objects.clear();
      return 1;
    }

    m_dispatcher.yield();
  }

  return 0;
}

int CryptoNoteProtocolHandler::doPushLiteBlock(CryptoNoteConnectionContext& context, uint32_t hops, BlockHeight height,
                                               LiteBlock block, std::vector<CachedTransaction> txs) {
  context.m_pending_lite_block = boost::none;
  auto result = m_core.addBlock(LiteBlock{block}, std::move(txs));
  if (result.isError()) {
    // An error can actually be a non error due to legacy interface, we need to check now whether it was actually added
    // or not.
    auto error = result.error();
    if (error.isException()) {
      m_logger(Logging::Error) << context << "Exception occured handling lite block: " << error.message();
      m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
      context.m_state = CryptoNoteConnectionContext::state_shutdown;
      return 1;
    } else if (error.isErrorCode()) {
      auto ec = error.errorCode();
      if (ec == error::AddBlockErrorCondition::BLOCK_ADDED) {
        if (ec == error::AddBlockErrorCode::ADDED_TO_MAIN) {
          NOTIFY_NEW_LITE_BLOCK::request arg{std::move(block), height, hops + 1};
          m_p2p->report_success(context.m_remote_ip);
          relay_post_notify<NOTIFY_NEW_LITE_BLOCK>(*m_p2p, arg, &context.m_connection_id);
        } else if (ec == error::AddBlockErrorCode::ADDED_TO_ALTERNATIVE_AND_SWITCHED) {
          NOTIFY_NEW_LITE_BLOCK::request arg{std::move(block), height, hops + 1};
          m_p2p->report_success(context.m_remote_ip);
          relay_post_notify<NOTIFY_NEW_LITE_BLOCK>(*m_p2p, arg, &context.m_connection_id);
          requestMissingPoolTransactions(context);
        } else {
          m_logger(Logging::Trace) << "Pushed lite block already exists.";
        }
      } else if (ec == error::AddBlockErrorCondition::BLOCK_REJECTED) {
        if (context.m_is_light_node) {
          m_logger(Logging::Debugging) << "Orphan block from light node, will drop connection.";
          context.m_state = CryptoNoteConnectionContext::state_shutdown;
        } else {
          context.m_state = CryptoNoteConnectionContext::state_synchronizing;
          NOTIFY_REQUEST_CHAIN::request r = boost::value_initialized<NOTIFY_REQUEST_CHAIN::request>();
          r.block_hashes = m_core.buildSparseChain();
          m_logger(Logging::Trace) << context
                                   << "-->>NOTIFY_REQUEST_CHAIN: m_block_ids.size()=" << r.block_hashes.size();
          post_notify<NOTIFY_REQUEST_CHAIN>(*m_p2p, r, context);
        }
      } else {
        m_logger(Logging::Debugging) << context
                                     << "Block verification failed, dropping connection: " << error.message();
        reportFailureIfSynced(context, P2pPenalty::BlockValidationFailure);
        context.m_state = CryptoNoteConnectionContext::state_shutdown;
      }
    }
  } else {
    // Not an error means we queried missing transactions, we need to gather them now from our peers.
    m_logger(Logging::Trace) << context
                             << "Lite block missing transactions, going to request transactions from publisher.";
    NOTIFY_MISSING_TXS_REQUEST_ENTRY::request missingTxRequest;
    missingTxRequest.current_blockchain_height = BlockHeight::fromIndex(m_core.getTopBlockIndex());
    missingTxRequest.missing_txs = result.take();
    context.m_pending_lite_block = PendingLiteBlock{
        std::move(block), height, hops, {missingTxRequest.missing_txs.begin(), missingTxRequest.missing_txs.end()}};

    if (!post_notify<NOTIFY_MISSING_TXS_REQUEST_ENTRY>(*m_p2p, missingTxRequest, context)) {
      m_logger(Logging::Error)
          << context << "Lite block is missing transactions but the publisher is not reachable, dropping connection.";
      reportFailureIfSynced(context, P2pPenalty::NoResponse);
      context.m_state = CryptoNoteConnectionContext::state_shutdown;
    }
  }

  return 1;
}

void CryptoNoteProtocolHandler::reportFailureIfSynced(CryptoNoteConnectionContext& context, P2pPenalty penalty) {
  if (m_synchronized.load()) {
    m_p2p->report_failure(context.m_remote_ip, penalty);
  }
}

int CryptoNoteProtocolHandler::handle_request_chain(int command, NOTIFY_REQUEST_CHAIN::request& arg,
                                                    CryptoNoteConnectionContext& context) {
  XI_UNUSED(command);
  m_logger(Logging::Trace) << context << "NOTIFY_REQUEST_CHAIN: m_block_ids.size()=" << arg.block_hashes.size();

  if (arg.block_hashes.empty()) {
    m_logger(Logging::Error) << context << "Failed to handle NOTIFY_REQUEST_CHAIN. block_ids is empty";
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  if (arg.block_hashes.back() != m_core.getBlockHashByIndex(0)) {
    m_logger(Logging::Error) << context
                             << "Failed to handle NOTIFY_REQUEST_CHAIN. block_ids doesn't end with genesis block ID";
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }

  NOTIFY_RESPONSE_CHAIN_ENTRY::request r;
  BlockHeight::value_type totalIndex = 0;
  BlockHeight::value_type startIndex = 0;
  auto idQueryResult = m_core.findBlockchainSupplement(
      arg.block_hashes, Xi::Config::Network::blockIdentifiersSynchronizationBatchSize(), totalIndex, startIndex);
  if (idQueryResult.isError()) {
    m_logger(Logging::Error) << context
                             << "Failed to handle NOTIFY_REQUEST_CHAIN. Blockchain supplement query returned error: "
                             << idQueryResult.error().message();
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  }
  r.block_hashes = idQueryResult.take();
  r.total_height = BlockHeight::fromIndex(totalIndex);
  r.start_height = BlockHeight::fromIndex(startIndex);

  m_logger(Logging::Trace) << context << "-->>NOTIFY_RESPONSE_CHAIN_ENTRY: m_start_height=" << r.start_height.native()
                           << ", m_total_height=" << r.total_height.native()
                           << ", m_block_ids.size()=" << r.block_hashes.size();
  if (m_suspiciousGuard.pushAndInspect(context, arg, r)) {
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::SuspiciousRequestSequence);
    P2P_DROP_AND_LOG_RETURN("suspicious sequence of chain requests");
  }
  if (post_notify<NOTIFY_RESPONSE_CHAIN_ENTRY>(*m_p2p, r, context)) {
    m_p2p->report_success(context.m_remote_ip);
  }
  return 1;
}

bool CryptoNoteProtocolHandler::request_missing_objects(CryptoNoteConnectionContext& context,
                                                        bool check_having_blocks) {
  if (context.m_needed_objects.size()) {
    // we know objects that we need, request this objects
    NOTIFY_REQUEST_GET_OBJECTS::request req;
    size_t count = 0;
    auto it = context.m_needed_objects.begin();

    while (it != context.m_needed_objects.end() &&
           count < Xi::Config::Network::blocksP2pSynchronizationMaxBatchSize()) {
      if (!check_having_blocks || m_core.hasBlock(*it)) {
        req.blocks.push_back(*it);
        ++count;
        context.m_requested_objects.insert(*it);
      }
      it = context.m_needed_objects.erase(it);
    }
    m_logger(Logging::Trace) << context << "-->>NOTIFY_REQUEST_GET_OBJECTS: blocks.size()=" << req.blocks.size()
                             << ", txs.size()=" << req.txs.size();
    post_notify<NOTIFY_REQUEST_GET_OBJECTS>(*m_p2p, req, context);
  } else if (context.m_last_response_height.native() <
             context.m_remote_blockchain_height
                 .native()) {  // we have to fetch more objects ids, request blockchain entry
    NOTIFY_REQUEST_CHAIN::request r = boost::value_initialized<NOTIFY_REQUEST_CHAIN::request>();
    r.block_hashes = m_core.buildSparseChain();
    m_logger(Logging::Trace) << context << "-->>NOTIFY_REQUEST_CHAIN: m_block_ids.size()=" << r.block_hashes.size();
    post_notify<NOTIFY_REQUEST_CHAIN>(*m_p2p, r, context);
  } else {
    if (!(context.m_last_response_height == context.m_remote_blockchain_height && !context.m_needed_objects.size() &&
          !context.m_requested_objects.size())) {
      m_logger(Logging::Error) << "request_missing_blocks final condition failed!"
                               << "\r\nm_last_response_height=" << context.m_last_response_height.native()
                               << "\r\nm_remote_blockchain_height=" << context.m_remote_blockchain_height.native()
                               << "\r\nm_needed_objects.size()=" << context.m_needed_objects.size()
                               << "\r\nm_requested_objects.size()=" << context.m_requested_objects.size()
                               << "\r\non connection [" << context << "]";
      return false;
    }

    requestMissingPoolTransactions(context);

    context.m_state = CryptoNoteConnectionContext::state_normal;
    m_logger(Logging::Info, Logging::GREEN) << context << "Successfully synchronized with the XI Network.";
    on_connection_synchronized();
  }
  return true;
}

bool CryptoNoteProtocolHandler::on_connection_synchronized() {
  bool val_expected = false;
  if (m_synchronized.compare_exchange_strong(val_expected, true)) {
    m_logger(Logging::Info) << ENDL;
    m_logger(Info, BRIGHT_MAGENTA) << "===[ " + m_core.currency().coin().name() + " Tip! ]============================="
                                   << ENDL;
    m_logger(Info) << " Always exit " + WalletConfig::daemonName + " and " + WalletConfig::walletName +
                          " with the \"exit\" command to preserve your chain and wallet data."
                   << ENDL;
    m_logger(Info) << " Use the \"help\" command to see a list of available commands." << ENDL;
    m_logger(Info) << " Use the \"backup\" command in " + WalletConfig::walletName +
                          " to display your keys/seed for restoring a corrupted wallet."
                   << ENDL;
    m_logger(Info) << " If you need more assistance, you can contact us for support at " +
                          m_core.currency().general().contactUrl()
                   << ENDL;
    m_logger(Info, BRIGHT_MAGENTA) << "===================================================" << ENDL << ENDL;

    m_logger(Info) << Xi::Config::asciiArt(true) << ENDL;

    m_observerManager.notify(&ICryptoNoteProtocolObserver::blockchainSynchronized,
                             BlockHeight::fromIndex(m_core.getTopBlockIndex()));
  }
  return true;
}

int CryptoNoteProtocolHandler::handle_response_chain_entry(int command, NOTIFY_RESPONSE_CHAIN_ENTRY::request& arg,
                                                           CryptoNoteConnectionContext& context) {
  XI_UNUSED(command);
  m_logger(Logging::Trace) << context << "NOTIFY_RESPONSE_CHAIN_ENTRY: m_block_ids.size()=" << arg.block_hashes.size()
                           << ", m_start_height=" << arg.start_height.native()
                           << ", m_total_height=" << arg.total_height.native();

  if (!arg.block_hashes.size()) {
    m_logger(Logging::Error) << context << "sent empty m_block_ids, dropping connection";
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidResponse);
    return XI_FALSE;
  }

  if (!m_core.hasBlock(arg.block_hashes.front())) {
    m_logger(Logging::Error) << context << "sent m_block_ids starting from unknown id: "
                             << Common::podToHex(arg.block_hashes.front()) << " , dropping connection";
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidResponse);
    return XI_FALSE;
  }

  context.m_remote_blockchain_height = arg.total_height;
  context.m_last_response_height =
      arg.start_height + BlockOffset::fromNative(static_cast<uint32_t>(arg.block_hashes.size() - 1));

  if (context.m_last_response_height > context.m_remote_blockchain_height) {
    m_logger(Logging::Error) << context << "sent wrong NOTIFY_RESPONSE_CHAIN_ENTRY, with \r\nm_total_height="
                             << arg.total_height.native() << "\r\nm_start_height=" << arg.start_height.native()
                             << "\r\nm_block_ids.size()=" << arg.block_hashes.size();
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidResponse);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return XI_FALSE;
  }

  std::copy_if(arg.block_hashes.begin(), arg.block_hashes.end(), std::back_inserter(context.m_needed_objects),
               [this](const auto& blockId) { return !this->m_core.hasBlock(blockId); });
  request_missing_objects(context, false);
  return XI_TRUE;
}

int CryptoNoteProtocolHandler::handleRequestTxPool(int command, NOTIFY_REQUEST_TX_POOL::request& arg,
                                                   CryptoNoteConnectionContext& context) {
  XI_UNUSED(command);
  m_logger(Logging::Trace) << context << "NOTIFY_REQUEST_TX_POOL: txs.size() = " << arg.transactions.size();
  NOTIFY_NEW_TRANSACTIONS::request notification;
  std::vector<Crypto::Hash> deletedTransactions;
  m_core.getPoolChanges(m_core.getTopBlockHash(), arg.transactions, notification.transactions, deletedTransactions);
  if (!notification.transactions.empty()) {
    bool ok = post_notify<NOTIFY_NEW_TRANSACTIONS>(*m_p2p, notification, context);
    if (!ok) {
      m_logger(Logging::Warning) << "Failed to post notification NOTIFY_NEW_TRANSACTIONS to "
                                 << context.m_connection_id;
    }
  }

  return 1;
}

int CryptoNoteProtocolHandler::handle_notify_new_lite_block(int command, NOTIFY_NEW_LITE_BLOCK::request& arg,
                                                            CryptoNoteConnectionContext& context) {
  assert(command == NOTIFY_NEW_LITE_BLOCK::ID);
  XI_UNUSED(command);

  if (m_suspiciousGuard.pushAndInspect(context, arg)) {
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::SuspiciousRequestSequence);
    P2P_DROP_AND_LOG_RETURN("suspicious sequence of liteblock notifications");
  }

  m_logger(Logging::Trace) << context << "handle_notify_new_lite_block (hop " << arg.hop << ")";
  updateObservedHeight(arg.current_blockchain_height, context);
  context.m_remote_blockchain_height = arg.current_blockchain_height;
  if (context.m_state != CryptoNoteConnectionContext::state_normal) {
    return 1;
  }

  if (context.m_pending_lite_block.has_value()) {
    m_logger(Logging::Debugging) << context
                                 << "Tries to send a lite block while we are still wating for pending lite blocks";
    context.m_pending_lite_block = boost::none;
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    return 1;
  }
  return doPushLiteBlock(context, arg.hop, arg.current_blockchain_height, std::move(arg.block), {});
}

int CryptoNoteProtocolHandler::handle_notify_missing_txs_request(int command,
                                                                 NOTIFY_MISSING_TXS_REQUEST_ENTRY::request& arg,
                                                                 CryptoNoteConnectionContext& context) {
  assert(command == NOTIFY_MISSING_TXS_REQUEST_ENTRY::ID);
  XI_UNUSED(command);

  m_logger(Logging::Trace) << context << "NOTIFY_MISSING_TXS";
  updateObservedHeight(arg.current_blockchain_height, context);

  NOTIFY_MISSING_TXS_RESPONSE_ENTRY::request req;

  if (arg.missing_txs.empty()) {
    m_logger(Logging::Error) << context << "request empty, dropping connection";
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    return 1;
  }

  std::vector<BinaryArray> txs;
  std::vector<Crypto::Hash> missedHashes;
  m_core.getTransactions(arg.missing_txs, txs, missedHashes);
  if (!missedHashes.empty()) {
    m_logger(Logging::Error)
        << context
        << "Failed to Handle NOTIFY_MISSING_TXS, Unable to retrieve requested transactions, Dropping Connection";
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidRequest);
    context.m_state = CryptoNoteConnectionContext::state_shutdown;
    return 1;
  } else {
    req.transactions = std::move(txs);
  }

  m_logger(Logging::Debugging) << "--> NOTIFY_RESPONSE_MISSING_TXS: "
                               << "txs.size() = " << req.transactions.size();

  if (m_suspiciousGuard.pushAndInspect(context, arg, req)) {
    m_p2p->report_failure(context.m_remote_ip, P2pPenalty::SuspiciousRequestSequence);
    P2P_DROP_AND_LOG_RETURN("suspicious sequence of missing transactions notifications");
  } else if (post_notify<NOTIFY_MISSING_TXS_RESPONSE_ENTRY>(*m_p2p, req, context)) {
    m_p2p->report_success(context.m_remote_ip);
    m_logger(Logging::Debugging) << "NOTIFY_MISSING_TXS response sent to peer successfully";
  } else {
    m_logger(Logging::Debugging) << "Error while sending NOTIFY_MISSING_TXS response to peer";
  }

  return 1;
}

int CryptoNoteProtocolHandler::handle_notify_missing_txs_response(int command,
                                                                  NOTIFY_MISSING_TXS_RESPONSE_ENTRY::request& arg,
                                                                  CryptoNoteConnectionContext& context) {
#define TXS_RES_DROP(X)                                                    \
  m_logger(Logging::Debugging) << context << X;                            \
  context.m_pending_lite_block = boost::none;                              \
  context.m_state = CryptoNoteConnectionContext::state_shutdown;           \
  m_p2p->report_failure(context.m_remote_ip, P2pPenalty::InvalidResponse); \
  return 1

  assert(command == NOTIFY_MISSING_TXS_RESPONSE_ENTRY::ID);
  XI_UNUSED(command);

  m_logger(Logging::Trace) << "NOTIFY_MISSING_TXS_RESPONSE: txs.size()=" << arg.transactions.size();

  if (!context.m_pending_lite_block.has_value()) {
    TXS_RES_DROP("missing txs received but no lite block is pending, dropping connection.");
  }

  auto& pendingLiteBlock = context.m_pending_lite_block.value();
  std::vector<CachedTransaction> transactions;

  if (pendingLiteBlock.MissingTransactions.size() != arg.transactions.size()) {
    TXS_RES_DROP("publisher sent " << arg.transactions.size() << " transactions but "
                                   << pendingLiteBlock.MissingTransactions.size()
                                   << " were requested, dropping connection.");
  }

  for (uint64_t i = 0; i < arg.transactions.size(); ++i) {
    auto iTxParseResult = CachedTransaction::fromBinaryArray(std::move(arg.transactions[i]));
    if (iTxParseResult.isError()) {
      TXS_RES_DROP("invalid encoded transaction, dropping connection");
    }
    auto iTx = iTxParseResult.take();
    if (pendingLiteBlock.MissingTransactions.find(iTx.getTransactionHash()) ==
        pendingLiteBlock.MissingTransactions.end()) {
      TXS_RES_DROP("transaction hash was not requested, dropping connection");
    }
    transactions.emplace_back(std::move(iTx));
  }

  return doPushLiteBlock(context, pendingLiteBlock.Hops, pendingLiteBlock.Height, std::move(pendingLiteBlock.Block),
                         std::move(transactions));

#undef TXS_RES_DROP
}

void CryptoNoteProtocolHandler::relayBlock(LiteBlock&& arg) {
  NOTIFY_NEW_LITE_BLOCK::request req;
  req.block = std::move(arg);
  req.current_blockchain_height = BlockHeight::fromIndex(m_core.getTopBlockIndex());
  req.hop = 0;
  m_p2p->externalRelayNotifyToAll(NOTIFY_NEW_LITE_BLOCK::ID, LevinProtocol::encode(req), nullptr);
}

void CryptoNoteProtocolHandler::relayTransactions(const std::vector<BinaryArray>& transactions) {
  auto buf = LevinProtocol::encode(NOTIFY_NEW_TRANSACTIONS::request{transactions});
  m_p2p->externalRelayNotifyToAll(NOTIFY_NEW_TRANSACTIONS::ID, buf, nullptr);
}

void CryptoNoteProtocolHandler::requestMissingPoolTransactions(CryptoNoteConnectionContext& context) {
  if (context.version < 1) {
    return;
  }

  NOTIFY_REQUEST_TX_POOL::request notification;
  notification.transactions = m_core.getPoolTransactionHashes();

  bool ok = post_notify<NOTIFY_REQUEST_TX_POOL>(*m_p2p, notification, context);
  if (!ok) {
    m_logger(Logging::Warning) << "Failed to post notification NOTIFY_REQUEST_TX_POOL to " << context.m_connection_id;
  }
}

void CryptoNoteProtocolHandler::updateObservedHeight(BlockHeight peerHeight,
                                                     const CryptoNoteConnectionContext& context) {
  bool updated = false;
  {
    std::lock_guard<std::mutex> lock(m_observedHeightMutex);

    BlockHeight height = m_observedHeight;
    if (!context.m_remote_blockchain_height.isNull() &&
        context.m_last_response_height <= context.m_remote_blockchain_height) {
      m_observedHeight = context.m_remote_blockchain_height;
      if (m_observedHeight != height) {
        updated = true;
      }
    } else if (peerHeight > context.m_remote_blockchain_height) {
      m_observedHeight = std::max(m_observedHeight, peerHeight);
      if (m_observedHeight != height) {
        updated = true;
      }
    } else if (peerHeight != context.m_remote_blockchain_height &&
               context.m_remote_blockchain_height == m_observedHeight) {
      // the client switched to alternative chain and had maximum observed height. need to recalculate max height
      recalculateMaxObservedHeight(context);
      if (m_observedHeight != height) {
        updated = true;
      }
    }
  }

  {
    std::lock_guard<std::mutex> lock(m_blockchainHeightMutex);
    if (peerHeight > m_blockchainHeight) {
      m_blockchainHeight = peerHeight;
      m_logger(Logging::Info) << "New Top Block Detected: " << peerHeight.native();
    }
  }

  if (updated) {
    m_logger(Trace) << "Observed height updated: " << m_observedHeight.native();
    m_observerManager.notify(&ICryptoNoteProtocolObserver::lastKnownBlockHeightUpdated, m_observedHeight);
  }
}

void CryptoNoteProtocolHandler::recalculateMaxObservedHeight(const CryptoNoteConnectionContext& context) {
  // should be locked outside
  BlockHeight peerHeight;
  m_p2p->for_each_connection([&peerHeight, &context](const CryptoNoteConnectionContext& ctx, PeerIdType peerId) {
    XI_UNUSED(peerId);
    if (ctx.m_connection_id != context.m_connection_id) {
      peerHeight = std::max(peerHeight, ctx.m_remote_blockchain_height);
    }
  });

  if (!peerHeight.isNull()) {
    m_observedHeight = std::max(peerHeight, BlockHeight::fromIndex(m_core.getTopBlockIndex()));
  }
  if (context.m_state == CryptoNoteConnectionContext::state_normal) {
    m_observedHeight = BlockHeight::fromIndex(m_core.getTopBlockIndex());
  }
}

BlockHeight CryptoNoteProtocolHandler::getObservedHeight() const {
  std::lock_guard<std::mutex> lock(m_observedHeightMutex);
  return m_observedHeight;
};

BlockHeight CryptoNoteProtocolHandler::getBlockchainHeight() const {
  std::lock_guard<std::mutex> lock(m_blockchainHeightMutex);
  return m_blockchainHeight;
};

bool CryptoNoteProtocolHandler::addObserver(ICryptoNoteProtocolObserver* observer) {
  return m_observerManager.add(observer);
}

bool CryptoNoteProtocolHandler::removeObserver(ICryptoNoteProtocolObserver* observer) {
  return m_observerManager.remove(observer);
}

};  // namespace CryptoNote
