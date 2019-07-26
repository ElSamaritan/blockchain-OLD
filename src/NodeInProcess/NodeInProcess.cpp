/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include "NodeInProcess/NodeInProcess.hpp"

#include <chrono>

#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/Transactions/TransactionApi.h>
#include <CryptoNoteCore/Transactions/ITransactionPool.h>
#include <NodeRpcProxy/NodeErrors.h>

using NodeError = CryptoNote::error::NodeErrorCodes;

#define NODE_CHECK_INITIALIZED                             \
  if (state() != State::Initialized) {                     \
    callback(make_error_code(NodeError::NOT_INITIALIZED)); \
    return;                                                \
  } else if (!m_core.isInitialized()) {                    \
    callback(make_error_code(NodeError::NODE_BUSY));       \
    return;                                                \
  }                                                        \
  try {
#define NODE_SUCCESS                                           \
  callback(std::error_code{});                                 \
  }                                                            \
  catch (...) {                                                \
    callback(make_error_code(NodeError::INTERNAL_NODE_ERROR)); \
  }

namespace CryptoNote {

NodeInProcess::NodeInProcess(NodeServer &p2p, CryptoNoteProtocolHandler &protocol, ICore &core,
                             Logging::ILogger &logger)
    : m_p2p{p2p}, m_protocol{protocol}, m_core{core}, m_logger{logger, "CoreNode"} {
}

NodeInProcess::~NodeInProcess() {
  m_observers.clear();
  if (state() == State::Initialized) {
    shutdown();
  }
}

NodeInProcess::State NodeInProcess::state() const {
  return m_state.load(std::memory_order_consume);
}

void NodeInProcess::blockAdded(uint32_t index, const Crypto::Hash &hash) {
  XI_UNUSED(hash);
  m_observers.notify(&INodeObserver::localBlockchainUpdated, BlockHeight::fromIndex(index));
}

void NodeInProcess::mainChainSwitched(const IBlockchainCache &previous, const IBlockchainCache &current,
                                      uint32_t splitIndex) {
  XI_UNUSED(previous);
  m_observers.notify(&INodeObserver::chainSwitched, BlockHeight::fromIndex(current.getTopBlockIndex()),
                     BlockHeight::fromIndex(splitIndex), std::vector<Crypto::Hash>{});
  m_observers.notify(&INodeObserver::localBlockchainUpdated, BlockHeight::fromIndex(current.getTopBlockIndex()));
}

void NodeInProcess::transactionDeletedFromPool(const Crypto::Hash &hash,
                                               ITransactionPoolObserver::DeletionReason reason) {
  XI_UNUSED(hash, reason);
  m_observers.notify(&INodeObserver::poolChanged);
}

void NodeInProcess::transactionAddedToPool(const Crypto::Hash &hash, ITransactionPoolObserver::AdditionReason reason) {
  XI_UNUSED(hash, reason);
  m_observers.notify(&INodeObserver::poolChanged);
}

void NodeInProcess::peerCountUpdated(size_t count) {
  m_observers.notify(&INodeObserver::peerCountUpdated, count);
}

void NodeInProcess::lastKnownBlockHeightUpdated(BlockHeight height) {
  m_observers.notify(&INodeObserver::lastKnownBlockHeightUpdated, height);
}

void NodeInProcess::blockchainSynchronized(BlockHeight topHeight) {
  m_observers.notify(&INodeObserver::blockchainSynchronized, topHeight);
}

bool NodeInProcess::addObserver(INodeObserver *observer) {
  return m_observers.add(observer);
}

bool NodeInProcess::removeObserver(INodeObserver *observer) {
  return m_observers.remove(observer);
}

void NodeInProcess::init(const INode::Callback &callback) {
  if (state() != State::NotInitialized) {
    callback(make_error_code(NodeError::ALREADY_INITIALIZED));
  }

  m_state.store(State::Initializing);
  m_cachedTopInfo = BlockHeaderInfo{};

  m_initThread = std::thread{[this, callback]() {
    try {
      auto startTime = std::chrono::high_resolution_clock::now();
      while (!m_core.isInitialized()) {
        if (std::chrono::high_resolution_clock::now() - startTime > std::chrono::seconds{30}) {
          callback(make_error_code(NodeError::CONNECT_ERROR));
          return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{20});
      }

      m_protocol.addObserver(this);
      m_core.addObserver(this);
      m_core.transactionPool().addObserver(this);

      m_state.store(State::Initialized);
      callback(std::error_code{});
    } catch (...) {
      callback(make_error_code(NodeError::CONNECT_ERROR));
    }
  }};
}

bool NodeInProcess::shutdown() {
  if (m_initThread.joinable()) {
    m_initThread.join();
  }

  if (state() != State::Initialized) {
    return false;
  } else {
    m_protocol.removeObserver(this);
    m_core.removeObserver(this);
    m_core.transactionPool().removeObserver(this);
    m_state.store(State::NotInitialized);
    return true;
  }
}

size_t NodeInProcess::getPeerCount() const {
  return m_p2p.get_connections_count();
}

BlockHeight NodeInProcess::getLastLocalBlockHeight() const {
  return BlockHeight::fromIndex(m_core.getTopBlockIndex());
}

BlockHeight NodeInProcess::getLastKnownBlockHeight() const {
  return m_protocol.getObservedHeight();
}

BlockVersion NodeInProcess::getLastKnownBlockVersion() const {
  if (state() != State::Initialized || !m_core.isInitialized()) {
    return BlockVersion::Genesis;
  } else {
    return m_core.getTopBlockVersion();
  }
}

uint32_t NodeInProcess::getLocalBlockCount() const {
  return getLastLocalBlockHeight().toSize();
}

uint32_t NodeInProcess::getKnownBlockCount() const {
  return getLastKnownBlockHeight().toSize();
}

uint64_t NodeInProcess::getLastLocalBlockTimestamp() const {
  return m_core.getBlockTimestampByIndex(m_core.getTopBlockIndex());
}

BlockHeight NodeInProcess::getNodeHeight() const {
  return getLastLocalBlockHeight();
}

void NodeInProcess::getFeeInfo() {
  /* */
}

const Currency &NodeInProcess::currency() const {
  return m_core.currency();
}

std::error_code NodeInProcess::ping() {
  if (state() != State::Initialized) {
    return make_error_code(NodeError::NOT_INITIALIZED);
  } else if (!m_core.isInitialized()) {
    return make_error_code(NodeError::NODE_BUSY);
  } else {
    return std::error_code{};
  }
}

void NodeInProcess::getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount,
                                               std::vector<Crypto::Hash> &blockHashes,
                                               const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  blockHashes = m_core.getBlockHashesByTimestamps(timestampBegin, secondsCount);
  NODE_SUCCESS
}

void NodeInProcess::getTransactionHashesByPaymentId(const PaymentId &paymentId,
                                                    std::vector<Crypto::Hash> &transactionHashes,
                                                    const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  transactionHashes = m_core.getTransactionHashesByPaymentId(paymentId);
  NODE_SUCCESS
}

BlockHeaderInfo NodeInProcess::getLastLocalBlockHeaderInfo() const {
  return m_cachedTopInfo;
}

void NodeInProcess::getLastBlockHeaderInfo(BlockHeaderInfo &info, const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  const auto index = m_core.getTopBlockIndex();
  const auto details = m_core.getBlockDetails(m_core.getBlockHashByIndex(index));
  if (!details.has_value()) {
    // TODO logging
    callback(make_error_code(NodeError::INTERNAL_NODE_ERROR));
    return;
  }
  info.height = BlockHeight::fromIndex(index);
  info.version = details->version;
  info.upgradeVote = details->upgradeVote;
  info.timestamp = details->timestamp;
  info.hash = details->hash;
  info.prevHash = details->prevBlockHash;
  info.nonce = details->nonce;
  info.isAlternative = false;
  info.depth = 0;
  info.difficulty = details->difficulty;
  info.reward = details->reward;
  NODE_SUCCESS
}

void NodeInProcess::relayTransaction(const Transaction &transaction, const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  BinaryArray bin{};
  if (!toBinaryArray(transaction, bin)) {
    m_logger(Logging::Error) << "relayTransaction has invalid transaction, not serializable.";
    callback(make_error_code(NodeError::REQUEST_ERROR));
    return;
  }

  if (const auto ec = m_core.transactionPool().pushTransaction(transaction); ec.isError()) {
    if (ec.error().isErrorCode()) {
      callback(ec.error().errorCode());
      return;
    } else {
      callback(make_error_code(NodeError::INTERNAL_NODE_ERROR));
      return;
    }
  } else {
    static_cast<ICryptoNoteProtocol &>(m_protocol).relayTransactions({std::move(bin)});
  }
  NODE_SUCCESS
}

void NodeInProcess::getRandomOutsByAmounts(
    std::map<uint64_t, uint64_t> &&amounts,
    std::vector<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount> &result, const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  result.clear();
  result.reserve(amounts.size());
  for (const auto &[amount, count] : amounts) {
    COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount iOuts{};
    iOuts.amount = amount;
    iOuts.outs.reserve(count);
    std::vector<uint32_t> indices{};
    indices.reserve(count);
    std::vector<Crypto::PublicKey> pks{};
    pks.reserve(count);
    if (!m_core.getRandomOutputs(amount, static_cast<uint16_t>(count), indices, pks)) {
      m_logger(Logging::Warning) << "Failed to query random outputs.";
      callback(make_error_code(NodeError::INTERNAL_NODE_ERROR));
    }
    for (size_t i = 0; i < count && i < indices.size() && i < pks.size(); ++i) {
      COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_out_entry ientry{};
      ientry.global_amount_index = indices[i];
      ientry.out_key = std::move(pks[i]);
      iOuts.outs.emplace_back(std::move(ientry));
    }
  }
  NODE_SUCCESS
}

void NodeInProcess::getRequiredMixinByAmounts(std::set<uint64_t> &&amounts, std::map<uint64_t, uint64_t> &out,
                                              const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  for (const auto amount : amounts) {
    out[amount] = m_core.getCurrentRequiredMixin(amount);
  }
  NODE_SUCCESS
}

void NodeInProcess::getNewBlocks(std::vector<Crypto::Hash> &&knownBlockIds, std::vector<RawBlock> &newBlocks,
                                 BlockHeight &startHeight, const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  if (knownBlockIds.back() != m_core.getBlockHashByIndex(0)) {
    callback(make_error_code(NodeError::REQUEST_ERROR));
    return;
  }

  uint32_t totalBlockCount;
  uint32_t startBlockIndex;
  auto supplementQuery = m_core.findBlockchainSupplement(knownBlockIds, 500, totalBlockCount, startBlockIndex);

  if (supplementQuery.isError()) {
    m_logger(Logging::Error) << "Failed to query blockchain supplement: " << supplementQuery.error().message();
    callback(make_error_code(NodeError::REQUEST_ERROR));
    return;
  }

  startHeight = BlockHeight::fromIndex(startBlockIndex);
  std::vector<Crypto::Hash> missedHashes;
  m_core.getBlocks(supplementQuery.value(), newBlocks, missedHashes);
  if (!missedHashes.empty()) {
    m_logger(Logging::Warning) << "Node queried missing block hashes.";
  }
  NODE_SUCCESS
}

void NodeInProcess::getTransactionOutsGlobalIndices(const Crypto::Hash &transactionHash,
                                                    std::vector<uint32_t> &outsGlobalIndices,
                                                    const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  m_core.getTransactionGlobalIndexes(transactionHash, outsGlobalIndices);
  NODE_SUCCESS
}

void NodeInProcess::queryBlocks(std::vector<Crypto::Hash> &&knownBlockIds, uint64_t timestamp,
                                std::vector<BlockShortEntry> &newBlocks, BlockHeight &startHeight,
                                const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  uint32_t startIndex = 0;
  uint32_t currentIndex = 0;
  uint32_t fullOffset = 0;
  std::vector<BlockShortInfo> infos{};
  m_core.queryBlocksLite(knownBlockIds, timestamp, startIndex, currentIndex, fullOffset, infos);
  startHeight = BlockHeight::fromIndex(startIndex);

  for (auto &item : infos) {
    BlockShortEntry bse;
    bse.hasBlock = false;

    bse.blockHash = std::move(item.block_hash);
    if (!item.block.empty()) {
      if (!fromBinaryArray(bse.block, item.block)) {
        m_logger(Logging::Error) << "failed to desiariled block info returend from daemon.";
        callback(make_error_code(NodeError::RESPONSE_ERROR));
      }

      bse.hasBlock = true;
    }

    for (const auto &txp : item.transaction_prefixes) {
      TransactionShortInfo tsi;
      tsi.txId = txp.hash;
      tsi.txPrefix = txp.prefix;
      bse.txsShortInfo.push_back(std::move(tsi));
    }

    newBlocks.push_back(std::move(bse));
  }

  NODE_SUCCESS
}

void NodeInProcess::getPoolSymmetricDifference(std::vector<Crypto::Hash> &&knownPoolTxIds, Crypto::Hash knownBlockId,
                                               bool &isBcActual,
                                               std::vector<std::unique_ptr<ITransactionReader> > &newTxs,
                                               std::vector<Crypto::Hash> &deletedTxIds,
                                               const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  std::vector<TransactionPrefixInfo> info{};
  m_core.getPoolChangesLite(knownBlockId, knownPoolTxIds, info, deletedTxIds);
  for (const auto &tpi : info) {
    newTxs.push_back(createTransactionPrefix(tpi.prefix, tpi.hash));
  }
  isBcActual = (knownBlockId == m_core.getTopBlockHash());
  NODE_SUCCESS
}

void NodeInProcess::getBlocks(const std::vector<BlockHeight> &blockHeights,
                              std::vector<std::vector<BlockDetails> > &blocks, const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  blocks.resize(1);
  blocks[0].reserve(blockHeights.size());
  for (const auto &height : blockHeights) {
    if (height.isNull()) {
      m_logger(Logging::Error) << "Null height provided in get blocks";
      callback(make_error_code(NodeError::REQUEST_ERROR));
      return;
    }
    auto hash = m_core.getBlockHashByIndex(height.toIndex());
    auto detail = m_core.getBlockDetails(hash);
    if (!detail.has_value()) {
      m_logger(Logging::Error) << "Invalid height provided in get blocks";
      callback(make_error_code(NodeError::REQUEST_ERROR));
      return;
    }
    blocks[0].emplace_back(std::move(*detail));
  }
  NODE_SUCCESS
}

void NodeInProcess::getBlocks(const std::vector<Crypto::Hash> &blockHashes, std::vector<BlockDetails> &blocks,
                              const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  blocks.resize(1);
  blocks.reserve(blockHashes.size());
  for (const auto &hash : blockHashes) {
    auto detail = m_core.getBlockDetails(hash);
    if (!detail.has_value()) {
      m_logger(Logging::Error) << "Invalid hash provided in get blocks";
      callback(make_error_code(NodeError::REQUEST_ERROR));
      return;
    }
    blocks.emplace_back(std::move(*detail));
  }
  NODE_SUCCESS
}

void NodeInProcess::getRawBlocksByRange(BlockHeight height, uint32_t count, std::vector<RawBlock> &blocks,
                                        const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  blocks.reserve(count);
  for (uint32_t i = 0; i < count; height.advance(1), ++i) {
    if (height.isNull()) {
      m_logger(Logging::Error) << "Null height requested in get raw blocks by range";
      callback(make_error_code(NodeError::REQUEST_ERROR));
      return;
    }
    auto block = m_core.getBlockByIndex(height.toIndex());
    if (!block.has_value()) {
      m_logger(Logging::Error) << "Invalid height requested in get raw blocks by range";
      callback(make_error_code(NodeError::REQUEST_ERROR));
      return;
    }
    blocks.emplace_back(block->block.raw());
  }
  NODE_SUCCESS
}

void NodeInProcess::getBlock(const BlockHeight blockHeight, BlockDetails &block, const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  auto hash = m_core.getBlockHashByIndex(blockHeight.toIndex());
  auto blockquery = m_core.getBlockDetails(hash);
  if (!blockquery.has_value()) {
    m_logger(Logging::Error) << "Invalid height requested in get block";
    callback(make_error_code(NodeError::REQUEST_ERROR));
  }
  block = std::move(*blockquery);
  NODE_SUCCESS
}

void NodeInProcess::getTransactions(const std::vector<Crypto::Hash> &transactionHashes,
                                    std::vector<TransactionDetails> &transactions, const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  transactions.reserve(transactionHashes.size());
  for (const auto &hash : transactionHashes) {
    transactions.emplace_back(m_core.getTransactionDetails(hash));
  }
  NODE_SUCCESS
}

void NodeInProcess::isSynchronized(bool &syncStatus, const INode::Callback &callback) {
  NODE_CHECK_INITIALIZED
  syncStatus = getPeerCount() > 0 && m_protocol.isSynchronized();
  NODE_SUCCESS
}

std::optional<FeeAddress> NodeInProcess::feeAddress() const {
  return std::nullopt;
}

}  // namespace CryptoNote
