﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "BlockchainSynchronizer.h"

#include <functional>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <thread>
#include <chrono>

#include "Common/StreamTools.h"
#include "Common/StringTools.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteCore/Transactions/TransactionApi.h"

using namespace Common;
using namespace Crypto;
using namespace Logging;

namespace {

const int RETRY_TIMEOUT = 5;

class TransactionReaderListFormatter {
 public:
  explicit TransactionReaderListFormatter(
      const std::vector<std::unique_ptr<CryptoNote::ITransactionReader>>& transactionList)
      : m_transactionList(transactionList) {
  }

  void print(std::ostream& os) const {
    os << '{';

    if (!m_transactionList.empty()) {
      os << m_transactionList.front()->getTransactionHash();
      for (auto it = std::next(m_transactionList.begin()); it != m_transactionList.end(); ++it) {
        os << ", " << (*it)->getTransactionHash();
      }
    }

    os << '}';
  }

  friend std::ostream& operator<<(std::ostream& os, const TransactionReaderListFormatter& formatter) {
    formatter.print(os);
    return os;
  }

 private:
  const std::vector<std::unique_ptr<CryptoNote::ITransactionReader>>& m_transactionList;
};

}  // namespace

namespace CryptoNote {

BlockchainSynchronizer::BlockchainSynchronizer(INode& node, const Currency& currency, Logging::ILogger& logger)
    : m_logger(logger, "BlockchainSynchronizer"),
      m_node(node),
      m_currency(currency),
      m_genesisBlockHash(currency.genesisBlockHash()),
      m_currentState(State::stopped),
      m_futureState(State::stopped) {
}

BlockchainSynchronizer::~BlockchainSynchronizer() {
  stop();
}

void BlockchainSynchronizer::addConsumer(IBlockchainConsumer* consumer) {
  assert(consumer != nullptr);
  assert(m_consumers.count(consumer) == 0);

  if (!(checkIfStopped() && checkIfShouldStop())) {
    auto message = "Failed to add consumer: not stopped";
    m_logger(Error) << message << ", consumer " << consumer;
    throw std::runtime_error(message);
  }

  m_consumers.insert(std::make_pair(consumer, std::make_shared<SynchronizationState>(m_genesisBlockHash)));
  m_logger(Info) << "Consumer added, consumer " << consumer << ", count " << m_consumers.size();
}

bool BlockchainSynchronizer::removeConsumer(IBlockchainConsumer* consumer) {
  assert(consumer != nullptr);

  if (!(checkIfStopped() && checkIfShouldStop())) {
    auto message = "Failed to remove consumer: not stopped";
    m_logger(Error) << message << ", consumer " << consumer;
    throw std::runtime_error(message);
  }

  bool result = m_consumers.erase(consumer) > 0;
  if (result) {
    m_logger(Info) << "Consumer removed, consumer " << consumer << ", count " << m_consumers.size();
  } else {
    m_logger(Error) << "Failed to remove consumer: not found, consumer " << consumer;
  }

  return result;
}

IStreamSerializable* BlockchainSynchronizer::getConsumerState(IBlockchainConsumer* consumer) const {
  std::unique_lock<std::mutex> lk(m_consumersMutex);
  return getConsumerSynchronizationState(consumer);
}

std::vector<Crypto::Hash> BlockchainSynchronizer::getConsumerKnownBlocks(IBlockchainConsumer& consumer) const {
  std::unique_lock<std::mutex> lk(m_consumersMutex);

  auto state = getConsumerSynchronizationState(&consumer);
  if (state == nullptr) {
    auto message = "Failed to get consumer known blocks: not found";
    m_logger(Error) << message << ", consumer " << &consumer;
    throw std::invalid_argument(message);
  }

  return state->getKnownBlockHashes();
}

std::future<std::error_code> BlockchainSynchronizer::addUnconfirmedTransaction(const ITransactionReader& transaction) {
  m_logger(Info) << "Adding unconfirmed transaction, hash " << transaction.getTransactionHash();

  std::unique_lock<std::mutex> lock(m_stateMutex);

  if (m_currentState == State::stopped || m_futureState == State::stopped) {
    auto message = "Failed to add unconfirmed transaction: not stopped";
    m_logger(Error) << message << ", hash " << transaction.getTransactionHash();
    throw std::runtime_error(message);
  }

  std::promise<std::error_code> promise;
  auto future = promise.get_future();
  m_addTransactionTasks.emplace_back(&transaction, std::move(promise));
  m_hasWork.notify_one();

  return future;
}

std::future<void> BlockchainSynchronizer::removeUnconfirmedTransaction(const Crypto::Hash& transactionHash) {
  m_logger(Info) << "Removing unconfirmed transaction, hash " << transactionHash;

  std::unique_lock<std::mutex> lock(m_stateMutex);

  if (m_currentState == State::stopped || m_futureState == State::stopped) {
    auto message = "Failed to remove unconfirmed transaction: not stopped";
    m_logger(Error) << message << ", hash " << transactionHash;
    throw std::runtime_error(message);
  }

  std::promise<void> promise;
  auto future = promise.get_future();
  m_removeTransactionTasks.emplace_back(&transactionHash, std::move(promise));
  m_hasWork.notify_one();

  return future;
}

std::error_code BlockchainSynchronizer::doAddUnconfirmedTransaction(const ITransactionReader& transaction) {
  std::unique_lock<std::mutex> lk(m_consumersMutex);

  std::error_code ec;
  auto addIt = m_consumers.begin();
  for (; addIt != m_consumers.end(); ++addIt) {
    ec = addIt->first->addUnconfirmedTransaction(transaction);
    if (ec) {
      m_logger(Error) << "Failed to add unconfirmed transaction to consumer: " << ec << ", " << ec.message()
                      << ", consumer " << addIt->first << ", hash " << transaction.getTransactionHash();
      break;
    }
  }

  if (ec) {
    auto transactionHash = transaction.getTransactionHash();
    for (auto rollbackIt = m_consumers.begin(); rollbackIt != addIt; ++rollbackIt) {
      rollbackIt->first->removeUnconfirmedTransaction(transactionHash);
    }
  } else {
    m_logger(Info) << "Unconfirmed transaction added, hash " << transaction.getTransactionHash();
  }

  return ec;
}

void BlockchainSynchronizer::doRemoveUnconfirmedTransaction(const Crypto::Hash& transactionHash) {
  std::unique_lock<std::mutex> lk(m_consumersMutex);
  (void)lk;

  for (auto& consumer : m_consumers) {
    consumer.first->removeUnconfirmedTransaction(transactionHash);
  }

  m_logger(Info) << "Unconfirmed transaction removed, hash " << transactionHash;
}

bool BlockchainSynchronizer::save(std::ostream& os) {
  m_logger(Info) << "Saving...";
  os.write(reinterpret_cast<const char*>(&m_genesisBlockHash), sizeof(m_genesisBlockHash));
  m_logger(Info) << "Saved";
  return !os.bad();
}

bool BlockchainSynchronizer::load(std::istream& in) {
  m_logger(Info) << "Loading...";
  Hash genesisBlockHash;
  in.read(reinterpret_cast<char*>(&genesisBlockHash), sizeof(genesisBlockHash));
  if (genesisBlockHash != m_genesisBlockHash) {
    auto message = "Failed to load: genesis block hash does not match stored state";
    m_logger(Error) << message << ", read " << genesisBlockHash << ", expected " << m_genesisBlockHash;
    XI_RETURN_EC(false);
  }

  m_logger(Info) << "Loaded";
  return !in.bad();
}

//--------------------------- FSM ------------------------------------

bool BlockchainSynchronizer::setFutureState(State s) {
  return setFutureStateIf(s, [this, s] { return s > m_futureState; });
}

bool BlockchainSynchronizer::setFutureStateIf(State s, std::function<bool(void)>&& pred) {
  std::unique_lock<std::mutex> lk(m_stateMutex);
  if (pred()) {
    m_futureState = s;
    m_hasWork.notify_one();
    return true;
  }

  return false;
}

void BlockchainSynchronizer::actualizeFutureState() {
  std::unique_lock<std::mutex> lk(m_stateMutex);
  if (m_currentState == State::stopped &&
      (m_futureState == State::deleteOldTxs ||
       m_futureState == State::blockchainSync)) {  // start(), immideately attach observer
    m_node.addObserver(this);
  }

  if (m_futureState == State::stopped && m_currentState != State::stopped) {  // stop(), immideately detach observer
    m_node.removeObserver(this);
  }

  while (!m_removeTransactionTasks.empty()) {
    auto& task = m_removeTransactionTasks.front();
    const Crypto::Hash& transactionHash = *task.first;
    auto detachedPromise = std::move(task.second);
    m_removeTransactionTasks.pop_front();

    try {
      doRemoveUnconfirmedTransaction(transactionHash);
      detachedPromise.set_value();
    } catch (...) {
      m_logger(Error) << "Failed to remove unconfirmed transaction, hash " << transactionHash;
      detachedPromise.set_exception(std::current_exception());
    }
  }

  while (!m_addTransactionTasks.empty()) {
    auto& task = m_addTransactionTasks.front();
    const ITransactionReader& transaction = *task.first;
    auto detachedPromise = std::move(task.second);
    m_addTransactionTasks.pop_front();

    try {
      auto ec = doAddUnconfirmedTransaction(transaction);
      detachedPromise.set_value(ec);
    } catch (...) {
      m_logger(Error) << "Failed to add unconfirmed transaction, hash " << transaction.getTransactionHash();
      detachedPromise.set_exception(std::current_exception());
    }
  }

  m_currentState = m_futureState;
  switch (m_futureState) {
    case State::stopped:
      break;
    case State::deleteOldTxs:
      m_futureState = State::blockchainSync;
      lk.unlock();
      removeOutdatedTransactions();
      break;
    case State::blockchainSync:
      m_futureState = State::poolSync;
      lk.unlock();
      startBlockchainSync();
      break;
    case State::poolSync:
      m_futureState = State::idle;
      lk.unlock();
      startPoolSync();
      break;
    case State::idle:
      m_logger(Debugging) << "Idle";
      m_hasWork.wait(lk, [this] {
        return m_futureState != State::idle || !m_removeTransactionTasks.empty() || !m_addTransactionTasks.empty();
      });
      m_logger(Debugging) << "Resume";
      lk.unlock();
      break;
    default:
      break;
  }
}

bool BlockchainSynchronizer::checkIfShouldStop() const {
  std::unique_lock<std::mutex> lk(m_stateMutex);
  return m_futureState == State::stopped;
}

bool BlockchainSynchronizer::checkIfStopped() const {
  std::unique_lock<std::mutex> lk(m_stateMutex);
  return m_currentState == State::stopped;
}

void BlockchainSynchronizer::workingProcedure() {
  m_logger(Debugging) << "Working thread started";

  while (!checkIfShouldStop()) {
    actualizeFutureState();
  }

  actualizeFutureState();

  m_logger(Debugging) << "Working thread stopped";
}

void BlockchainSynchronizer::start() {
  m_logger(Info) << "Starting...";

  {
    std::unique_lock<std::mutex> lk(m_consumersMutex);
    (void)lk;
    if (m_consumers.empty()) {
      auto message = "Failed to start: no consumers";
      m_logger(Error) << message;
      throw std::runtime_error(message);
    }
  }

  State nextState;
  if (!wasStarted) {
    nextState = State::deleteOldTxs;
    wasStarted = true;
  } else {
    nextState = State::blockchainSync;
  }

  if (!setFutureStateIf(nextState,
                        [this] { return m_currentState == State::stopped && m_futureState == State::stopped; })) {
    auto message = "Failed to start: already started";
    m_logger(Error) << message;
    throw std::runtime_error(message);
  }

  workingThread.reset(new std::thread([this] { workingProcedure(); }));
}

void BlockchainSynchronizer::stop() {
  m_logger(Info) << "Stopping...";
  setFutureState(State::stopped);

  // wait for previous processing to end
  if (workingThread.get() != nullptr && workingThread->joinable()) {
    workingThread->join();
  }

  workingThread.reset();
  m_logger(Info) << "Stopped";
}

void BlockchainSynchronizer::localBlockchainUpdated(BlockHeight height) {
  m_logger(Debugging) << "Event: localBlockchainUpdated " << height.native();
  setFutureState(State::blockchainSync);
}

void BlockchainSynchronizer::lastKnownBlockHeightUpdated(BlockHeight height) {
  m_logger(Debugging) << "Event: lastKnownBlockHeightUpdated " << height.native();
  setFutureState(State::blockchainSync);
}

void BlockchainSynchronizer::poolChanged() {
  m_logger(Debugging) << "Event: poolChanged";
  setFutureState(State::poolSync);
}
//--------------------------- FSM END ------------------------------------

void BlockchainSynchronizer::getPoolUnionAndIntersection(std::unordered_set<Crypto::Hash>& poolUnion,
                                                         std::unordered_set<Crypto::Hash>& poolIntersection) const {
  std::unique_lock<std::mutex> lk(m_consumersMutex);

  auto itConsumers = m_consumers.begin();
  poolUnion = itConsumers->first->getKnownPoolTxIds();
  poolIntersection = itConsumers->first->getKnownPoolTxIds();
  ++itConsumers;

  for (; itConsumers != m_consumers.end(); ++itConsumers) {
    const std::unordered_set<Crypto::Hash>& consumerKnownIds = itConsumers->first->getKnownPoolTxIds();

    poolUnion.insert(consumerKnownIds.begin(), consumerKnownIds.end());

    for (auto itIntersection = poolIntersection.begin(); itIntersection != poolIntersection.end();) {
      if (consumerKnownIds.count(*itIntersection) == 0) {
        itIntersection = poolIntersection.erase(itIntersection);
      } else {
        ++itIntersection;
      }
    }
  }

  m_logger(Debugging) << "Pool union size " << poolUnion.size() << ", intersection size " << poolIntersection.size();
}

BlockchainSynchronizer::GetBlocksRequest BlockchainSynchronizer::getCommonHistory() {
  GetBlocksRequest request;
  std::unique_lock<std::mutex> lk(m_consumersMutex);
  if (m_consumers.empty()) {
    return request;
  }

  auto shortest = m_consumers.begin();
  auto syncStart = shortest->first->getSyncStart();
  auto it = shortest;
  ++it;
  for (; it != m_consumers.end(); ++it) {
    if (it->second->getHeight() < shortest->second->getHeight()) {
      shortest = it;
    }

    auto consumerStart = it->first->getSyncStart();
    syncStart.timestamp = std::min(syncStart.timestamp, consumerStart.timestamp);
    syncStart.height = std::min(syncStart.height, consumerStart.height);
  }

  m_logger(Debugging) << "Shortest chain size " << shortest->second->getHeight().native();

  request.knownBlocks = shortest->second->getShortHistory(m_node.getLastLocalBlockHeight());
  request.syncStart = syncStart;

  m_logger(Debugging) << "Common history: start block height " << request.syncStart.height.native()
                      << ", sparse chain size " << request.knownBlocks.size();

  return request;
}

void BlockchainSynchronizer::startBlockchainSync() {
  m_logger(Debugging) << "Starting blockchain synchronization...";

  GetBlocksResponse response;
  GetBlocksRequest req = getCommonHistory();

  try {
    if (!req.knownBlocks.empty()) {
      auto queryBlocksCompleted = std::promise<std::error_code>();
      auto queryBlocksWaitFuture = queryBlocksCompleted.get_future();

      m_node.queryBlocks(std::move(req.knownBlocks), req.syncStart.timestamp, response.newBlocks, response.startHeight,
                         [&queryBlocksCompleted](std::error_code ec) {
                           auto detachedPromise = std::move(queryBlocksCompleted);
                           detachedPromise.set_value(ec);
                         });

      std::error_code ec = queryBlocksWaitFuture.get();

      if (ec) {
        m_logger(Error) << "Failed to query blocks: " << ec << ", " << ec.message();
        setFutureStateIf(State::idle, [this] { return m_futureState != State::stopped; });
        m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted, ec);
      } else {
        m_logger(Debugging) << "Blocks received, start height " << response.startHeight.native() << ", count "
                            << response.newBlocks.size();
        processBlocks(response);
      }
    }
  } catch (const std::exception& e) {
    m_logger(Error) << "Failed to query and process blocks: " << e.what();
    setFutureStateIf(State::idle, [this] { return m_futureState != State::stopped; });
    m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted,
                             std::make_error_code(std::errc::invalid_argument));
  }
}

void BlockchainSynchronizer::processBlocks(GetBlocksResponse& response) {
  m_logger(Debugging) << "Process blocks, start height " << response.startHeight.native() << ", count "
                      << response.newBlocks.size();

  BlockchainInterval interval;
  interval.startHeight = response.startHeight;
  std::vector<CompleteBlock> blocks;

  for (auto& block : response.newBlocks) {
    if (checkIfShouldStop()) {
      break;
    }

    CompleteBlock completeBlock;
    completeBlock.timestamp = block.timestamp;
    completeBlock.blockHash = block.blockHash;
    if (block.hasBlock) {
      completeBlock.block = std::move(block.block);
      completeBlock.transactions.push_back(createTransactionPrefix(completeBlock.block->baseTransaction));
      if (m_currency.isStaticRewardEnabledForBlockVersion(completeBlock.block->version)) {
        auto staticReward = m_currency.constructStaticRewardTx(*completeBlock.block);
        if (staticReward.isError()) {
          m_logger(Error) << "Failed to construct static reward: " << staticReward.error().message();
          return;
        } else if (!staticReward.value().has_value()) {
          m_logger(Error) << "Expected static reward but none given.";
          return;
        } else {
          completeBlock.transactions.push_back(createTransactionPrefix(*staticReward.value()));
        }
      }

      try {
        for (const auto& txShortInfo : block.txsShortInfo) {
          completeBlock.transactions.push_back(
              createTransactionPrefix(txShortInfo.txPrefix, reinterpret_cast<const Hash&>(txShortInfo.txId)));
        }
      } catch (const std::exception& e) {
        m_logger(Error) << "Failed to process blocks: " << e.what();
        setFutureStateIf(State::idle, [this] { return m_futureState != State::stopped; });
        m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted,
                                 std::make_error_code(std::errc::invalid_argument));
        return;
      }
    }

    interval.blocks.push_back(completeBlock.blockHash);
    blocks.push_back(std::move(completeBlock));
  }

  uint32_t processedBlockCount = response.startHeight.native() + static_cast<uint32_t>(response.newBlocks.size());
  if (!checkIfShouldStop()) {
    response.newBlocks.clear();
    std::unique_lock<std::mutex> lk(m_consumersMutex);
    auto result = updateConsumers(interval, blocks);
    lk.unlock();

    switch (result) {
      case UpdateConsumersResult::errorOccurred:
        if (setFutureStateIf(State::idle, [this] { return m_futureState != State::stopped; })) {
          m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted,
                                   std::make_error_code(std::errc::invalid_argument));
        }
        break;

      case UpdateConsumersResult::nothingChanged:
        if (m_node.getKnownBlockCount() != m_node.getLocalBlockCount()) {
          m_logger(Debugging) << "Blockchain updated, resume blockchain synchronization";
          std::this_thread::sleep_for(std::chrono::milliseconds{50});
        } else {
          break;
        }

        BOOST_FALLTHROUGH;
      case UpdateConsumersResult::addedNewBlocks:
        setFutureState(State::blockchainSync);
        m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationProgressUpdated, processedBlockCount,
                                 std::max(m_node.getKnownBlockCount(), m_node.getLocalBlockCount()));
        break;
    }
  }

  if (checkIfShouldStop()) {
    m_logger(Debugging) << "Block processing is interrupted";
    m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted,
                             std::make_error_code(std::errc::interrupted));
  }
}

/// \pre m_consumersMutex is locked
BlockchainSynchronizer::UpdateConsumersResult BlockchainSynchronizer::updateConsumers(
    const BlockchainInterval& interval, const std::vector<CompleteBlock>& blocks) {
  assert(interval.blocks.size() == blocks.size());

  bool smthChanged = false;
  bool hasErrors = false;

  BlockHeight lastBlockHeight = BlockHeight::Null;
  for (auto& kv : m_consumers) {
    auto result = kv.second->checkInterval(interval);

    if (result.detachRequired) {
      m_logger(Debugging) << "Detach consumer, consumer " << kv.first << ", block height "
                          << toString(result.detachHeight);
      kv.first->onBlockchainDetach(result.detachHeight);
      kv.second->detach(result.detachHeight);
    }

    if (result.hasNewBlocks) {
      auto startOffset = result.newBlockHeight - interval.startHeight;
      auto blockCount = BlockHeight::fromSize(blocks.size()) - startOffset;
      // update consumer
      m_logger(Debugging) << "Adding blocks to consumer, consumer " << kv.first << ", start height "
                          << toString(result.newBlockHeight) << ", count " << blockCount.native();
      uint32_t addedCount =
          kv.first->onNewBlocks(blocks.data() + startOffset.native(), result.newBlockHeight, blockCount.native());
      if (addedCount > 0) {
        if (addedCount < blockCount.native()) {
          m_logger(Error) << "Failed to add " << (blockCount.native() - addedCount) << " blocks of "
                          << blockCount.native() << " to consumer, consumer " << kv.first;
          hasErrors = true;
        }

        // update state if consumer succeeded
        kv.second->addBlocks(interval.blocks.data() + startOffset.native(), result.newBlockHeight, addedCount);
        smthChanged = true;
      } else {
        m_logger(Error) << "Failed to add blocks to consumer, consumer " << kv.first;
        hasErrors = true;
      }

      if (addedCount > 0) {
        lastBlockHeight = std::max(lastBlockHeight, BlockHeight::fromSize(addedCount) + startOffset);
      }
    }
  }

  if (!lastBlockHeight.isNull()) {
    assert(lastBlockHeight.native() <= blocks.size());
    m_lastBlockId = blocks[lastBlockHeight.toIndex()].blockHash;
  }

  if (hasErrors) {
    m_logger(Debugging) << "Not all blocks were added to consumers, there were errors";
    return UpdateConsumersResult::errorOccurred;
  } else if (smthChanged) {
    m_logger(Debugging) << "Blocks added to consumers";
    return UpdateConsumersResult::addedNewBlocks;
  } else {
    m_logger(Debugging) << "No new blocks received. Consumers not updated";
    return UpdateConsumersResult::nothingChanged;
  }
}

void BlockchainSynchronizer::removeOutdatedTransactions() {
  m_logger(Info) << "Removing outdated pool transactions...";

  std::unordered_set<Crypto::Hash> unionPoolHistory;
  std::unordered_set<Crypto::Hash> ignored;
  getPoolUnionAndIntersection(unionPoolHistory, ignored);

  GetPoolRequest request;
  request.knownTxIds.assign(unionPoolHistory.begin(), unionPoolHistory.end());
  request.lastKnownBlock = m_lastBlockId;

  GetPoolResponse response;
  response.isLastKnownBlockActual = false;

  std::error_code ec = getPoolSymmetricDifferenceSync(std::move(request), response);

  if (!ec) {
    m_logger(Debugging) << "Outdated pool transactions received, " << response.deletedTxIds.size() << ':'
                        << makeContainerFormatter(response.deletedTxIds);

    std::unique_lock<std::mutex> lock(m_consumersMutex);
    for (auto& consumer : m_consumers) {
      ec = consumer.first->onPoolUpdated({}, response.deletedTxIds);
      if (ec) {
        m_logger(Error) << "Failed to process outdated pool transactions: " << ec << ", " << ec.message()
                        << ", consumer " << consumer.first;
        break;
      }
    }
  } else {
    m_logger(Debugging, BRIGHT_RED) << "Failed to query outdated pool transaction: " << ec << ", " << ec.message();
  }

  if (!ec) {
    m_logger(Info) << "Outdated pool transactions processed";
  } else {
    m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted, ec);

    m_logger(Info) << "Retry in " << RETRY_TIMEOUT << " seconds...";
    std::unique_lock<std::mutex> lock(m_stateMutex);
    bool stopped = m_hasWork.wait_for(lock, std::chrono::seconds(RETRY_TIMEOUT),
                                      [this] { return m_futureState == State::stopped; });

    if (!stopped) {
      m_futureState = State::deleteOldTxs;
    }
  }
}

void BlockchainSynchronizer::startPoolSync() {
  m_logger(Debugging) << "Starting pool synchronization...";

  std::unordered_set<Crypto::Hash> unionPoolHistory;
  std::unordered_set<Crypto::Hash> intersectedPoolHistory;
  getPoolUnionAndIntersection(unionPoolHistory, intersectedPoolHistory);

  GetPoolRequest unionRequest;
  unionRequest.knownTxIds.assign(unionPoolHistory.begin(), unionPoolHistory.end());
  unionRequest.lastKnownBlock = m_lastBlockId;

  GetPoolResponse unionResponse;
  unionResponse.isLastKnownBlockActual = false;

  std::error_code ec = getPoolSymmetricDifferenceSync(std::move(unionRequest), unionResponse);

  if (ec) {
    m_logger(Error) << "Failed to query transaction pool changes: " << ec << ", " << ec.message();
    setFutureStateIf(State::idle, [this] { return m_futureState != State::stopped; });
    m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted, ec);
  } else {                                        // get union ok
    if (!unionResponse.isLastKnownBlockActual) {  // bc outdated
      m_logger(Debugging) << "Transaction pool changes received, but blockchain has been changed";
      setFutureState(State::blockchainSync);
    } else {
      m_logger(Debugging) << "Transaction pool changes received, added " << unionResponse.newTxs.size() << ", deleted "
                          << unionResponse.deletedTxIds.size();

      if (unionPoolHistory == intersectedPoolHistory) {  // usual case, start pool processing
        m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted,
                                 processPoolTxs(unionResponse));
      } else {
        GetPoolRequest intersectionRequest;
        intersectionRequest.knownTxIds.assign(intersectedPoolHistory.begin(), intersectedPoolHistory.end());
        intersectionRequest.lastKnownBlock = m_lastBlockId;

        GetPoolResponse intersectionResponse;
        intersectionResponse.isLastKnownBlockActual = false;

        std::error_code ec2 = getPoolSymmetricDifferenceSync(std::move(intersectionRequest), intersectionResponse);

        if (ec2) {
          m_logger(Error) << "Failed to query transaction pool changes, stage 2: " << ec << ", " << ec.message();
          setFutureStateIf(State::idle, [this] { return m_futureState != State::stopped; });
          m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted, ec2);
        } else {                                               // get intersection ok
          if (!intersectionResponse.isLastKnownBlockActual) {  // bc outdated
            m_logger(Debugging) << "Transaction pool changes at stage 2 received, but blockchain has been changed";
            setFutureState(State::blockchainSync);
          } else {
            m_logger(Debugging) << "Transaction pool changes at stage 2 received, added "
                                << intersectionResponse.newTxs.size() << ", deleted "
                                << intersectionResponse.deletedTxIds.size();
            intersectionResponse.deletedTxIds.assign(unionResponse.deletedTxIds.begin(),
                                                     unionResponse.deletedTxIds.end());
            std::error_code ec3 = processPoolTxs(intersectionResponse);

            // notify about error, or success
            m_observerManager.notify(&IBlockchainSynchronizerObserver::synchronizationCompleted, ec3);
          }
        }
      }
    }
  }
}

std::error_code BlockchainSynchronizer::getPoolSymmetricDifferenceSync(GetPoolRequest&& request,
                                                                       GetPoolResponse& response) {
  auto promise = std::promise<std::error_code>();
  auto future = promise.get_future();

  m_node.getPoolSymmetricDifference(std::move(request.knownTxIds), std::move(request.lastKnownBlock),
                                    response.isLastKnownBlockActual, response.newTxs, response.deletedTxIds,
                                    [&promise](std::error_code ec) {
                                      auto detachedPromise = std::move(promise);
                                      detachedPromise.set_value(ec);
                                    });

  return future.get();
}

std::error_code BlockchainSynchronizer::processPoolTxs(GetPoolResponse& response) {
  m_logger(Debugging) << "Starting to process pool transactions, added " << response.newTxs.size() << ':'
                      << TransactionReaderListFormatter(response.newTxs) << ", deleted " << response.deletedTxIds.size()
                      << ':' << Common::makeContainerFormatter(response.deletedTxIds);

  std::error_code error;
  {
    std::unique_lock<std::mutex> lk(m_consumersMutex);
    for (auto& consumer : m_consumers) {
      if (checkIfShouldStop()) {  // if stop, return immediately, without notification
        m_logger(Warning) << "Pool transactions processing is interrupted";
        return std::make_error_code(std::errc::interrupted);
      }

      error = consumer.first->onPoolUpdated(response.newTxs, response.deletedTxIds);
      if (error) {
        m_logger(Error) << "Failed to process pool transactions: " << error << ", " << error.message() << ", consumer "
                        << consumer.first;
        break;
      }
    }
  }

  if (!error) {
    m_logger(Debugging) << "Pool changes processed";
  }

  return error;
}

/// pre: m_consumersMutex is locked
SynchronizationState* BlockchainSynchronizer::getConsumerSynchronizationState(IBlockchainConsumer* consumer) const {
  assert(consumer != nullptr);

  if (!(checkIfStopped() && checkIfShouldStop())) {
    auto message = "Failed to get consumer state: not stopped";
    m_logger(Error) << message << ", consumer " << consumer;
    throw std::runtime_error(message);
  }

  auto it = m_consumers.find(consumer);
  if (it == m_consumers.end()) {
    return nullptr;
  }

  return it->second.get();
}

}  // namespace CryptoNote
