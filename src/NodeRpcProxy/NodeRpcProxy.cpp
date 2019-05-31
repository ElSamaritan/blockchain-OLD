// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "NodeRpcProxy.h"
#include "NodeErrors.h"

#include <atomic>
#include <ctime>
#include <system_error>
#include <thread>

#include <System/ContextGroup.h>
#include <System/Dispatcher.h>
#include <System/Event.h>
#include <System/EventLock.h>
#include <System/Timer.h>
#include <CryptoNoteCore/Transactions/TransactionApi.h>
#include <Logging/ILogger.h>

#include "Common/StringTools.h"
#include "Common/FormatTools.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"
#include "Rpc/JsonRpc.h"

#ifndef AUTO_VAL_INIT
#define AUTO_VAL_INIT(n) boost::value_initialized<decltype(n)>()
#endif

using namespace Crypto;
using namespace Common;
using namespace Logging;
using namespace System;

namespace CryptoNote {

namespace {

std::error_code interpretResponseStatus(const std::string& status) {
  if (CORE_RPC_STATUS_BUSY == status) {
    return make_error_code(error::NODE_BUSY);
  } else if (CORE_RPC_STATUS_OK != status) {
    return make_error_code(error::INTERNAL_NODE_ERROR);
  }
  return std::error_code();
}

}  // namespace

NodeRpcProxy::NodeRpcProxy(const std::string& nodeHost, unsigned short nodePort, Xi::Http::SSLConfiguration sslConfig,
                           const Currency& currency, Logging::ILogger& logger)
    : m_logger(logger, "NodeRpcProxy"),
      m_currency{currency},
      m_nodeHost(nodeHost),
      m_nodePort(nodePort),
      m_rpcTimeout(10000),
      m_pullInterval(5000),
      m_peerCount(0),
      m_networkHeight(BlockHeight::Null),
      m_nodeHeight(BlockHeight::Null),
      m_connected(true) {
  resetInternalState();
  m_httpClient = std::make_unique<Xi::Http::Client>(nodeHost, nodePort, sslConfig);
}

NodeRpcProxy::~NodeRpcProxy() {
  try {
    shutdown();
  } catch (std::exception& ex) {
    m_logger(FATAL) << "Node could not shutdown properly: " << ex.what();
  }
}

void NodeRpcProxy::resetInternalState() {
  m_stop = false;
  m_peerCount.store(0, std::memory_order_relaxed);
  m_networkHeight.store(BlockHeight::Null, std::memory_order_relaxed);
  lastLocalBlockHeaderInfo.height = BlockHeight::Null;
  lastLocalBlockHeaderInfo.majorVersion = 0;
  lastLocalBlockHeaderInfo.minorVersion = 0;
  lastLocalBlockHeaderInfo.timestamp = 0;
  lastLocalBlockHeaderInfo.hash = Crypto::Hash::Null;
  lastLocalBlockHeaderInfo.prevHash = Crypto::Hash::Null;
  lastLocalBlockHeaderInfo.nonce = BlockNonce::Null;
  lastLocalBlockHeaderInfo.isAlternative = false;
  lastLocalBlockHeaderInfo.depth = 0;
  lastLocalBlockHeaderInfo.difficulty = 0;
  lastLocalBlockHeaderInfo.reward = 0;
  m_knownTxs.clear();
}

void NodeRpcProxy::init(const INode::Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (m_state != STATE_NOT_INITIALIZED) {
    callback(make_error_code(error::ALREADY_INITIALIZED));
    return;
  }

  m_state = STATE_INITIALIZING;
  resetInternalState();
  m_workerThread = std::thread([this, callback] { workerThread(callback); });
}

void NodeRpcProxy::getLastBlockHeaderInfo(BlockHeaderInfo& info, const INode::Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(std::bind(&NodeRpcProxy::doGetLastBlockHeaderInfo, this, std::ref(info)), callback);
}

bool NodeRpcProxy::shutdown() {
  if (m_workerThread.joinable()) {
    m_workerThread.detach();
  }

  m_state = STATE_NOT_INITIALIZED;

  return true;
}

void NodeRpcProxy::workerThread(const INode::Callback& initialized_callback) {
  try {
    Dispatcher dispatcher;
    m_dispatcher = &dispatcher;
    ContextGroup contextGroup(dispatcher);
    m_context_group = &contextGroup;
    Event httpEvent(dispatcher);
    m_httpEvent = &httpEvent;
    m_httpEvent->set();

    if (!ping()) {
      initialized_callback(make_error_code(error::CONNECT_ERROR));
    } else {
      m_connected = true;
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(m_state == STATE_INITIALIZING);
        m_state = STATE_INITIALIZED;
        m_cv_initialized.notify_all();
      }

      try {
        getFeeInfo();
        updateNodeStatus();
      } catch (...) {
        initialized_callback(make_error_code(error::NETWORK_ERROR));
        throw;
      }
      initialized_callback(std::error_code());

      contextGroup.spawn([this]() {
        const uint8_t MaxRetries = 5;
        Timer pullTimer(*m_dispatcher);
        uint8_t continiousExceptionCounter = 0;
        while (!m_stop) {
          try {
            updateNodeStatus();
            continiousExceptionCounter = 0;
          } catch (std::exception& ex) {
            m_logger(ERROR) << "Failed to updated node status (try " << std::to_string(continiousExceptionCounter + 1)
                            << " of " << std::to_string(MaxRetries) << "): " << ex.what();
            if (++continiousExceptionCounter > MaxRetries)
              throw ex;  // The endpoint failed MaxRetries times, we should break here.
            else
              pullTimer.sleep(std::chrono::seconds{1});  // We encountered an error, we should give the endpoint some
                                                         // time to recover before retrying.
          }
          if (!m_stop) {
            pullTimer.sleep(std::chrono::milliseconds(m_pullInterval));
          }
        }
      });

      contextGroup.wait();
      // Make sure all remote spawns are executed
      m_dispatcher->yield();
    }
  } catch (std::exception& ex) {
    m_logger(FATAL) << "Error in node synchronization: '" << ex.what() << "\n', going to shutdown...";
  }

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_state = STATE_NOT_INITIALIZED;
    m_cv_initialized.notify_all();
  }

  m_dispatcher = nullptr;
  m_context_group = nullptr;
  m_httpEvent = nullptr;
  m_connected = false;
  m_rpcProxyObserverManager.notify(&INodeRpcProxyObserver::connectionStatusUpdated, m_connected);
}

void NodeRpcProxy::updateNodeStatus() {
  bool updateBlockchain = true;
  while (pollUpdatesEnabled() && updateBlockchain && !m_stop) {
    updateBlockchainStatus();
    updateBlockchain = !updatePoolStatus();
  }
}

bool NodeRpcProxy::updatePoolStatus() {
  std::vector<Crypto::Hash> knownTxs = getKnownTxsVector();
  Crypto::Hash tailBlock = lastLocalBlockHeaderInfo.hash;

  bool isBcActual = false;
  std::vector<std::unique_ptr<ITransactionReader>> addedTxs;
  std::vector<Crypto::Hash> deletedTxsIds;

  std::error_code ec =
      doGetPoolSymmetricDifference(std::move(knownTxs), tailBlock, isBcActual, addedTxs, deletedTxsIds);
  if (ec) {
    return true;
  }

  if (!isBcActual) {
    return false;
  }

  if (!addedTxs.empty() || !deletedTxsIds.empty()) {
    updatePoolState(addedTxs, deletedTxsIds);
    m_observerManager.notify(&INodeObserver::poolChanged);
  }

  return true;
}

void NodeRpcProxy::updateBlockchainStatus() {
  CryptoNote::COMMAND_RPC_GET_LAST_BLOCK_HEADER::request req = AUTO_VAL_INIT(req);
  CryptoNote::COMMAND_RPC_GET_LAST_BLOCK_HEADER::response rsp = AUTO_VAL_INIT(rsp);

  std::error_code ec = jsonRpcCommand("getlastblockheader", req, rsp);

  if (!ec) {
    Crypto::Hash blockHash = rsp.block_header.hash;
    Crypto::Hash prevBlockHash = rsp.block_header.prev_hash;

    std::unique_lock<std::mutex> lock(m_mutex);
    auto blockHeight = rsp.block_header.height;
    if (blockHash != lastLocalBlockHeaderInfo.hash) {
      lastLocalBlockHeaderInfo.height = blockHeight;
      lastLocalBlockHeaderInfo.majorVersion = rsp.block_header.major_version;
      lastLocalBlockHeaderInfo.minorVersion = rsp.block_header.minor_version;
      lastLocalBlockHeaderInfo.timestamp = rsp.block_header.timestamp;
      lastLocalBlockHeaderInfo.hash = blockHash;
      lastLocalBlockHeaderInfo.prevHash = prevBlockHash;
      lastLocalBlockHeaderInfo.nonce = rsp.block_header.nonce;
      lastLocalBlockHeaderInfo.isAlternative = rsp.block_header.orphan_status;
      lastLocalBlockHeaderInfo.depth = rsp.block_header.depth;
      lastLocalBlockHeaderInfo.difficulty = rsp.block_header.difficulty;
      lastLocalBlockHeaderInfo.reward = rsp.block_header.reward;
      lock.unlock();
      m_observerManager.notify(&INodeObserver::localBlockchainUpdated, blockHeight);
    }
  }

  CryptoNote::COMMAND_RPC_GET_INFO::request getInfoReq = AUTO_VAL_INIT(getInfoReq);
  CryptoNote::COMMAND_RPC_GET_INFO::response getInfoResp = AUTO_VAL_INIT(getInfoResp);

  ec = jsonCommand("/getinfo", getInfoReq, getInfoResp);
  if (!ec) {
    // a quirk to let wallets work with previous versions daemons.
    // Previous daemons didn't have the 'last_known_block_index' parameter in RPC so it may have zero value.
    std::unique_lock<std::mutex> lock(m_mutex);
    auto lastKnownBlockHeight = std::max(getInfoResp.last_known_block_height, lastLocalBlockHeaderInfo.height);
    lock.unlock();
    if (m_networkHeight.load(std::memory_order_relaxed) != lastKnownBlockHeight) {
      m_networkHeight.store(lastKnownBlockHeight, std::memory_order_relaxed);
      m_observerManager.notify(&INodeObserver::lastKnownBlockHeightUpdated,
                               m_networkHeight.load(std::memory_order_relaxed));
    }

    m_nodeHeight.store(getInfoResp.height, std::memory_order_relaxed);

    updatePeerCount(getInfoResp.incoming_connections_count + getInfoResp.outgoing_connections_count);

    if (!m_connected) {
      m_connected = true;
      m_rpcProxyObserverManager.notify(&INodeRpcProxyObserver::connectionStatusUpdated, m_connected);
    }
  } else if (m_connected) {
    m_connected = false;
    m_rpcProxyObserverManager.notify(&INodeRpcProxyObserver::connectionStatusUpdated, m_connected);
  }
}

void NodeRpcProxy::updatePeerCount(size_t peerCount) {
  if (peerCount != m_peerCount) {
    m_peerCount = peerCount;
    m_observerManager.notify(&INodeObserver::peerCountUpdated, m_peerCount.load(std::memory_order_relaxed));
  }
}

void NodeRpcProxy::updatePoolState(const std::vector<std::unique_ptr<ITransactionReader>>& addedTxs,
                                   const std::vector<Crypto::Hash>& deletedTxsIds) {
  for (const auto& hash : deletedTxsIds) {
    m_knownTxs.erase(hash);
  }

  for (const auto& tx : addedTxs) {
    Hash hash = tx->getTransactionHash();
    m_knownTxs.emplace(std::move(hash));
  }
}

std::error_code NodeRpcProxy::doGetLastBlockHeaderInfo(BlockHeaderInfo& info) {
  CryptoNote::COMMAND_RPC_GET_LAST_BLOCK_HEADER::request req = AUTO_VAL_INIT(req);
  CryptoNote::COMMAND_RPC_GET_LAST_BLOCK_HEADER::response rsp = AUTO_VAL_INIT(rsp);

  std::error_code ec = jsonRpcCommand("getlastblockheader", req, rsp);

  if (!ec) {
    Crypto::Hash blockHash = rsp.block_header.hash;
    Crypto::Hash prevBlockHash = rsp.block_header.prev_hash;

    info.height = rsp.block_header.height;
    info.majorVersion = rsp.block_header.major_version;
    info.minorVersion = rsp.block_header.minor_version;
    info.timestamp = rsp.block_header.timestamp;
    info.hash = blockHash;
    info.prevHash = prevBlockHash;
    info.nonce = rsp.block_header.nonce;
    info.isAlternative = rsp.block_header.orphan_status;
    info.depth = rsp.block_header.depth;
    info.difficulty = rsp.block_header.difficulty;
    info.reward = rsp.block_header.reward;
  }
  return ec;
}

void NodeRpcProxy::getFeeInfo() {
  CryptoNote::COMMAND_RPC_GET_FEE_ADDRESS::request ireq = AUTO_VAL_INIT(ireq);
  CryptoNote::COMMAND_RPC_GET_FEE_ADDRESS::response iresp = AUTO_VAL_INIT(iresp);

  std::error_code ec = jsonCommand("/feeinfo", ireq, iresp);

  if (ec || iresp.status != CORE_RPC_STATUS_OK) {
    return;
  }

  if (iresp.fee && !iresp.fee->address.isValid()) {
    return;
  }

  m_fee = iresp.fee;

  return;
}

const Currency& NodeRpcProxy::currency() const { return m_currency; }

bool NodeRpcProxy::ping() {
  try {
    CryptoNote::COMMAND_RPC_GET_FEE_ADDRESS::request ireq = AUTO_VAL_INIT(ireq);
    CryptoNote::COMMAND_RPC_GET_FEE_ADDRESS::response iresp = AUTO_VAL_INIT(iresp);

    std::error_code ec = jsonCommand("/feeinfo", ireq, iresp);

    if (ec || iresp.status != CORE_RPC_STATUS_OK) {
      return false;
    }
    return true;
  } catch (...) {
    return false;
  }
}

std::optional<FeeAddress> NodeRpcProxy::feeAddress() const { return m_fee; }

void NodeRpcProxy::setPollUpdatesEnabled(bool enabled) { m_pollUpdates.store(enabled); }

bool NodeRpcProxy::pollUpdatesEnabled() const { return m_pollUpdates.load(); }

std::string NodeRpcProxy::getInfo() {
  CryptoNote::COMMAND_RPC_GET_INFO::request ireq;
  CryptoNote::COMMAND_RPC_GET_INFO::response iresp;

  std::error_code ec = jsonCommand("/getinfo", ireq, iresp);

  if (ec || iresp.status != CORE_RPC_STATUS_OK) {
    return std::string("Problem retrieving information from RPC server.");
  }

  return Common::get_status_string(iresp);
}

std::vector<Crypto::Hash> NodeRpcProxy::getKnownTxsVector() const {
  return std::vector<Crypto::Hash>(m_knownTxs.begin(), m_knownTxs.end());
}

bool NodeRpcProxy::addObserver(INodeObserver* observer) { return m_observerManager.add(observer); }

bool NodeRpcProxy::removeObserver(INodeObserver* observer) { return m_observerManager.remove(observer); }

bool NodeRpcProxy::addObserver(CryptoNote::INodeRpcProxyObserver* observer) {
  return m_rpcProxyObserverManager.add(observer);
}

bool NodeRpcProxy::removeObserver(CryptoNote::INodeRpcProxyObserver* observer) {
  return m_rpcProxyObserverManager.remove(observer);
}

size_t NodeRpcProxy::getPeerCount() const { return m_peerCount.load(std::memory_order_relaxed); }

BlockHeight NodeRpcProxy::getLastLocalBlockHeight() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return lastLocalBlockHeaderInfo.height;
}

BlockHeight NodeRpcProxy::getLastKnownBlockHeight() const { return m_networkHeight.load(std::memory_order_relaxed); }

uint32_t NodeRpcProxy::getLocalBlockCount() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return lastLocalBlockHeaderInfo.height.native();
}

uint32_t NodeRpcProxy::getKnownBlockCount() const { return m_networkHeight.load(std::memory_order_relaxed).native(); }

BlockHeight NodeRpcProxy::getNodeHeight() const { return m_nodeHeight.load(std::memory_order_relaxed); }

uint64_t NodeRpcProxy::getLastLocalBlockTimestamp() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return lastLocalBlockHeaderInfo.timestamp;
}

BlockHeaderInfo NodeRpcProxy::getLastLocalBlockHeaderInfo() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return lastLocalBlockHeaderInfo;
}

void NodeRpcProxy::getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount,
                                              std::vector<Crypto::Hash>& blockHashes, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(
      std::bind(&NodeRpcProxy::doGetBlockHashesByTimestamps, this, timestampBegin, secondsCount, std::ref(blockHashes)),
      callback);
}

void NodeRpcProxy::getTransactionHashesByPaymentId(const Crypto::Hash& paymentId,
                                                   std::vector<Crypto::Hash>& transactionHashes,
                                                   const INode::Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(std::bind(&NodeRpcProxy::doGetTransactionHashesByPaymentId, this, std::cref(paymentId),
                            std::ref(transactionHashes)),
                  callback);
}

std::error_code NodeRpcProxy::doGetBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount,
                                                           std::vector<Crypto::Hash>& blockHashes) {
  COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::request req = AUTO_VAL_INIT(req);
  COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::response rsp = AUTO_VAL_INIT(rsp);

  req.timestamp_begin = timestampBegin;
  req.timestamp_end = secondsCount;

  std::error_code ec = jsonCommand("/get_blocks_hashes_by_timestamps", req, rsp);
  if (!ec) {
    blockHashes = std::move(rsp.block_hashes);
  }

  return ec;
}

void NodeRpcProxy::relayTransaction(const CryptoNote::Transaction& transaction, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(std::bind(&NodeRpcProxy::doRelayTransaction, this, transaction), callback);
}

void NodeRpcProxy::getRandomOutsByAmounts(
    std::vector<uint64_t>&& amounts, uint16_t outsCount,
    std::vector<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& outs, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(
      std::bind(&NodeRpcProxy::doGetRandomOutsByAmounts, this, std::move(amounts), outsCount, std::ref(outs)),
      callback);
}

void NodeRpcProxy::getNewBlocks(std::vector<Crypto::Hash>&& knownBlockIds, std::vector<CryptoNote::RawBlock>& newBlocks,
                                BlockHeight& startHeight, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(std::bind(&NodeRpcProxy::doGetNewBlocks, this, std::move(knownBlockIds), std::ref(newBlocks),
                            std::ref(startHeight)),
                  callback);
}

void NodeRpcProxy::getTransactionOutsGlobalIndices(const Crypto::Hash& transactionHash,
                                                   std::vector<uint32_t>& outsGlobalIndices, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(
      std::bind(&NodeRpcProxy::doGetTransactionOutsGlobalIndices, this, transactionHash, std::ref(outsGlobalIndices)),
      callback);
}

void NodeRpcProxy::queryBlocks(std::vector<Crypto::Hash>&& knownBlockIds, uint64_t timestamp,
                               std::vector<BlockShortEntry>& newBlocks, BlockHeight& startHeight,
                               const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(std::bind(&NodeRpcProxy::doQueryBlocksLite, this, std::move(knownBlockIds), timestamp,
                            std::ref(newBlocks), std::ref(startHeight)),
                  callback);
}

void NodeRpcProxy::getPoolSymmetricDifference(std::vector<Crypto::Hash>&& knownPoolTxIds, Crypto::Hash knownBlockId,
                                              bool& isBcActual,
                                              std::vector<std::unique_ptr<ITransactionReader>>& newTxs,
                                              std::vector<Crypto::Hash>& deletedTxIds, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(
      [this, knownPoolTxIds, knownBlockId, &isBcActual, &newTxs, &deletedTxIds]() mutable -> std::error_code {
        return this->doGetPoolSymmetricDifference(std::move(knownPoolTxIds), knownBlockId, isBcActual, newTxs,
                                                  deletedTxIds);
      },
      callback);
}

void NodeRpcProxy::getBlocks(const std::vector<BlockHeight>& blockHeights,
                             std::vector<std::vector<BlockDetails>>& blocks, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(std::bind(&NodeRpcProxy::doGetBlocksByHeight, this, std::cref(blockHeights), std::ref(blocks)),
                  callback);
}

void NodeRpcProxy::getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<BlockDetails>& blocks,
                             const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(std::bind(&NodeRpcProxy::doGetBlocksByHash, this, std::cref(blockHashes), std::ref(blocks)),
                  callback);
}

void NodeRpcProxy::getRawBlocksByRange(BlockHeight height, uint32_t count, std::vector<RawBlock>& blocks,
                                       const INode::Callback& callback) {
  std::lock_guard<std::mutex> lock{m_mutex};

  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(std::bind(&NodeRpcProxy::doGetRawBlocksByRange, this, height, count, std::ref(blocks)), callback);
}

void NodeRpcProxy::getBlock(const BlockHeight blockHeight, BlockDetails& block, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(std::bind(&NodeRpcProxy::doGetBlock, this, blockHeight, std::ref(block)), callback);
}

void NodeRpcProxy::getTransactions(const std::vector<Crypto::Hash>& transactionHashes,
                                   std::vector<TransactionDetails>& transactions, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  scheduleRequest(
      std::bind(&NodeRpcProxy::doGetTransactions, this, std::cref(transactionHashes), std::ref(transactions)),
      callback);
}

void NodeRpcProxy::isSynchronized(bool& syncStatus, const Callback& callback) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_state != STATE_INITIALIZED) {
    callback(make_error_code(error::NOT_INITIALIZED));
    return;
  }

  syncStatus = getPeerCount() > 0 && getNodeHeight() >= getLastKnownBlockHeight();
  callback(std::error_code());
}

std::error_code NodeRpcProxy::doRelayTransaction(const CryptoNote::Transaction& transaction) {
  COMMAND_RPC_SEND_RAW_TX::request req;
  COMMAND_RPC_SEND_RAW_TX::response rsp;
  req.transaction = transaction;
  m_logger(TRACE) << "NodeRpcProxy::doRelayTransaction";
  return jsonCommand("/sendrawtransaction", req, rsp);
}

std::error_code NodeRpcProxy::doGetRandomOutsByAmounts(
    std::vector<uint64_t>& amounts, uint16_t outsCount,
    std::vector<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& outs) {
  COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::request req = AUTO_VAL_INIT(req);
  COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::response rsp = AUTO_VAL_INIT(rsp);
  req.amounts = std::move(amounts);
  req.outs_count = outsCount;

  m_logger(TRACE) << "Send getrandom_outs request";
  std::error_code ec = jsonCommand("/getrandom_outs", req, rsp);
  if (!ec) {
    m_logger(TRACE) << "getrandom_outs complete";
    outs = std::move(rsp.outs);
  } else {
    m_logger(TRACE) << "getrandom_outs failed: " << ec << ", " << ec.message();
  }

  return ec;
}

std::error_code NodeRpcProxy::doGetNewBlocks(std::vector<Crypto::Hash>& knownBlockIds,
                                             std::vector<CryptoNote::RawBlock>& newBlocks, BlockHeight& startHeight) {
  CryptoNote::COMMAND_RPC_GET_BLOCKS_FAST::request req = AUTO_VAL_INIT(req);
  CryptoNote::COMMAND_RPC_GET_BLOCKS_FAST::response rsp = AUTO_VAL_INIT(rsp);
  req.block_hashes = std::move(knownBlockIds);

  m_logger(TRACE) << "Send getblocks request";
  std::error_code ec = jsonCommand("/getblocks", req, rsp);
  if (!ec) {
    m_logger(TRACE) << "getblocks complete, start_height " << rsp.start_height.native() << ", block count "
                    << rsp.blocks.size();
    newBlocks = std::move(rsp.blocks);
    startHeight = rsp.start_height;
  } else {
    m_logger(TRACE) << "getblocks failed: " << ec << ", " << ec.message();
  }

  return ec;
}

std::error_code NodeRpcProxy::doGetTransactionOutsGlobalIndices(const Crypto::Hash& transactionHash,
                                                                std::vector<uint32_t>& outsGlobalIndices) {
  CryptoNote::COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::request req = AUTO_VAL_INIT(req);
  CryptoNote::COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::response rsp = AUTO_VAL_INIT(rsp);
  req.transaction_hash = transactionHash;

  m_logger(TRACE) << "Send get_o_indexes request, transaction " << req.transaction_hash;
  std::error_code ec = jsonCommand("/get_o_indexes", req, rsp);
  if (!ec) {
    m_logger(TRACE) << "get_o_indexes complete";
    outsGlobalIndices.clear();
    for (auto idx : rsp.output_indices) {
      outsGlobalIndices.push_back(static_cast<uint32_t>(idx));
    }
  } else {
    m_logger(TRACE) << "get_o_indexes failed: " << ec << ", " << ec.message();
  }

  return ec;
}

std::error_code NodeRpcProxy::doQueryBlocksLite(const std::vector<Crypto::Hash>& knownBlockIds, uint64_t timestamp,
                                                std::vector<CryptoNote::BlockShortEntry>& newBlocks,
                                                BlockHeight& startHeight) {
  CryptoNote::COMMAND_RPC_QUERY_BLOCKS_LITE::request req = AUTO_VAL_INIT(req);
  CryptoNote::COMMAND_RPC_QUERY_BLOCKS_LITE::response rsp = AUTO_VAL_INIT(rsp);

  req.block_hashes = knownBlockIds;
  req.timestamp = timestamp;

  m_logger(TRACE) << "Send queryblockslite request, timestamp " << req.timestamp;
  std::error_code ec = jsonCommand("/queryblockslite", req, rsp);
  if (ec) {
    m_logger(TRACE) << "queryblockslite failed: " << ec << ", " << ec.message();
    return ec;
  }

  m_logger(TRACE) << "queryblockslite complete, startHeight " << rsp.start_height.native() << ", block count "
                  << rsp.blocks.size();
  startHeight = rsp.start_height;

  for (auto& item : rsp.blocks) {
    BlockShortEntry bse;
    bse.hasBlock = false;

    bse.blockHash = std::move(item.block_hash);
    if (!item.block.empty()) {
      if (!fromBinaryArray(bse.block, item.block)) {
        return std::make_error_code(std::errc::invalid_argument);
      }

      bse.hasBlock = true;
    }

    for (const auto& txp : item.transaction_prefixes) {
      TransactionShortInfo tsi;
      tsi.txId = txp.hash;
      tsi.txPrefix = txp.prefix;
      bse.txsShortInfo.push_back(std::move(tsi));
    }

    newBlocks.push_back(std::move(bse));
  }

  return std::error_code();
}

std::error_code NodeRpcProxy::doGetPoolSymmetricDifference(std::vector<Crypto::Hash>&& knownPoolTxIds,
                                                           Crypto::Hash knownBlockId, bool& isBcActual,
                                                           std::vector<std::unique_ptr<ITransactionReader>>& newTxs,
                                                           std::vector<Crypto::Hash>& deletedTxIds) {
  CryptoNote::COMMAND_RPC_GET_POOL_CHANGES_LITE::request req = AUTO_VAL_INIT(req);
  CryptoNote::COMMAND_RPC_GET_POOL_CHANGES_LITE::response rsp = AUTO_VAL_INIT(rsp);

  req.tail_block_hash = knownBlockId;
  req.known_transaction_hashes = knownPoolTxIds;

  m_logger(TRACE) << "Send get_pool_changes_lite request, tailBlockId " << req.tail_block_hash;
  std::error_code ec = jsonCommand("/get_pool_changes_lite", req, rsp);

  if (ec) {
    m_logger(TRACE) << "get_pool_changes_lite failed: " << ec << ", " << ec.message();
    return ec;
  }

  m_logger(TRACE) << "get_pool_changes_lite complete, isTailBlockActual " << rsp.is_current_tail_block;
  isBcActual = rsp.is_current_tail_block;

  deletedTxIds = std::move(rsp.deleted_transaction_hashes);

  for (const auto& tpi : rsp.added_transactions) {
    newTxs.push_back(createTransactionPrefix(tpi.prefix, tpi.hash));
  }

  return ec;
}

std::error_code NodeRpcProxy::doGetBlocksByHeight(const std::vector<BlockHeight>& blockHeights,
                                                  std::vector<std::vector<BlockDetails>>& blocks) {
  COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS::request req = AUTO_VAL_INIT(req);
  COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS::response resp = AUTO_VAL_INIT(resp);

  req.block_heights = blockHeights;

  std::error_code ec = jsonCommand("/get_blocks_details_by_heights", req, resp);
  if (ec) {
    return ec;
  }

  auto tmp = std::move(resp.blocks);
  blocks.push_back(tmp);

  return ec;
}

std::error_code NodeRpcProxy::doGetBlocksByHash(const std::vector<Crypto::Hash>& blockHashes,
                                                std::vector<BlockDetails>& blocks) {
  COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::request req = AUTO_VAL_INIT(req);
  COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::response resp = AUTO_VAL_INIT(resp);

  req.block_hashes = blockHashes;

  std::error_code ec = jsonCommand("/get_blocks_details_by_hashes", req, resp);
  if (ec) {
    return ec;
  }

  blocks = std::move(resp.blocks);
  return ec;
}

std::error_code NodeRpcProxy::doGetRawBlocksByRange(BlockHeight height, uint32_t count, std::vector<RawBlock>& blocks) {
  F_COMMAND_RPC_GET_BLOCKS_RAW_BY_RANGE::request req = AUTO_VAL_INIT(req);
  F_COMMAND_RPC_GET_BLOCKS_RAW_BY_RANGE::response resp = AUTO_VAL_INIT(resp);

  req.height = height;
  req.count = count;

  std::error_code ec = jsonRpcCommand("f_blocks_list_raw", req, resp);
  if (ec) {
    return ec;
  }

  size_t blocksOffset = blocks.size();
  blocks.reserve(blocksOffset + resp.blocks.size());
  for (const auto& block : resp.blocks) {
    RawBlock iRawBlock{};
    iRawBlock.block = toBinaryArray(block);
    blocks.emplace_back(std::move(iRawBlock));
  }

  return ec;
}

std::error_code NodeRpcProxy::doGetBlock(const BlockHeight blockHeight, BlockDetails& block) {
  COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::request req = AUTO_VAL_INIT(req);
  COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::response resp = AUTO_VAL_INIT(resp);

  req.block_height = blockHeight;

  std::error_code ec = jsonCommand("/get_block_details_by_height", req, resp);

  if (ec) {
    return ec;
  }

  block = std::move(resp.block);

  return ec;
}

std::error_code NodeRpcProxy::doGetTransactionHashesByPaymentId(const Crypto::Hash& paymentId,
                                                                std::vector<Crypto::Hash>& transactionHashes) {
  COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::request req = AUTO_VAL_INIT(req);
  COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::response resp = AUTO_VAL_INIT(resp);

  req.payment_id = paymentId;
  std::error_code ec = jsonCommand("/get_transaction_hashes_by_payment_id", req, resp);
  if (ec) {
    return ec;
  }

  transactionHashes = std::move(resp.transactionHashes);
  return ec;
}

std::error_code NodeRpcProxy::doGetTransactions(const std::vector<Crypto::Hash>& transactionHashes,
                                                std::vector<TransactionDetails>& transactions) {
  COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::request req = AUTO_VAL_INIT(req);
  COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::response resp = AUTO_VAL_INIT(resp);

  req.transaction_hashes = transactionHashes;
  std::error_code ec = jsonCommand("/get_transaction_details_by_hashes", req, resp);
  if (ec) {
    return ec;
  }

  transactions = std::move(resp.transactions);
  return ec;
}

void NodeRpcProxy::scheduleRequest(std::function<std::error_code()>&& procedure, const Callback& callback) {
  // callback is located on stack, so copy it inside binder
  class Wrapper {
   public:
    Wrapper(std::function<void(std::function<std::error_code()>&, Callback&)>&& _func,
            std::function<std::error_code()>&& _procedure, const Callback& _callback)
        : func(std::move(_func)), procedure(std::move(_procedure)), callback(std::move(_callback)) {}
    Wrapper(const Wrapper& other) : func(other.func), procedure(other.procedure), callback(other.callback) {}
    Wrapper(Wrapper&& other)  // must be noexcept
        : func(std::move(other.func)), procedure(std::move(other.procedure)), callback(std::move(other.callback)) {}
    void operator()() { func(procedure, callback); }

   private:
    std::function<void(std::function<std::error_code()>&, Callback&)> func;
    std::function<std::error_code()> procedure;
    Callback callback;
  };
  assert(m_dispatcher != nullptr && m_context_group != nullptr);
  m_dispatcher->remoteSpawn(Wrapper(
      [this](std::function<std::error_code()>& procedure, Callback& callback) {
        m_context_group->spawn(Wrapper(
            [this](std::function<std::error_code()>& procedure, const Callback& callback) {
              if (m_stop) {
                callback(std::make_error_code(std::errc::operation_canceled));
              } else {
                std::error_code ec = procedure();
                callback(m_stop ? std::make_error_code(std::errc::operation_canceled) : ec);
              }
            },
            std::move(procedure), std::move(callback)));
      },
      std::move(procedure), callback));
}

template <typename Request, typename Response>
std::error_code NodeRpcProxy::binaryCommand(const std::string& url, const Request& req, Response& res) {
  std::error_code ec;

  try {
    invokeBinaryCommand(*m_httpClient, url, req, res);
    ec = interpretResponseStatus(res.status);
  } catch (const std::exception&) {
    ec = make_error_code(error::NETWORK_ERROR);
  }

  return ec;
}

namespace {
template <typename Request, typename Response>
void invokeJsonCommand(HttpClient& client, const std::string& url, const Request& req, Response& res) {
  using namespace ::Xi::Http;

  const auto response = client.postSync(url, Xi::Http::ContentType::Json, storeToJson(req));
  if (response.status() != StatusCode::Ok) {
    throw std::runtime_error("HTTP status: " + Xi::to_string(response.status()));
  }

  if (!loadFromJson(res, response.body())) {
    throw std::runtime_error("Failed to parse JSON response");
  }
}
}  // namespace

template <typename Request, typename Response>
std::error_code NodeRpcProxy::jsonCommand(const std::string& url, const Request& req, Response& res) {
  std::error_code ec;

  try {
    m_logger(TRACE) << "Send " << url << " JSON request";
    invokeJsonCommand(*m_httpClient, url, req, res);
    ec = interpretResponseStatus(res.status);
  } catch (const std::exception&) {
    ec = make_error_code(error::NETWORK_ERROR);
  }

  if (ec) {
    m_logger(TRACE) << url << " JSON request failed: " << ec << ", " << ec.message();
  } else {
    m_logger(TRACE) << url << " JSON request compete";
  }

  return ec;
}

template <typename Request, typename Response>
std::error_code NodeRpcProxy::jsonRpcCommand(const std::string& method, const Request& req, Response& res) {
  using namespace ::Xi::Http;
  std::error_code ec = make_error_code(error::INTERNAL_NODE_ERROR);

  try {
    m_logger(TRACE) << "Send " << method << " JSON RPC request";

    JsonRpc::JsonRpcRequest jsReq;

    jsReq.setMethod(method);
    jsReq.setParams(req);

    const auto httpRes = m_httpClient->postSync("/json_rpc", ContentType::Json, jsReq.getBody());

    JsonRpc::JsonRpcResponse jsRes;

    if (httpRes.status() == StatusCode::Ok) {
      jsRes.parse(httpRes.body());
      if (jsRes.getResult(res)) {
        ec = interpretResponseStatus(res.status);
      }
    }
  } catch (const std::exception&) {
    ec = make_error_code(error::NETWORK_ERROR);
  }

  if (ec) {
    m_logger(TRACE) << method << " JSON RPC request failed: " << ec << ", " << ec.message();
  } else {
    m_logger(TRACE) << method << " JSON RPC request compete";
  }

  return ec;
}

}  // namespace CryptoNote
