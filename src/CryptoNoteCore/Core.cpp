// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include <algorithm>
#include <numeric>
#include <set>
#include <unordered_set>

#include <Xi/Global.hh>
#include <Xi/Exceptions.hpp>
#include <Xi/Algorithm/IsUnique.h>

#include "Core.h"
#include "Common/ShuffleGenerator.h"
#include "Common/Math.h"
#include "Common/MemoryInputStream.h"
#include "CryptoNoteTools.h"
#include "CryptoNoteFormatUtils.h"
#include "BlockchainCache.h"
#include "BlockchainStorage.h"
#include "BlockchainUtils.h"
#include "CryptoNoteCore/CoreErrors.h"
#include "CryptoNoteCore/MemoryBlockchainStorage.h"
#include "CryptoNoteCore/Time/LocalTimeProvider.h"
#include "CryptoNoteCore/Transactions/Validation.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"
#include "CryptoNoteCore/Transactions/TransactionPool.h"
#include "CryptoNoteCore/Transactions/TransactionPoolCleaner.h"
#include "CryptoNoteCore/Transactions/TransactionApi.h"
#include "CryptoNoteCore/UpgradeManager.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandlerCommon.h"

#include <System/Timer.h>

using namespace Crypto;

namespace CryptoNote {

namespace {

template <class T>
std::vector<T> preallocateVector(size_t elements) {
  std::vector<T> vect;
  vect.reserve(elements);
  return vect;
}
UseGenesis addGenesisBlock = UseGenesis(true);

inline IBlockchainCache* findIndexInChain(IBlockchainCache* blockSegment, const Crypto::Hash& blockHash) {
  assert(blockSegment != nullptr);
  while (blockSegment != nullptr) {
    if (blockSegment->hasBlock(blockHash)) {
      return blockSegment;
    }

    blockSegment = blockSegment->getParent();
  }

  return nullptr;
}

inline IBlockchainCache* findIndexInChain(IBlockchainCache* blockSegment, uint32_t blockIndex) {
  assert(blockSegment != nullptr);
  while (blockSegment != nullptr) {
    if (blockIndex >= blockSegment->getStartBlockIndex() &&
        blockIndex < blockSegment->getStartBlockIndex() + blockSegment->getBlockCount()) {
      return blockSegment;
    }

    blockSegment = blockSegment->getParent();
  }

  return nullptr;
}

BlockTemplate extractBlockTemplate(const RawBlock& block) {
  BlockTemplate blockTemplate;
  if (!fromBinaryArray(blockTemplate, block.blockTemplate)) {
    throw std::system_error(make_error_code(error::AddBlockErrorCode::DESERIALIZATION_FAILED));
  }

  return blockTemplate;
}

Crypto::Hash getBlockHash(const RawBlock& block) {
  BlockTemplate blockTemplate = extractBlockTemplate(block);
  return CachedBlock(blockTemplate).getBlockHash();
}

TransactionValidatorState extractSpentOutputs(const CachedTransaction& transaction) {
  TransactionValidatorState spentOutputs;
  const auto& cryptonoteTransaction = transaction.getTransaction();

  for (const auto& input : cryptonoteTransaction.inputs) {
    if (auto keyInput = std::get_if<KeyInput>(&input)) {
      bool r = spentOutputs.spentKeyImages.insert(keyInput->keyImage).second;
      if (r) {
      }
      assert(r);
    } else {
      assert(false);
    }
  }

  return spentOutputs;
}

TransactionValidatorState extractSpentOutputs(const std::vector<CachedTransaction>& transactions) {
  TransactionValidatorState resultOutputs;
  for (const auto& transaction : transactions) {
    auto transactionOutputs = extractSpentOutputs(transaction);
    mergeStates(resultOutputs, transactionOutputs);
  }

  return resultOutputs;
}

int64_t getEmissionChange(const Currency& currency, IBlockchainCache& segment, uint32_t previousBlockIndex,
                          const CachedBlock& cachedBlock, uint64_t cumulativeSize, uint64_t cumulativeFee) {
  uint64_t reward = 0;
  int64_t emissionChange = 0;
  const auto version = cachedBlock.getBlock().version;
  auto alreadyGeneratedCoins = segment.getAlreadyGeneratedCoins(previousBlockIndex);
  auto lastBlocksSizes = segment.getLastBlocksSizes(currency.rewardBlocksWindowByBlockVersion(version),
                                                    previousBlockIndex, UseGenesis{false});
  auto blocksSizeMedian = Common::medianValue(lastBlocksSizes);
  if (!currency.getBlockReward(version, blocksSizeMedian, cumulativeSize, alreadyGeneratedCoins, cumulativeFee, reward,
                               emissionChange)) {
    throw std::system_error(make_error_code(error::BlockValidationError::CUMULATIVE_BLOCK_SIZE_TOO_BIG));
  }

  return emissionChange;
}

uint32_t findCommonRoot(IMainChainStorage& storage, IBlockchainCache& rootSegment) {
  assert(storage.getBlockCount());
  assert(rootSegment.getBlockCount());
  assert(rootSegment.getStartBlockIndex() == 0);
  assert(getBlockHash(storage.getBlockByIndex(0)) == rootSegment.getBlockHash(0));

  const auto hash_equal = [&](uint32_t index) {
    return getBlockHash(storage.getBlockByIndex(index)) == rootSegment.getBlockHash(index);
  };

  uint32_t left = 0;
  uint32_t right = std::min(storage.getBlockCount() - 1, rootSegment.getBlockCount() - 1);

  if (hash_equal(right)) {
    return right;
  }

  while (left != right) {
    assert(right >= left);
    uint32_t checkElement = left + (right - left) / 2 + 1;
    if (hash_equal(checkElement)) {
      left = checkElement;
    } else {
      right = checkElement - 1;
    }
  }

  return left;
}

const std::chrono::seconds OUTDATED_TRANSACTION_POLLING_INTERVAL = std::chrono::minutes(10);

}  // namespace

Core::Core(const Currency& currency, Logging::ILogger& logger, Checkpoints& checkpoints, System::Dispatcher& dispatcher,
           bool isLightNode, std::unique_ptr<IBlockchainCacheFactory>&& blockchainCacheFactory,
           std::unique_ptr<IMainChainStorage>&& mainchainStorage)
    : m_currency(currency),
      dispatcher(dispatcher),
      contextGroup(dispatcher),
      logger(logger, "Core"),
      checkpoints(checkpoints),
      m_upgradeManager(new UpgradeManager()),
      m_isLightNode{isLightNode},
      blockchainCacheFactory(std::move(blockchainCacheFactory)),
      mainChainStorage(std::move(mainchainStorage)),
      initialized(false),
      m_access{} {
  for (auto version : Xi::Config::BlockVersion::versions())
    m_upgradeManager->addBlockVersion(version, currency.upgradeHeight(version));
  m_transactionPool = std::make_unique<TransactionPoolCleanWrapper>(
      std::unique_ptr<ITransactionPool>(new TransactionPool(*this, logger)),
      std::unique_ptr<ITimeProvider>(new LocalRealTimeProvider()), logger, currency.mempoolTxLiveTime());
  m_transactionPool->addObserver(this);
  m_timeProvider = std::make_unique<LocalRealTimeProvider>();
}

Core::~Core() {
  contextGroup.interrupt();
  contextGroup.wait();
}

Xi::Concurrent::RecursiveLock::lock_t Core::lock() const { return Xi::Concurrent::RecursiveLock::lock_t{m_access}; }

bool Core::addMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) { return queueList.insert(messageQueue); }

bool Core::removeMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) { return queueList.remove(messageQueue); }

void Core::addObserver(IBlockchainObserver* observer) { m_blockchainObservers.add(observer); }
void Core::removeObserver(IBlockchainObserver* observer) { m_blockchainObservers.remove(observer); }

const Currency& Core::currency() const { return getCurrency(); }

const IBlockchainCache* Core::mainChain() const {
  if (!initialized || chainsLeaves.empty())
    return nullptr;
  else
    return chainsLeaves[0];
}

const IUpgradeManager& Core::upgradeManager() const { return *m_upgradeManager; }
const ITimeProvider& Core::timeProvider() const { return *m_timeProvider; }
bool Core::isInitialized() const { return initialized; }

void Core::transactionDeletedFromPool(const Hash& hash, ITransactionPoolObserver::DeletionReason reason) {
  using Reason = ITransactionPoolObserver::DeletionReason;
  switch (reason) {
    case Reason::KeyImageUsedInMainChain:
    case Reason::PoolCleanupProcedure:
    case Reason::BlockVersionUpgrade:
    case Reason::Forced:
      notifyObservers(makeDelTransactionMessage({hash}, Messages::DeleteTransaction::Reason::NotActual));
      break;
    case Reason::AddedToMainChain:
      /* swallow handled by addBlockRoutine */
      break;
    case Reason::SkipNotification:
      /* swallow, we should not receive it in first place */
      break;
  }
}

void Core::transactionAddedToPool(const Hash& hash, ITransactionPoolObserver::AdditionReason reason) {
  using Reason = ITransactionPoolObserver::AdditionReason;
  switch (reason) {
    case Reason::Incoming:
      notifyObservers(makeAddTransactionMessage({hash}));
      break;
    case Reason::Deserialization:
    case Reason::MainChainSwitch:
      /* swallow no appropriate blockchain event */
      break;
    case Reason::SkipNotification:
      /* swallow, we should not receive it in first place */
      break;
  }
}

bool Core::notifyObservers(BlockchainMessage&& msg) /* noexcept */ {
  try {
    for (auto& queue : queueList) {
      queue.push(std::move(msg));
    }
    return true;
  } catch (std::exception& e) {
    logger(Logging::WARNING) << "failed to notify observers: " << e.what();
    return false;
  }
}

uint32_t Core::getTopBlockIndex() const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  return chainsLeaves[0]->getTopBlockIndex();
}

Crypto::Hash Core::getTopBlockHash() const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  return chainsLeaves[0]->getTopBlockHash();
}

BlockVersion Core::getTopBlockVersion() const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  return chainsLeaves[0]->getTopBlockVersion();
}

Crypto::Hash Core::getBlockHashByIndex(uint32_t blockIndex) const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());
  assert(blockIndex <= getTopBlockIndex());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  return chainsLeaves[0]->getBlockHash(blockIndex);
}

uint64_t Core::getBlockTimestampByIndex(uint32_t blockIndex) const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());
  assert(blockIndex <= getTopBlockIndex());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  auto timestamps = chainsLeaves[0]->getLastTimestamps(1, blockIndex, addGenesisBlock);
  assert(timestamps.size() == 1);

  return timestamps[0];
}

std::optional<BlockSource> Core::hasBlock(const Crypto::Hash& blockHash) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  bool isMainChain = false;
  if (findSegmentContainingBlock(blockHash, &isMainChain) == nullptr) {
    return std::nullopt;
  } else {
    return isMainChain ? BlockSource::MainChain : BlockSource::AlternativeChain;
  }
}

std::optional<CoreBlockInfo> Core::getBlockByIndex(uint32_t index) const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());
  assert(index <= getTopBlockIndex());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  IBlockchainCache* segment = findMainChainSegmentContainingBlock(index);
  assert(segment != nullptr);

  if (segment == nullptr) {
    return std::nullopt;
  } else {
    return std::make_optional<CoreBlockInfo>(BlockSource::MainChain, index, segment->getBlockByIndex(index));
  }
}

std::optional<CoreBlockInfo> Core::getBlockByHash(const Crypto::Hash& blockHash) const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  bool isMainChain = false;
  IBlockchainCache* segment = findSegmentContainingBlock(blockHash, &isMainChain);
  if (segment == nullptr) {
    return std::nullopt;
  } else {
    auto source = isMainChain ? BlockSource::MainChain : BlockSource::AlternativeChain;
    const uint32_t index = segment->getBlockIndex(blockHash);
    return std::make_optional<CoreBlockInfo>(source, index, segment->getBlockByIndex(index));
  }
}

std::vector<Crypto::Hash> Core::buildSparseChain() const {
  throwIfNotInitialized();
  Crypto::Hash topBlockHash = chainsLeaves[0]->getTopBlockHash();
  return doBuildSparseChain(topBlockHash);
}

std::vector<RawBlock> Core::getBlocks(uint32_t minIndex, uint32_t count) const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  std::vector<RawBlock> blocks;
  if (count > 0) {
    auto cache = chainsLeaves[0];
    auto maxIndex = std::min(minIndex + count - 1, cache->getTopBlockIndex());
    blocks.reserve(count);
    while (cache) {
      if (cache->getTopBlockIndex() >= maxIndex) {
        auto minChainIndex = std::max(minIndex, cache->getStartBlockIndex());
        for (; minChainIndex <= maxIndex; --maxIndex) {
          blocks.emplace_back(cache->getBlockByIndex(maxIndex));
          if (maxIndex == 0) {
            break;
          }
        }
      }

      if (blocks.size() == count) {
        break;
      }

      cache = cache->getParent();
    }
  }
  std::reverse(blocks.begin(), blocks.end());

  return blocks;
}

void Core::getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<RawBlock>& blocks,
                     std::vector<Crypto::Hash>& missedHashes) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  blocks.reserve(blockHashes.size());
  for (const auto& hash : blockHashes) {
    IBlockchainCache* blockchainSegment = findSegmentContainingBlock(hash);
    if (blockchainSegment == nullptr) {
      missedHashes.push_back(hash);
    } else {
      uint32_t blockIndex = blockchainSegment->getBlockIndex(hash);
      assert(blockIndex <= blockchainSegment->getTopBlockIndex());
      assert(hash == blockchainSegment->getBlockHash(blockIndex));

      blocks.push_back(blockchainSegment->getBlockByIndex(blockIndex));
    }
  }
}

bool Core::queryBlocks(const std::vector<Crypto::Hash>& blockHashes, uint64_t timestamp, uint32_t& startIndex,
                       uint32_t& currentIndex, uint32_t& fullOffset, std::vector<BlockFullInfo>& entries) const {
  assert(entries.empty());
  assert(!chainsLeaves.empty());
  assert(!chainsStorage.empty());
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  try {
    IBlockchainCache* mainChain = chainsLeaves[0];
    currentIndex = mainChain->getTopBlockIndex();

    auto startIndexResult = findBlockchainSupplement(blockHashes);
    if (startIndexResult.isError()) return false;
    startIndex = startIndexResult.value();

    fullOffset = mainChain->getTimestampLowerBoundBlockIndex(timestamp);
    if (fullOffset < startIndex) {
      fullOffset = startIndex;
    }

    size_t hashesPushed = pushBlockHashes(startIndex, fullOffset,
                                          Xi::Config::Network::blockIdentifiersSynchronizationBatchSize(), entries);

    if (startIndex + hashesPushed != fullOffset) {
      return true;
    }

    fillQueryBlockFullInfo(fullOffset, currentIndex, Xi::Config::Network::blocksSynchronizationBatchSize(), entries);

    return true;
  } catch (std::exception&) {
    // TODO log
    return false;
  }
}

bool Core::queryBlocksLite(const std::vector<Crypto::Hash>& knownBlockHashes, uint64_t timestamp, uint32_t& startIndex,
                           uint32_t& currentIndex, uint32_t& fullOffset, std::vector<BlockShortInfo>& entries) const {
  assert(entries.empty());
  assert(!chainsLeaves.empty());
  assert(!chainsStorage.empty());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  try {
    IBlockchainCache* mainChain = chainsLeaves[0];
    currentIndex = mainChain->getTopBlockIndex();

    auto startIndexResult = findBlockchainSupplement(knownBlockHashes);
    if (startIndexResult.isError()) return false;
    startIndex = startIndexResult.value();

    // Stops bug where wallets fail to sync, because timestamps have been adjusted after syncronisation.
    // check for a query of the blocks where the block index is non-zero, but the timestamp is zero
    // indicating that the originator did not know the internal time of the block, but knew which block
    // was wanted by index.  Fullfill this by getting the time of m_blocks[startIndex].timestamp.

    if (startIndex > 0 && timestamp == 0) {
      if (startIndex <= mainChain->getTopBlockIndex()) {
        RawBlock block = mainChain->getBlockByIndex(startIndex);
        auto blockTemplate = extractBlockTemplate(block);
        timestamp = blockTemplate.timestamp;
      }
    }

    fullOffset = mainChain->getTimestampLowerBoundBlockIndex(timestamp);
    if (fullOffset < startIndex) {
      fullOffset = startIndex;
    }

    size_t hashesPushed = pushBlockHashes(startIndex, fullOffset,
                                          Xi::Config::Network::blockIdentifiersSynchronizationBatchSize(), entries);

    if (startIndex + static_cast<uint32_t>(hashesPushed) != fullOffset) {
      return true;
    }

    fillQueryBlockShortInfo(fullOffset, currentIndex, Xi::Config::Network::blocksSynchronizationBatchSize(), entries);

    return true;
  } catch (std::exception& e) {
    logger(Logging::ERROR) << "Failed to query blocks: " << e.what();
    return false;
  }
}

bool Core::queryBlocksDetailed(const std::vector<Crypto::Hash>& knownBlockHashes, uint64_t timestamp,
                               uint32_t& startIndex, uint32_t& currentIndex, uint32_t& fullOffset,
                               std::vector<BlockDetails>& entries) const {
  assert(entries.empty());
  assert(!chainsLeaves.empty());
  assert(!chainsStorage.empty());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  try {
    IBlockchainCache* mainChain = chainsLeaves[0];
    currentIndex = mainChain->getTopBlockIndex();

    auto startIndexResult = findBlockchainSupplement(knownBlockHashes);
    if (startIndexResult.isError()) return false;
    startIndex = startIndexResult.value();

    // Stops bug where wallets fail to sync, because timestamps have been adjusted after syncronisation.
    // check for a query of the blocks where the block index is non-zero, but the timestamp is zero
    // indicating that the originator did not know the internal time of the block, but knew which block
    // was wanted by index.  Fullfill this by getting the time of m_blocks[startIndex].timestamp.

    if (startIndex > 0 && timestamp == 0) {
      if (startIndex <= mainChain->getTopBlockIndex()) {
        RawBlock block = mainChain->getBlockByIndex(startIndex);
        auto blockTemplate = extractBlockTemplate(block);
        timestamp = blockTemplate.timestamp;
      }
    }

    fullOffset = mainChain->getTimestampLowerBoundBlockIndex(timestamp);

    if (fullOffset < startIndex) {
      fullOffset = startIndex;
    }

    size_t hashesPushed = pushBlockHashes(startIndex, fullOffset,
                                          Xi::Config::Network::blockIdentifiersSynchronizationBatchSize(), entries);

    if (startIndex + static_cast<uint32_t>(hashesPushed) != fullOffset) {
      return true;
    }

    if (!fillQueryBlockDetails(fullOffset, currentIndex, Xi::Config::Network::blocksSynchronizationBatchSize(),
                               entries)) {
      return false;
    }

    return true;
  } catch (std::exception& e) {
    logger(Logging::ERROR) << "Failed to query blocks: " << e.what();
    return false;
  }
}

void Core::getTransactions(const std::vector<Crypto::Hash>& transactionHashes, std::vector<BinaryArray>& transactions,
                           std::vector<Crypto::Hash>& missedHashes, bool poolOnly) const {
  XI_CONCURRENT_RLOCK(m_access);

  assert(!chainsLeaves.empty());
  assert(!chainsStorage.empty());
  throwIfNotInitialized();

  std::map<Crypto::Hash, BinaryArray> found{};

  if (!poolOnly) {
    auto segments = queryTransactionSegments(transactionHashes);
    for (const auto& segment : segments) {
      const auto txs = segment.segment->getTransactions(segment.content);
      for (const auto& tx : txs) {
        found[tx.getTransactionHash()] = tx.getTransactionBinaryArray();
      }
    }
  }

  {
    const auto& pool = transactionPool();
    XI_UNUSED_REVAL(pool.acquireExclusiveAccess());
    for (const auto& hash : transactionHashes) {
      if (found.find(hash) != found.end()) {
        continue;
      }
      if (pool.containsTransaction(hash)) {
        const auto tx = pool.queryTransaction(hash);
        found[tx->transaction().getTransactionHash()] = tx->transaction().getTransactionBinaryArray();
      }
    }
  }

  for (const auto& hash : transactionHashes) {
    if (const auto search = found.find(hash); search != found.end()) {
      transactions.emplace_back(std::move(search->second));
    } else {
      missedHashes.push_back(hash);
    }
  }
}

uint64_t Core::getBlockDifficulty(uint32_t blockIndex) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  IBlockchainCache* mainChain = chainsLeaves[0];
  auto difficulties = mainChain->getLastCumulativeDifficulties(2, blockIndex, addGenesisBlock);
  if (difficulties.size() == 2) {
    return difficulties[1] - difficulties[0];
  }

  assert(difficulties.size() == 1);
  return difficulties[0];
}

uint64_t Core::getDifficultyForNextBlock() const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  const IBlockchainCache* mainChain = chainsLeaves[0];
  uint32_t topBlockIndex = mainChain->getTopBlockIndex();
  auto nextBlockVersion = getBlockVersionForIndex(topBlockIndex + 1);
  return mainChain->getDifficultyForNextBlock(nextBlockVersion, topBlockIndex);
}

Xi::Result<std::vector<Crypto::Hash>> Core::findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds,
                                                                     size_t maxCount, uint32_t& totalBlockCount,
                                                                     uint32_t& startBlockIndex) const {
  XI_ERROR_TRY();
  assert(!remoteBlockIds.empty());
  assert(remoteBlockIds.back() == getBlockHashByIndex(0));
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  totalBlockCount = getTopBlockIndex() + 1;
  startBlockIndex = (uint32_t)-1;
  auto startIndexResult = findBlockchainSupplement(remoteBlockIds);
  if (startIndexResult.isError()) return startIndexResult.error();
  startBlockIndex = startIndexResult.value();

  return success(getBlockHashes(startBlockIndex, static_cast<uint32_t>(maxCount)));
  XI_ERROR_CATCH();
}

std::error_code Core::addBlock(const CachedBlock& cachedBlock, RawBlock&& rawBlock) {
  throwIfNotInitialized();

  XI_CONCURRENT_RLOCK(m_access);
  XI_UNUSED_REVAL(transactionPool().acquireExclusiveAccess());

  uint32_t blockIndex = cachedBlock.getBlockIndex();
  Crypto::Hash blockHash = cachedBlock.getBlockHash();
  std::ostringstream os;
  os << blockIndex << " (" << blockHash << ")";
  std::string blockStr = os.str();

  logger(Logging::DEBUGGING) << "Request to add block " << blockStr;
  if (cachedBlock.getBlockIndex() == 0) {
    logger(Logging::DEBUGGING) << "Block index=" << cachedBlock.getBlockIndex() << " is genesis block index";
    return error::AddBlockErrorCode::REJECTED_AS_ORPHANED;
  }
  const auto& blockTemplate = cachedBlock.getBlock();
  const auto& previousBlockHash = blockTemplate.previousBlockHash;
  auto cache = findSegmentContainingBlock(previousBlockHash);
  if (cache == nullptr) {
    logger(Logging::DEBUGGING) << "Block " << blockStr << " rejected as orphaned";
    return error::AddBlockErrorCode::REJECTED_AS_ORPHANED;
  }

  if (hasBlock(cachedBlock.getBlockHash())) {
    logger(Logging::DEBUGGING) << "Block " << blockStr << " already exists";
    return error::AddBlockErrorCode::ALREADY_EXISTS;
  }

  if (rawBlock.transactions.size() != blockTemplate.transactionHashes.size()) {
    return error::BlockValidationError::TRANSACTION_INCONSISTENCY;
  }

  std::vector<CachedTransaction> transactions;
  uint64_t cumulativeSize = 0;
  if (!extractTransactions(rawBlock.transactions, transactions, cumulativeSize, blockTemplate.version)) {
    logger(Logging::DEBUGGING) << "Couldn't deserialize raw block transactions in block " << blockStr;
    return error::AddBlockErrorCode::DESERIALIZATION_FAILED;
  }

  for (uint64_t i = 0; i < transactions.size(); ++i) {
    if (blockTemplate.transactionHashes[i] != transactions[i].getTransactionHash()) {
      logger(Logging::DEBUGGING)
          << "BlockTemplate and raw transactions are inconsisten, may out of sync or wrongly ordered";
      return error::BlockValidationError::TRANSACTION_INCONSISTENCY;
    }
  }

  auto coinbaseTransactionSize = blockTemplate.baseTransaction.binarySize();
  assert(coinbaseTransactionSize < std::numeric_limits<decltype(coinbaseTransactionSize)>::max());
  auto cumulativeBlockSize = coinbaseTransactionSize + cumulativeSize;

  auto previousBlockIndex = cache->getBlockIndex(previousBlockHash);

  bool addOnTop = cache->getTopBlockIndex() == previousBlockIndex;
  auto maxBlockCumulativeSize = m_currency.maxBlockCumulativeSize(previousBlockIndex + 1);
  if (cumulativeBlockSize > maxBlockCumulativeSize) {
    logger(Logging::DEBUGGING) << "Block " << blockStr << " has too big cumulative size";
    return error::BlockValidationError::CUMULATIVE_BLOCK_SIZE_TOO_BIG;
  }

  uint64_t minerReward = 0;
  auto blockValidationResult = validateBlock(cachedBlock, cache, minerReward);
  if (blockValidationResult) {
    logger(Logging::DEBUGGING) << "Failed to validate block " << blockStr << ": " << blockValidationResult.message();
    return blockValidationResult;
  }

  auto currentDifficulty = cache->getDifficultyForNextBlock(blockTemplate.version, previousBlockIndex);
  if (currentDifficulty == 0) {
    logger(Logging::DEBUGGING) << "Block " << blockStr << " has difficulty overhead";
    return error::BlockValidationError::DIFFICULTY_OVERHEAD;
  }

  if (checkpoints.isInCheckpointZone(cachedBlock.getBlockIndex())) {
    if (!checkpoints.checkBlock(cachedBlock.getBlockIndex(), cachedBlock.getBlockHash())) {
      logger(Logging::WARNING) << "Checkpoint block hash mismatch for block " << blockStr;
      return error::BlockValidationError::CHECKPOINT_BLOCK_HASH_MISMATCH;
    }
  } else {
    using BlockError = error::BlockValidationError;
    const auto& block = cachedBlock.getBlock();
    if (!std::holds_alternative<NoMergeMiningTag>(block.mergeMiningTag)) {
      if (const auto mmt = std::get_if<MergeMiningTag>(std::addressof(block.mergeMiningTag))) {
        const auto maxMergeMiningSize = currency().maximumMergeMiningSize(block.version);
        XI_RETURN_EC_IF(maxMergeMiningSize == 0, BlockError::MERGE_MINING_TAG_DISABLED);
        XI_RETURN_EC_IF(mmt->prefix.size() > maxMergeMiningSize, BlockError::MERGE_MINING_TAG_TOO_LARGE);
        XI_RETURN_EC_IF(mmt->postfix.size() > maxMergeMiningSize, BlockError::MERGE_MINING_TAG_TOO_LARGE);
        const auto mergeMiningSize = mmt->size();
        XI_RETURN_EC_IF(mergeMiningSize == 0, BlockError::MERGE_MINING_TAG_EMPTY);
        XI_RETURN_EC_IF(mergeMiningSize > maxMergeMiningSize, BlockError::MERGE_MINING_TAG_TOO_LARGE);
      } else if (const auto pmmt = std::get_if<PrunedMergeMiningTag>(std::addressof(block.mergeMiningTag))) {
        XI_UNUSED(pmmt)
        XI_RETURN_EC(BlockError::MERGE_MINING_TAG_PRUNED);
      } else {
        XI_RETURN_EC(BlockError::MERGE_MINING_TAG_INVALID_TYPE);
      }
    }

    if (!m_currency.checkProofOfWork(cachedBlock, currentDifficulty)) {
      logger(Logging::WARNING) << "Proof of work too weak for block " << blockStr;
      return error::BlockValidationError::PROOF_OF_WORK_TOO_WEAK;
    }
  }

  {
    if (!Xi::Algorithm::is_unique(blockTemplate.transactionHashes.begin(), blockTemplate.transactionHashes.end()))
      return error::BlockValidationError::TRANSACTION_DUPLICATES;
    std::vector<Crypto::Hash> transactionHashes{transactions.size()};
    std::transform(transactions.begin(), transactions.end(), transactionHashes.begin(),
                   [](const auto& transaction) { return transaction.getTransactionHash(); });
    if (!Xi::Algorithm::is_unique(transactionHashes.begin(), transactionHashes.end()))
      return error::BlockValidationError::TRANSACTION_DUPLICATES;
    if (blockTemplate.transactionHashes.size() != transactions.size())
      return error::BlockValidationError::TRANSACTION_INCONSISTENCY;
    for (const auto& transaction : transactions) {
      auto search = std::find(blockTemplate.transactionHashes.begin(), blockTemplate.transactionHashes.end(),
                              transaction.getTransactionHash());
      if (search == blockTemplate.transactionHashes.end())
        return error::BlockValidationError::TRANSACTION_INCONSISTENCY;
    }
  }

  TransactionValidatorState validatorState;
  uint64_t cumulativeFee = 0;
  for (const auto& transaction : transactions) {
    uint64_t fee = 0;
    auto transactionValidationResult =
        validateTransaction(transaction, validatorState, cache, fee, previousBlockIndex, cachedBlock.getBlock().version,
                            cachedBlock.getBlock().timestamp);
    if (transactionValidationResult) {
      logger(Logging::DEBUGGING) << "Failed to validate transaction " << transaction.getTransactionHash() << ": "
                                 << transactionValidationResult.message();
      return transactionValidationResult;
    }

    cumulativeFee += fee;
  }

  uint64_t reward = 0;
  int64_t emissionChange = 0;
  auto alreadyGeneratedCoins = cache->getAlreadyGeneratedCoins(previousBlockIndex);
  auto lastBlocksSizes =
      cache->getLastBlocksSizes(m_currency.rewardBlocksWindowByBlockVersion(cachedBlock.getBlock().version),
                                previousBlockIndex, UseGenesis{false});
  auto blocksSizeMedian = Common::medianValue(lastBlocksSizes);

  if (!m_currency.getBlockReward(cachedBlock.getBlock().version, blocksSizeMedian, cumulativeBlockSize,
                                 alreadyGeneratedCoins, cumulativeFee, reward, emissionChange)) {
    logger(Logging::DEBUGGING) << "Block " << blockStr << " has too big cumulative size";
    return error::BlockValidationError::CUMULATIVE_BLOCK_SIZE_TOO_BIG;
  }

  if (minerReward != reward) {
    logger(Logging::DEBUGGING) << "Block reward mismatch for block " << blockStr << ". Expected reward: " << reward
                               << ", got reward: " << minerReward;
    return error::BlockValidationError::BLOCK_REWARD_MISMATCH;
  }

  auto ret = error::AddBlockErrorCode::ADDED_TO_ALTERNATIVE;

  if (m_isLightNode) {
    CachedBlock prunedBlockTemplate{fromBinaryArray<BlockTemplate>(rawBlock.blockTemplate)};
    prunedBlockTemplate.prune();
    rawBlock.blockTemplate = toBinaryArray(prunedBlockTemplate.getBlock());
    rawBlock.transactions.clear();
    rawBlock.transactions.reserve(transactions.size());
    for (auto& transaction : transactions) {
      transaction.prune();
      rawBlock.transactions.emplace_back(transaction.getTransactionBinaryArray());
    }
  }

  if (addOnTop) {
    if (cache->getChildCount() == 0) {
      // add block on top of leaf segment.
      auto hashes = preallocateVector<Crypto::Hash>(transactions.size());

      // TODO: exception safety
      if (cache == chainsLeaves[0]) {
        mainChainStorage->pushBlock(rawBlock, cumulativeBlockSize);

        cache->pushBlock(cachedBlock, transactions, validatorState, cumulativeBlockSize, emissionChange,
                         currentDifficulty, std::move(rawBlock));

        updateBlockMedianSize();

        ret = error::AddBlockErrorCode::ADDED_TO_MAIN;
        logger(Logging::DEBUGGING) << "Block " << blockStr << " added to main chain.";
        if ((previousBlockIndex + 1) % 100 == 0) {
          logger(Logging::INFO) << "Block " << blockStr << " added to main chain";
        }

        m_blockchainObservers.notify(&IBlockchainObserver::blockAdded, cachedBlock.getBlockIndex(),
                                     cachedBlock.getBlockHash());
      } else {
        cache->pushBlock(cachedBlock, transactions, validatorState, cumulativeBlockSize, emissionChange,
                         currentDifficulty, std::move(rawBlock));
        logger(Logging::DEBUGGING) << "Block " << blockStr << " added to alternative chain.";

        auto mainChainCache = chainsLeaves[0];
        if (cache->getCurrentCumulativeDifficulty() > mainChainCache->getCurrentCumulativeDifficulty()) {
          size_t endpointIndex =
              std::distance(chainsLeaves.begin(), std::find(chainsLeaves.begin(), chainsLeaves.end(), cache));

          assert(endpointIndex != chainsStorage.size());
          assert(endpointIndex != 0);

          std::swap(chainsLeaves[0], chainsLeaves[endpointIndex]);
          updateMainChainSet();
          updateBlockMedianSize();

          const auto splitIndex = chainsLeaves[0]->getStartBlockIndex();
          switchMainChainStorage(splitIndex, *chainsLeaves[0]);
          ret = error::AddBlockErrorCode::ADDED_TO_ALTERNATIVE_AND_SWITCHED;
          m_blockchainObservers.notify(&IBlockchainObserver::mainChainSwitched, std::cref(*chainsLeaves[endpointIndex]),
                                       std::cref(*chainsLeaves[0]), splitIndex);

          logger(Logging::INFO, Logging::YELLOW)
              << "Resolved: " << blockStr << ", Previous: " << chainsLeaves[endpointIndex]->getTopBlockIndex() << " ("
              << chainsLeaves[endpointIndex]->getTopBlockHash() << ")";
        } else {
          ret = error::AddBlockErrorCode::ADDED_TO_ALTERNATIVE;
        }
      }
    } else {
      // add block on top of segment which is not leaf! the case when we got more than one alternative block on the same
      // height
      auto newCache = blockchainCacheFactory->createBlockchainCache(m_currency, cache, previousBlockIndex + 1);
      cache->addChild(newCache.get());

      auto newlyForkedChainPtr = newCache.get();
      chainsStorage.emplace_back(std::move(newCache));
      chainsLeaves.push_back(newlyForkedChainPtr);

      logger(Logging::DEBUGGING) << "Resolving: " << blockStr;

      newlyForkedChainPtr->pushBlock(cachedBlock, transactions, validatorState, cumulativeBlockSize, emissionChange,
                                     currentDifficulty, std::move(rawBlock));

      updateMainChainSet();
      updateBlockMedianSize();
    }
  } else {
    logger(Logging::DEBUGGING) << "Resolving: " << blockStr;

    auto upperSegment = cache->split(previousBlockIndex + 1);
    //[cache] is lower segment now

    assert(upperSegment->getBlockCount() > 0);
    assert(cache->getBlockCount() > 0);

    if (upperSegment->getChildCount() == 0) {
      // newly created segment is leaf node
      //[cache] used to be a leaf node. we have to replace it with upperSegment
      auto found = std::find(chainsLeaves.begin(), chainsLeaves.end(), cache);
      assert(found != chainsLeaves.end());

      *found = upperSegment.get();
    }

    chainsStorage.emplace_back(std::move(upperSegment));

    auto newCache = blockchainCacheFactory->createBlockchainCache(m_currency, cache, previousBlockIndex + 1);
    cache->addChild(newCache.get());

    auto newlyForkedChainPtr = newCache.get();
    chainsStorage.emplace_back(std::move(newCache));
    chainsLeaves.push_back(newlyForkedChainPtr);

    newlyForkedChainPtr->pushBlock(cachedBlock, transactions, validatorState, cumulativeBlockSize, emissionChange,
                                   currentDifficulty, std::move(rawBlock));

    updateMainChainSet();
  }

  logger(Logging::DEBUGGING) << "Block: " << blockStr << " successfully added";
  notifyOnSuccess(ret, previousBlockIndex, cachedBlock, *cache);

  return ret;
}

void Core::switchMainChainStorage(uint32_t splitBlockIndex, IBlockchainCache& newChain) {
  XI_CONCURRENT_RLOCK(m_access);
  assert(mainChainStorage->getBlockCount() > splitBlockIndex);

  auto blocksToPop = mainChainStorage->getBlockCount() - splitBlockIndex;
  for (size_t i = 0; i < blocksToPop; ++i) {
    mainChainStorage->popBlock();
  }

  for (uint32_t index = splitBlockIndex; index <= newChain.getTopBlockIndex(); ++index) {
    mainChainStorage->pushBlock(newChain.getBlockByIndex(index), newChain.getCurrentBlockSize(index));
  }
}

void Core::notifyOnSuccess(error::AddBlockErrorCode opResult, uint32_t previousBlockIndex,
                           const CachedBlock& cachedBlock, const IBlockchainCache& cache) {
  XI_CONCURRENT_RLOCK(m_access);
  switch (opResult) {
    case error::AddBlockErrorCode::ADDED_TO_MAIN:
      notifyObservers(makeNewBlockMessage(previousBlockIndex + 1, cachedBlock.getBlockHash()));
      break;
    case error::AddBlockErrorCode::ADDED_TO_ALTERNATIVE:
      notifyObservers(makeNewAlternativeBlockMessage(previousBlockIndex + 1, cachedBlock.getBlockHash()));
      break;
    case error::AddBlockErrorCode::ADDED_TO_ALTERNATIVE_AND_SWITCHED: {
      auto parent = cache.getParent();
      auto hashes = cache.getBlockHashes(cache.getStartBlockIndex(), cache.getBlockCount());
      hashes.insert(hashes.begin(), parent->getTopBlockHash());
      notifyObservers(makeChainSwitchMessage(parent->getTopBlockIndex(), std::move(hashes)));
      break;
    }
    default:
      assert(false);
      break;
  }
}

std::error_code Core::addBlock(RawBlock&& rawBlock) {
  throwIfNotInitialized();

  BlockTemplate blockTemplate;
  bool result = fromBinaryArray(blockTemplate, rawBlock.blockTemplate);
  if (!result) {
    return error::AddBlockErrorCode::DESERIALIZATION_FAILED;
  }

  CachedBlock cachedBlock(blockTemplate);
  return addBlock(cachedBlock, std::move(rawBlock));
}

Xi::Result<std::vector<Crypto::Hash>> Core::addBlock(LiteBlock block, std::vector<CachedTransaction> txs) {
  XI_ERROR_TRY();
  throwIfNotInitialized();

  XI_CONCURRENT_RLOCK(m_access);
  XI_UNUSED_REVAL(transactionPool().acquireExclusiveAccess());

  BlockTemplate blockTemplate;
  if (!fromBinaryArray(blockTemplate, std::move(block.blockTemplate))) {
    logger(Logging::TRACE) << "Lite block has invalid blob";
    return failure(error::AddBlockErrorCode::DESERIALIZATION_FAILED);
  }

  // Quick check, we can skip everything if this fails
  if (findSegmentContainingBlock(blockTemplate.previousBlockHash) == nullptr) {
    logger(Logging::TRACE) << "Lite block rejected as orphaned";
    return failure(error::AddBlockErrorCode::REJECTED_AS_ORPHANED);
  }

  RawBlock filledRawBlock;
  filledRawBlock.blockTemplate = toBinaryArray(blockTemplate);
  filledRawBlock.transactions.resize(blockTemplate.transactionHashes.size());
  std::set<Crypto::Hash> providedTxs;
  std::transform(txs.begin(), txs.end(), std::inserter(providedTxs, providedTxs.begin()),
                 [](auto& iTx) { return iTx.getTransactionHash(); });

  std::vector<Crypto::Hash> missingTXs{};
  std::vector<uint64_t> missingTXsIndices{};
  {
    size_t i = 0;
    for (const auto& iTxHash : blockTemplate.transactionHashes) {
      if (providedTxs.find(iTxHash) != providedTxs.end()) {
        auto iTx =
            std::find_if(txs.begin(), txs.end(), [&](const auto& tx) { return tx.getTransactionHash() == iTxHash; });
        assert(iTx != txs.end());
        filledRawBlock.transactions[i] = iTx->getTransactionBinaryArray();
      } else {
        missingTXs.push_back(iTxHash);
        missingTXsIndices.push_back(i);
      }
      i += 1;
    }
  }

  if (!missingTXs.empty()) {
    std::vector<Crypto::Hash> innerMissingTXs{};
    std::vector<BinaryArray> txBlobs{};
    // If pruned we should only consider the pool, because the chain is missing crucial data to validate the lite block.
    getTransactions(missingTXs, txBlobs, innerMissingTXs, isPruned());
    if (!innerMissingTXs.empty()) {
      logger(Logging::TRACE) << "Lite block has missing transactions.";
      return success(std::move(innerMissingTXs));
    } else {
      assert(missingTXs.size() == missingTXsIndices.size());
      assert(missingTXs.size() == txBlobs.size());
      for (size_t i = 0; i < missingTXs.size(); ++i) {
        filledRawBlock.transactions[missingTXsIndices[i]] = std::move(txBlobs[i]);
      }
      logger(Logging::TRACE) << "Lite block is fully known and will be tried to add";
      return failure(addBlock(CachedBlock{std::move(blockTemplate)}, std::move(filledRawBlock)));
    }
  } else {
    logger(Logging::TRACE) << "Lite block is fully known and will be tried to add";
    return failure(addBlock(CachedBlock{std::move(blockTemplate)}, std::move(filledRawBlock)));
  }

  XI_ERROR_CATCH();
}

std::error_code Core::submitBlock(BinaryArray&& rawBlockTemplate) {
  throwIfNotInitialized();

  XI_CONCURRENT_RLOCK(m_access);
  XI_UNUSED_REVAL(transactionPool().acquireExclusiveAccess());

  BlockTemplate blockTemplate;
  bool result = fromBinaryArray(blockTemplate, rawBlockTemplate);
  if (!result) {
    logger(Logging::WARNING) << "Couldn't deserialize block template";
    return error::AddBlockErrorCode::DESERIALIZATION_FAILED;
  }

  RawBlock rawBlock;
  rawBlock.blockTemplate = std::move(rawBlockTemplate);

  rawBlock.transactions.reserve(blockTemplate.transactionHashes.size());
  Crypto::HashVector missedTransactions{};
  // If pruned we should only consider the pool, because the chain is missing crucial data to validate the lite block.
  getTransactions(blockTemplate.transactionHashes, rawBlock.transactions, missedTransactions, isPruned());
  XI_RETURN_EC_IF_NOT(missedTransactions.empty(), error::BlockValidationError::TRANSACTION_ABSENT);

  return addBlock(CachedBlock{blockTemplate}, std::move(rawBlock));
}

bool Core::getTransactionGlobalIndexes(const Crypto::Hash& transactionHash,
                                       std::vector<uint32_t>& globalIndexes) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  IBlockchainCache* segment = chainsLeaves[0];

  bool found = false;
  while (segment != nullptr && found == false) {
    found = segment->getTransactionGlobalIndexes(transactionHash, globalIndexes);
    segment = segment->getParent();
  }

  if (found) {
    return true;
  }

  for (size_t i = 1; i < chainsLeaves.size() && found == false; ++i) {
    segment = chainsLeaves[i];
    while (found == false && mainChainSet.count(segment) == 0) {
      found = segment->getTransactionGlobalIndexes(transactionHash, globalIndexes);
      segment = segment->getParent();
    }
  }

  return found;
}

bool Core::getRandomOutputs(uint64_t amount, uint16_t count, std::vector<uint32_t>& globalIndexes,
                            std::vector<Crypto::PublicKey>& publicKeys) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  if (count == 0) {
    return true;
  }

  auto upperBlockLimit = getTopBlockIndex() - m_currency.minedMoneyUnlockWindow();
  if (upperBlockLimit < m_currency.minedMoneyUnlockWindow()) {
    logger(Logging::DEBUGGING) << "Blockchain height is less than mined unlock window";
    return false;
  }

  globalIndexes = chainsLeaves[0]->getRandomOutsByAmount(amount, count, getTopBlockIndex());
  if (globalIndexes.empty()) {
    return false;
  }

  std::sort(globalIndexes.begin(), globalIndexes.end());

  switch (chainsLeaves[0]->extractKeyOutputKeys(amount, getTopBlockIndex(),
                                                {globalIndexes.data(), globalIndexes.size()}, publicKeys)) {
    case ExtractOutputKeysResult::SUCCESS:
      return true;
    case ExtractOutputKeysResult::TIME_PROVIDER_FAILED:
      logger(Logging::DEBUGGING) << "Time provider failed to yield a valid timestamp.";
      return false;
    case ExtractOutputKeysResult::INVALID_GLOBAL_INDEX:
      logger(Logging::DEBUGGING) << "Invalid global index is given";
      return false;
    case ExtractOutputKeysResult::OUTPUT_LOCKED:
      logger(Logging::DEBUGGING) << "Output is locked";
      return false;
  }

  return false;
}

std::vector<Crypto::Hash> Core::getPoolTransactionHashes() const {
  throwIfNotInitialized();

  return m_transactionPool->getTransactionHashes();
}

bool Core::getPoolChanges(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                          std::vector<Transaction>& addedTransactions,
                          std::vector<Crypto::Hash>& deletedTransactions) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  std::vector<Crypto::Hash> newTransactions;
  getTransactionPoolDifference(knownHashes, newTransactions, deletedTransactions);

  addedTransactions.reserve(newTransactions.size());
  for (const auto& hash : newTransactions) {
    addedTransactions.emplace_back(m_transactionPool->getTransaction(hash).getTransaction());
  }

  return getTopBlockHash() == lastBlockHash;
}

bool Core::getPoolChanges(const Hash& lastBlockHash, const std::vector<Hash>& knownHashes,
                          std::vector<BinaryArray>& addedTransactions, std::vector<Hash>& deletedTransactions) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  std::vector<Crypto::Hash> newTransactions;
  getTransactionPoolDifference(knownHashes, newTransactions, deletedTransactions);

  addedTransactions.reserve(newTransactions.size());
  for (const auto& hash : newTransactions) {
    addedTransactions.emplace_back(m_transactionPool->getTransaction(hash).getTransactionBinaryArray());
  }

  return getTopBlockHash() == lastBlockHash;
}

bool Core::getPoolChangesLite(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                              std::vector<TransactionPrefixInfo>& addedTransactions,
                              std::vector<Crypto::Hash>& deletedTransactions) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  std::vector<Crypto::Hash> newTransactions;
  getTransactionPoolDifference(knownHashes, newTransactions, deletedTransactions);

  addedTransactions.reserve(newTransactions.size());
  for (const auto& hash : newTransactions) {
    TransactionPrefixInfo transactionPrefixInfo;
    transactionPrefixInfo.hash = hash;
    transactionPrefixInfo.prefix =
        static_cast<const TransactionPrefix&>(m_transactionPool->getTransaction(hash).getTransaction());
    addedTransactions.emplace_back(std::move(transactionPrefixInfo));
  }

  return getTopBlockHash() == lastBlockHash;
}

bool Core::getBlockTemplate(BlockTemplate& b, const AccountPublicAddress& adr, const BinaryArray& extraNonce,
                            uint64_t& difficulty, uint32_t& index) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  XI_UNUSED_REVAL(transactionPool().acquireExclusiveAccess());

  index = getTopBlockIndex() + 1;
  difficulty = getDifficultyForNextBlock();
  if (difficulty == 0) {
    logger(Logging::ERROR) << "difficulty overhead.";
    return false;
  }

  b = boost::value_initialized<BlockTemplate>();
  b.version = BlockVersion{getBlockVersionForIndex(index)};
  b.upgradeVote = b.version;
  b.previousBlockHash = getTopBlockHash();

  if (m_currency.isStaticRewardEnabledForBlockVersion(b.version)) {
    auto staticReward = m_currency.constructStaticRewardTx(b.previousBlockHash, b.version, index);
    if (staticReward.isError() || !staticReward.value().has_value()) {
      logger(Logging::ERROR) << "expected static reward but consturation failed";
      return false;
    }
    CachedTransaction cStaticReward{std::move(staticReward.take().value())};
    auto txHash = cStaticReward.getTransactionHash();
    b.staticRewardHash = Xi::Crypto::Hash::Crc::Hash16::Null;
    compute(txHash.span(), *b.staticRewardHash);
  } else {
    b.staticRewardHash = std::nullopt;
  }

  auto timestamp = timeProvider().posixNow();
  if (timestamp.isError()) {
    logger(Logging::ERROR) << "Failed to receive timestamp: " << timestamp.error().message();
    return false;
  }
  b.timestamp = timestamp.value();

  /* Ok, so if an attacker is fiddling around with timestamps on the network,
     they can make it so all the valid pools / miners don't produce valid
     blocks. This is because the timestamp is created as the users current time,
     however, if the attacker is a large % of the hashrate, they can slowly
     increase the timestamp into the future, shifting the median timestamp
     forwards. At some point, this will mean the valid pools will submit a
     block with their valid timestamps, and it will be rejected for being
     behind the median timestamp / too far in the past. The simple way to
     handle this is just to check if our timestamp is going to be invalid, and
     set it to the median.

     Once the attack ends, the median timestamp will remain how it is, until
     the time on the clock goes forwards, and we can start submitting valid
     timestamps again, and then we are back to normal. */

  /* Thanks to jagerman for this patch:
     https://github.com/loki-project/loki/pull/26 */

  /* How many blocks we look in the past to calculate the median timestamp */
  uint32_t blockchain_timestamp_check_window = m_currency.timestampCheckWindow(b.version);

  /* Skip the first N blocks, we don't have enough blocks to calculate a
     proper median yet */
  if (index >= blockchain_timestamp_check_window) {
    std::vector<uint64_t> timestamps;

    /* For the last N blocks, get their timestamps */
    for (uint32_t offset = index - blockchain_timestamp_check_window; offset < index; offset++) {
      timestamps.push_back(getBlockTimestampByIndex(offset));
    }

    uint64_t medianTimestamp = Common::medianValue(timestamps);

    if (b.timestamp < medianTimestamp) {
      b.timestamp = medianTimestamp;
    }
  }

  size_t medianSize = calculateCumulativeBlocksizeLimit(index) / 2;
  const size_t maxBlockSize = std::max(2 * medianSize, m_currency.maxBlockCumulativeSize(index));

  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());
  uint64_t alreadyGeneratedCoins = chainsLeaves[0]->getAlreadyGeneratedCoins();

  size_t transactionsSize;
  uint64_t fee;
  fillBlockTemplate(b, index, medianSize, maxBlockSize, transactionsSize, fee);

  /*
     two-phase miner transaction generation: we don't know exact block size until we prepare block, but we don't know
     reward until we know
     block size, so first miner transaction generated with fake amount of money, and with phase we know think we know
     expected block size
  */
  // make blocks coin-base tx looks close to real coinbase tx to get truthful blob size
  bool r = m_currency.constructMinerTx(b.version, index, medianSize, alreadyGeneratedCoins, transactionsSize, fee, adr,
                                       b.baseTransaction, extraNonce, 20);
  if (!r) {
    logger(Logging::ERROR) << "Failed to construct miner tx, first chance";
    return false;
  }

  size_t cumulativeSize = transactionsSize + b.baseTransaction.binarySize();
  const size_t TRIES_COUNT = 10;
  for (size_t tryCount = 0; tryCount < TRIES_COUNT; ++tryCount) {
    r = m_currency.constructMinerTx(b.version, index, medianSize, alreadyGeneratedCoins, cumulativeSize, fee, adr,
                                    b.baseTransaction, extraNonce, 20);
    if (!r) {
      logger(Logging::ERROR) << "Failed to construct miner tx, second chance";
      return false;
    }

    size_t coinbaseBlobSize = b.baseTransaction.binarySize();
    if (coinbaseBlobSize > cumulativeSize - transactionsSize) {
      cumulativeSize = transactionsSize + coinbaseBlobSize;
      continue;
    }

    if (coinbaseBlobSize < cumulativeSize - transactionsSize) {
      size_t delta = cumulativeSize - transactionsSize - coinbaseBlobSize;
      b.baseTransaction.extra.insert(b.baseTransaction.extra.end(), delta, 0);
      // here  could be 1 byte difference, because of extra field counter is varint, and it can become from 1-byte len
      // to 2-bytes len.
      if (cumulativeSize != transactionsSize + b.baseTransaction.binarySize()) {
        if (!(cumulativeSize + 1 == transactionsSize + b.baseTransaction.binarySize())) {
          logger(Logging::ERROR) << "unexpected case: cumulative_size=" << cumulativeSize
                                 << " + 1 is not equal txs_cumulative_size=" << transactionsSize
                                 << " + get_object_blobsize(b.baseTransaction)=" << b.baseTransaction.binarySize();
          return false;
        }

        b.baseTransaction.extra.resize(b.baseTransaction.extra.size() - 1);
        if (cumulativeSize != transactionsSize + b.baseTransaction.binarySize()) {
          // not lucky, -1 makes varint-counter size smaller, in that case we continue to grow with
          // cumulative_size
          logger(Logging::TRACE, Logging::BRIGHT_RED)
              << "Miner tx creation have no luck with delta_extra size = " << delta << " and " << delta - 1;
          cumulativeSize += delta - 1;
          continue;
        }

        logger(Logging::DEBUGGING, Logging::BRIGHT_BLUE)
            << "Setting extra for block: " << b.baseTransaction.extra.size() << ", try_count=" << tryCount;
      }
    }
    if (!(cumulativeSize == transactionsSize + b.baseTransaction.binarySize())) {
      logger(Logging::ERROR) << "unexpected case: cumulative_size=" << cumulativeSize
                             << " is not equal txs_cumulative_size=" << transactionsSize
                             << " + get_object_blobsize(b.baseTransaction)=" << b.baseTransaction.binarySize();
      return false;
    }

    return true;
  }

  logger(Logging::ERROR) << "Failed to create_block_template with " << TRIES_COUNT << " tries";
  return false;
}

CoreStatistics Core::getCoreStatistics() const {
  XI_CONCURRENT_RLOCK(m_access);
  CoreStatistics result;
  result.transactionPoolSize = transactionPool().size();
  auto txHash = transactionPool().stateHash();
  result.transactionPoolState = txHash.toString();
  result.blockchainHeight = getTopBlockIndex();
  result.alternativeBlockCount = getAlternativeBlockCount();
  auto hash = getTopBlockHash();
  result.topBlockHashString = hash.toString();
  return result;
}

bool Core::isPruned() const { return m_isLightNode; }

size_t Core::getBlockchainTransactionCount() const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  IBlockchainCache* mainChain = chainsLeaves[0];
  return mainChain->getTransactionCount();
}

size_t Core::getAlternativeBlockCount() const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  using Ptr = decltype(chainsStorage)::value_type;
  return std::accumulate(chainsStorage.begin(), chainsStorage.end(), size_t(0), [&](size_t sum, const Ptr& ptr) {
    return mainChainSet.count(ptr.get()) == 0 ? sum + ptr->getBlockCount() : sum;
  });
}

uint64_t Core::getTotalGeneratedAmount() const {
  assert(!chainsLeaves.empty());
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  return chainsLeaves[0]->getAlreadyGeneratedCoins();
}

std::vector<BlockTemplate> Core::getAlternativeBlocks() const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  std::vector<BlockTemplate> alternativeBlocks;
  for (auto& cache : chainsStorage) {
    if (mainChainSet.count(cache.get())) continue;
    for (auto index = cache->getStartBlockIndex(); index <= cache->getTopBlockIndex(); ++index) {
      // TODO: optimize
      alternativeBlocks.push_back(fromBinaryArray<BlockTemplate>(cache->getBlockByIndex(index).blockTemplate));
    }
  }

  return alternativeBlocks;
}

std::vector<Transaction> Core::getPoolTransactions() const {
  throwIfNotInitialized();

  std::vector<Transaction> transactions;
  auto hashes = m_transactionPool->getPoolTransactions();
  std::transform(std::begin(hashes), std::end(hashes), std::back_inserter(transactions),
                 [&](const CachedTransaction& tx) { return tx.getTransaction(); });
  return transactions;
}

bool Core::extractTransactions(const std::vector<BinaryArray>& rawTransactions,
                               std::vector<CachedTransaction>& transactions, uint64_t& cumulativeSize,
                               BlockVersion blockVersion) {
  try {
    for (const auto& rawTransaction : rawTransactions) {
      if (rawTransaction.size() > m_currency.maxTxSize(blockVersion)) {
        logger(Logging::INFO) << "Raw transaction size " << rawTransaction.size() << " is too big.";
        return false;
      }

      cumulativeSize += rawTransaction.size();
      transactions.push_back(CachedTransaction{rawTransaction});
    }
  } catch (std::runtime_error& e) {
    logger(Logging::INFO) << e.what();
    return false;
  }

  return true;
}

std::error_code Core::validateTransaction(const CachedTransaction& cachedTransaction, TransactionValidatorState& state,
                                          IBlockchainCache* cache, uint64_t& fee, uint32_t blockIndex,
                                          BlockVersion blockVersion, uint64_t blockTimestamp) {
  // TransactionValidatorState currentState;
  if (cachedTransaction.getTransactionBinaryArray().size() > currency().maxTxSize(blockVersion)) {
    return error::TransactionValidationError::TOO_LARGE;
  }

  const auto& transaction = cachedTransaction.getTransaction();
  if (transaction.version > m_currency.maxTxVersion() || transaction.version < m_currency.minTxVersion()) {
    return error::TransactionValidationError::INVALID_VERSION;
  }

  auto error = validateSemantic(transaction, fee, blockIndex);
  if (error != error::TransactionValidationError::VALIDATION_SUCCESS) {
    return error;
  }

  if (!validateCanonicalDecomposition(transaction)) {
    return error::TransactionValidationError::OUTPUTS_NOT_CANONCIAL;
  }

  if (!(fee == 0 && currency().isFusionTransaction(transaction, cachedTransaction.getBlobSize(), blockVersion))) {
    const size_t canonicalBuckets = countCanonicalDecomposition(transaction);
    const auto minimumFee = currency().minimumFee(blockVersion);
    const auto penalizedMinimumFee = minimumFee + (canonicalBuckets / 4) * minimumFee;
    if (fee < penalizedMinimumFee) {
      return error::TransactionValidationError::FEE_INSUFFICIENT;
    }
  }

  error = validateExtra(transaction);
  if (error != error::TransactionValidationError::VALIDATION_SUCCESS) {
    return error;
  }

  const size_t requiredRingSize = m_currency.requiredMixin(blockVersion) + 1;
  std::map<Amount, std::set<uint32_t>> usedGlobalIndices;

  if (!std::holds_alternative<TransactionSignatureCollection>(transaction.signatures)) {
    return error::TransactionValidationError::INVALID_SIGNATURE_TYPE;
  }
  const auto& ringSignatures = std::get<TransactionSignatureCollection>(transaction.signatures);

  size_t inputIndex = 0;
  for (const auto& input : transaction.inputs) {
    if (inputIndex >= ringSignatures.size()) {
      return error::TransactionValidationError::INPUT_WRONG_SIGNATURES_COUNT;
    }

    if (auto keyInput = std::get_if<KeyInput>(&input)) {
      if (!state.spentKeyImages.insert(keyInput->keyImage).second) {
        return error::TransactionValidationError::INPUT_KEYIMAGE_ALREADY_SPENT;
      }

      const auto ringSize = keyInput->outputIndices.size();
      if (ringSize < requiredRingSize) {
        return error::TransactionValidationError::INPUT_MIXIN_TOO_LOW;
      } else if (ringSize > requiredRingSize) {
        return error::TransactionValidationError::INPUT_MIXIN_TOO_HIGH;
      }

      if (!checkpoints.isInCheckpointZone(blockIndex + 1)) {
        if (cache->checkIfSpent(keyInput->keyImage, blockIndex)) {
          return error::TransactionValidationError::INPUT_KEYIMAGE_ALREADY_SPENT;
        }

        std::vector<PublicKey> outputKeys;
        assert(!keyInput->outputIndices.empty());

        std::vector<uint32_t> globalIndexes = relativeOutputOffsetsToAbsolute(keyInput->outputIndices);
        for (auto globalIndex : globalIndexes) {
          if (!usedGlobalIndices[keyInput->amount].insert(globalIndex).second) {
            return error::TransactionValidationError::INPUT_DUPLICATE_GLOBAL_INDEX;
          }
        }

        auto result = cache->extractKeyOutputKeys(
            keyInput->amount, blockIndex, {globalIndexes.data(), globalIndexes.size()}, outputKeys, blockTimestamp);
        if (result == ExtractOutputKeysResult::INVALID_GLOBAL_INDEX) {
          return error::TransactionValidationError::INPUT_INVALID_GLOBAL_INDEX;
        }

        if (result == ExtractOutputKeysResult::OUTPUT_LOCKED) {
          return error::TransactionValidationError::INPUT_SPEND_LOCKED_OUT;
        }

        if (outputKeys.size() != ringSignatures[inputIndex].size()) {
          return error::TransactionValidationError::INPUT_INVALID_SIGNATURES_COUNT;
        }

        std::vector<const Crypto::PublicKey*> outputKeyPointers;
        outputKeyPointers.reserve(outputKeys.size());
        std::for_each(outputKeys.begin(), outputKeys.end(),
                      [&outputKeyPointers](const Crypto::PublicKey& key) { outputKeyPointers.push_back(&key); });
        if (outputKeyPointers.size() != ringSignatures[inputIndex].size()) {
          return error::TransactionValidationError::INPUT_INVALID_SIGNATURES_COUNT;
        }
        if (!Crypto::check_ring_signature(cachedTransaction.getTransactionPrefixHash(), keyInput->keyImage,
                                          outputKeyPointers.data(), outputKeyPointers.size(),
                                          ringSignatures[inputIndex].data(), true)) {
          return error::TransactionValidationError::INPUT_INVALID_SIGNATURES;
        }
      }

    } else {
      assert(false);
      return error::TransactionValidationError::INPUT_UNKNOWN_TYPE;
    }

    inputIndex++;
  }

  return error::TransactionValidationError::VALIDATION_SUCCESS;
}

std::error_code Core::validateSemantic(const Transaction& transaction, uint64_t& fee, uint32_t) {
  if (transaction.inputs.empty()) {
    return error::TransactionValidationError::EMPTY_INPUTS;
  }
  if (transaction.outputs.empty()) {
    return error::TransactionValidationError::EMPTY_OUTPUTS;
  }
  if (transaction.extra.size() > TX_EXTRA_MAX_SIZE) {
    return error::TransactionValidationError::EXTRA_NONCE_TOO_LARGE;
  }

  uint64_t summaryOutputAmount = 0;
  for (const auto& output : transaction.outputs) {
    if (output.amount == 0) {
      return error::TransactionValidationError::OUTPUT_ZERO_AMOUNT;
    }

    if (auto keyOutput = std::get_if<KeyOutput>(&output.target)) {
      if (!check_key(keyOutput->key)) {
        return error::TransactionValidationError::OUTPUT_INVALID_KEY;
      }
    } else {
      return error::TransactionValidationError::OUTPUT_UNKNOWN_TYPE;
    }

    if (std::numeric_limits<uint64_t>::max() - output.amount < summaryOutputAmount) {
      return error::TransactionValidationError::OUTPUTS_AMOUNT_OVERFLOW;
    }

    summaryOutputAmount += output.amount;
  }

  uint64_t summaryInputAmount = 0;
  std::unordered_set<Crypto::KeyImage> ki;
  std::set<std::pair<uint64_t, uint32_t>> outputsUsage;
  for (const auto& input : transaction.inputs) {
    uint64_t amount = 0;
    if (auto keyInput = std::get_if<KeyInput>(&input)) {
      amount = keyInput->amount;
      if (!ki.insert(keyInput->keyImage).second) {
        return error::TransactionValidationError::INPUT_IDENTICAL_KEYIMAGES;
      }

      if (keyInput->outputIndices.empty()) {
        return error::TransactionValidationError::INPUT_EMPTY_OUTPUT_USAGE;
      }

      // outputIndexes are packed here, first is absolute, others are offsets to previous,
      // so first can be zero, others can't
      // Fix discovered by Monero Lab and suggested by "fluffypony" (bitcointalk.org)
      if (!keyInput->keyImage.isValid()) {
        return error::TransactionValidationError::INPUT_INVALID_DOMAIN_KEYIMAGES;
      }

      if (std::find(++std::begin(keyInput->outputIndices), std::end(keyInput->outputIndices), 0u) !=
          std::end(keyInput->outputIndices)) {
        return error::TransactionValidationError::INPUT_IDENTICAL_OUTPUT_INDEXES;
      }
    } else {
      return error::TransactionValidationError::INPUT_UNKNOWN_TYPE;
    }

    if (std::numeric_limits<uint64_t>::max() - amount < summaryInputAmount) {
      return error::TransactionValidationError::INPUTS_AMOUNT_OVERFLOW;
    }

    summaryInputAmount += amount;
  }

  if (summaryOutputAmount > summaryInputAmount) {
    return error::TransactionValidationError::INPUT_AMOUNT_INSUFFICIENT;
  }

  fee = summaryInputAmount - summaryOutputAmount;
  return error::TransactionValidationError::VALIDATION_SUCCESS;
}

Xi::Result<uint32_t> Core::findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds) const {
  XI_ERROR_TRY();
  for (auto& hash : remoteBlockIds) {
    IBlockchainCache* blockchainSegment = findMainChainSegmentContainingBlock(hash);
    if (blockchainSegment != nullptr) {
      if (findIndexInChain(blockchainSegment, m_currency.genesisBlockHash()) == nullptr) {
        return failure(error::CoreErrorCode::GENESIS_BLOCK_NOT_FOUND);
      } else {
        return success(blockchainSegment->getBlockIndex(hash));
      }
    }
  }
  return failure(error::CoreErrorCode::GENESIS_BLOCK_NOT_FOUND);
  XI_ERROR_CATCH();
}

std::vector<Crypto::Hash> CryptoNote::Core::getBlockHashes(uint32_t startBlockIndex, uint32_t maxCount) const {
  return chainsLeaves[0]->getBlockHashes(startBlockIndex, maxCount);
}

std::error_code Core::validateBlock(const CachedBlock& cachedBlock, IBlockchainCache* cache, uint64_t& minerReward) {
  const auto& block = cachedBlock.getBlock();
  auto previousBlockIndex = cache->getBlockIndex(block.previousBlockHash);
  // assert(block.previousBlockHash == cache->getBlockHash(previousBlockIndex));

  minerReward = 0;

  if (m_upgradeManager->getBlockVersion(cachedBlock.getBlockIndex()) != block.version) {
    return error::BlockValidationError::WRONG_VERSION;
  }
  if (block.upgradeVote < block.version || block.upgradeVote.native() > block.version.native() + 1) {
    return error::BlockValidationError::WRONG_UPGRADE_VOTE;
  }

  if (block.timestamp > getAdjustedTime() + m_currency.blockFutureTimeLimit(block.version)) {
    return error::BlockValidationError::TIMESTAMP_TOO_FAR_IN_FUTURE;
  }

  auto timestamps =
      cache->getLastTimestamps(m_currency.timestampCheckWindow(block.version), previousBlockIndex, addGenesisBlock);
  if (timestamps.size() >= m_currency.timestampCheckWindow(block.version)) {
    auto median_ts = Common::medianValue(timestamps);
    if (block.timestamp < median_ts) {
      return error::BlockValidationError::TIMESTAMP_TOO_FAR_IN_PAST;
    }
  }

  if ((block.baseTransaction.version < m_currency.minTxVersion()) ||
      (block.baseTransaction.version > m_currency.maxTxVersion())) {
    return error::TransactionValidationError::INVALID_VERSION;
  }

  {
    auto extraEc = validateExtra(block.baseTransaction);
    if (extraEc != error::TransactionValidationError::VALIDATION_SUCCESS) {
      return extraEc;
    }
  }

  if (block.baseTransaction.inputs.size() != 1) {
    return error::TransactionValidationError::BASE_INPUT_WRONG_COUNT;
  }

  auto baseInput = std::get_if<BaseInput>(&block.baseTransaction.inputs[0]);
  if (baseInput == nullptr) {
    return error::TransactionValidationError::BASE_INPUT_UNEXPECTED_TYPE;
  }

  if (baseInput->height.isNull() || baseInput->height != BlockHeight::fromIndex(previousBlockIndex + 1)) {
    return error::TransactionValidationError::BASE_INPUT_WRONG_BLOCK_INDEX;
  }

  if (!(block.baseTransaction.unlockTime == previousBlockIndex + 1 + m_currency.minedMoneyUnlockWindow())) {
    return error::TransactionValidationError::BASE_TRANSACTION_WRONG_UNLOCK_TIME;
  }

  if (!std::holds_alternative<TransactionSignatureCollection>(block.baseTransaction.signatures)) {
    return error::TransactionValidationError::INVALID_SIGNATURE_TYPE;
  }
  const auto& ringSignatures = std::get<TransactionSignatureCollection>(block.baseTransaction.signatures);

  if (!ringSignatures.empty()) {
    return error::TransactionValidationError::BASE_INVALID_SIGNATURES_COUNT;
  }

  std::vector<uint64_t> embeddedAmounts{};
  embeddedAmounts.reserve(block.baseTransaction.outputs.size());

  for (const auto& output : block.baseTransaction.outputs) {
    if (output.amount == 0) {
      return error::TransactionValidationError::OUTPUT_ZERO_AMOUNT;
    }

    auto keyOutput = std::get_if<KeyOutput>(&output.target);
    if (keyOutput == nullptr) {
      return error::TransactionValidationError::OUTPUT_UNKNOWN_TYPE;
    }

    if (!check_key(keyOutput->key)) {
      return error::TransactionValidationError::OUTPUT_INVALID_KEY;
    }

    if (std::numeric_limits<uint64_t>::max() - output.amount < minerReward) {
      return error::TransactionValidationError::OUTPUTS_AMOUNT_OVERFLOW;
    }

    minerReward += output.amount;
    embeddedAmounts.push_back(output.amount);
  }

  std::vector<uint64_t> canoncialAmounts;
  decomposeAmount(minerReward, canoncialAmounts);

  if (embeddedAmounts != canoncialAmounts) {
    return error::TransactionValidationError::OUTPUTS_NOT_CANONCIAL;
  }

  // BEGIN: Static Reward Hash Validation ----------------------------------------------------------------------------
  const auto& b = cachedBlock.getBlock();
  if (m_currency.isStaticRewardEnabledForBlockVersion(b.version)) {
    if (!b.staticRewardHash.has_value()) {
      return error::BlockValidationError::STATIC_REWARD_MISMATCH;
    }

    auto staticReward = m_currency.constructStaticRewardTx(b.previousBlockHash, b.version, previousBlockIndex + 1);
    if (staticReward.isError() || !staticReward.value().has_value()) {
      logger(Logging::ERROR) << "expected static reward but consturation failed while validating block";
      return error::BlockValidationError::STATIC_REWARD_MISMATCH;
      ;
    }
    CachedTransaction cStaticReward{std::move(staticReward.take().value())};
    const auto expectedStaticRewardHash = cStaticReward.getTransactionHash();
    Xi::Crypto::Hash::Crc::Hash16 crc{};
    compute(expectedStaticRewardHash.span(), crc);
    if (std::memcmp(b.staticRewardHash->data(), crc.data(), crc.size()) != 0) {
      return error::BlockValidationError::STATIC_REWARD_MISMATCH;
    }
  } else if (b.staticRewardHash.has_value()) {
    return error::BlockValidationError::STATIC_REWARD_MISMATCH;
  }
  // END: Static Reward Hash Validation ------------------------------------------------------------------------------

  return error::TransactionValidationError::VALIDATION_SUCCESS;
}

uint64_t CryptoNote::Core::getAdjustedTime() const { return m_timeProvider->posixNow().valueOrThrow(); }

const Currency& Core::getCurrency() const { return m_currency; }

bool Core::save() {
  try {
    throwIfNotInitialized();
    XI_CONCURRENT_RLOCK(m_access);

    deleteAlternativeChains();
    mergeMainChainSegments();
    return chainsLeaves[0]->save();
  } catch (std::exception& e) {
    logger(Logging::FATAL) << "Core save routine failed: " << e.what();
    return false;
  } catch (...) {
    return false;
  }
}

bool Core::load() {
  try {
    if (initialized) {
      return false;
    }
    XI_CONCURRENT_RLOCK(m_access);

    XI_RETURN_EC_IF_NOT(initRootSegment(), false);

    start_time = std::time(nullptr);

    auto dbBlocksCount = chainsLeaves[0]->getTopBlockIndex() + 1;
    auto storageBlocksCount = mainChainStorage->getBlockCount();

    logger(Logging::DEBUGGING) << "Blockchain storage blocks count: " << storageBlocksCount
                               << ", DB blocks count: " << dbBlocksCount;

    assert(storageBlocksCount != 0);  // we assume the storage has at least genesis block

    if (storageBlocksCount > dbBlocksCount) {
      logger(Logging::INFO) << "Importing blocks from blockchain storage";
      XI_RETURN_EC_IF_NOT(importBlocksFromStorage(), false);
    } else if (storageBlocksCount < dbBlocksCount) {
      auto cutFrom = findCommonRoot(*mainChainStorage, *chainsLeaves[0]) + 1;

      logger(Logging::INFO) << "DB has more blocks than blockchain storage, cutting from block index: " << cutFrom;
      cutSegment(*chainsLeaves[0], cutFrom);

      assert(chainsLeaves[0]->getTopBlockIndex() + 1 == mainChainStorage->getBlockCount());
    } else if (getBlockHash(mainChainStorage->getBlockByIndex(storageBlocksCount - 1)) !=
               chainsLeaves[0]->getTopBlockHash()) {
      logger(Logging::INFO) << "Blockchain storage and root segment are on different chains. "
                            << "Cutting root segment to common block index "
                            << findCommonRoot(*mainChainStorage, *chainsLeaves[0]) << " and reimporting blocks";
      XI_RETURN_EC_IF_NOT(importBlocksFromStorage(), false);
    } else {
      logger(Logging::DEBUGGING) << "Blockchain storage and root segment are on the same height and chain";
    }

    initialized = true;
    return true;
  } catch (std::exception& e) {
    logger(Logging::FATAL) << "Core loading routine failed: " << e.what();
    return false;
  } catch (...) {
    return false;
  }
}

bool Core::initRootSegment() {
  auto cache = this->blockchainCacheFactory->createRootBlockchainCache(m_currency);

  mainChainSet.emplace(cache.get());

  chainsLeaves.push_back(cache.get());
  chainsStorage.push_back(std::move(cache));

  contextGroup.spawn(std::bind(&Core::transactionPoolCleaningProcedure, this));

  updateBlockMedianSize();

  return chainsLeaves[0]->load();
}

bool Core::importBlocksFromStorage() {
  uint32_t commonIndex = findCommonRoot(*mainChainStorage, *chainsLeaves[0]);
  assert(commonIndex <= mainChainStorage->getBlockCount());

  cutSegment(*chainsLeaves[0], commonIndex + 1);

  auto previousBlockHash = getBlockHash(mainChainStorage->getBlockByIndex(commonIndex));
  auto blockCount = mainChainStorage->getBlockCount();
  for (uint32_t i = commonIndex + 1; i < blockCount; ++i) {
    RawBlock rawBlock = mainChainStorage->getBlockByIndex(i);
    auto blockTemplate = extractBlockTemplate(rawBlock);
    CachedBlock cachedBlock(blockTemplate);

    if (blockTemplate.previousBlockHash != previousBlockHash) {
      logger(Logging::ERROR) << "Corrupted blockchain. Block with index " << i << " and hash "
                             << cachedBlock.getBlockHash() << " has previous block hash "
                             << blockTemplate.previousBlockHash << ", but parent has hash " << previousBlockHash
                             << ". Resynchronize your daemon please.";
      throw std::system_error(make_error_code(error::CoreErrorCode::CORRUPTED_BLOCKCHAIN));
    }

    previousBlockHash = cachedBlock.getBlockHash();

    std::vector<CachedTransaction> transactions;

    {
      uint64_t _ = 0;
      if (!extractTransactions(rawBlock.transactions, transactions, _, blockTemplate.version)) {
        logger(Logging::ERROR) << "Couldn't deserialize raw block transactions in block " << cachedBlock.getBlockHash();
        throw std::system_error(make_error_code(error::AddBlockErrorCode::DESERIALIZATION_FAILED));
      }
    }
    const auto cumulativeSize = mainChainStorage->getBlobSizeByIndex(i);
    TransactionValidatorState spentOutputs = extractSpentOutputs(transactions);
    auto currentDifficulty = chainsLeaves[0]->getDifficultyForNextBlock(blockTemplate.version, i - 1);

    uint64_t cumulativeFee = std::accumulate(
        transactions.begin(), transactions.end(), UINT64_C(0),
        [](uint64_t fee, const CachedTransaction& transaction) { return fee + transaction.getTransactionFee(); });

    int64_t emissionChange =
        getEmissionChange(m_currency, *chainsLeaves[0], i - 1, cachedBlock, cumulativeSize, cumulativeFee);

    chainsLeaves[0]->pushBlock(cachedBlock, transactions, spentOutputs, cumulativeSize, emissionChange,
                               currentDifficulty, std::move(rawBlock));

    if (i % 1000 == 0) {
      logger(Logging::INFO) << "Imported block with index " << i << " / " << (blockCount - 1);
    }
  }
  return true;
}

bool Core::importTransactionPool() { return false; }

bool Core::exportTransactionPool() { return false; }

void Core::cutSegment(IBlockchainCache& segment, uint32_t startIndex) {
  if (segment.getTopBlockIndex() < startIndex) {
    return;
  }

  logger(Logging::INFO) << "Cutting root segment from index " << startIndex;
  auto childCache = segment.split(startIndex);
  segment.deleteChild(childCache.get());
}

void Core::updateMainChainSet() {
  mainChainSet.clear();
  IBlockchainCache* chainPtr = chainsLeaves[0];
  assert(chainPtr != nullptr);
  do {
    mainChainSet.insert(chainPtr);
    chainPtr = chainPtr->getParent();
  } while (chainPtr != nullptr);
}

IBlockchainCache* Core::findSegmentContainingBlock(const Crypto::Hash& blockHash, bool* isMainChain) const {
  assert(chainsLeaves.size() > 0);

  // first search in main chain
  auto blockSegment = findMainChainSegmentContainingBlock(blockHash);
  if (blockSegment != nullptr) {
    if (isMainChain != nullptr) {
      *isMainChain = true;
    }
    return blockSegment;
  }

  // than search in alternative chains
  if (isMainChain != nullptr) {
    *isMainChain = false;
  }
  return findAlternativeSegmentContainingBlock(blockHash);
}

IBlockchainCache* Core::findSegmentContainingBlock(uint32_t blockHeight, bool* isMainChain) const {
  assert(chainsLeaves.size() > 0);

  // first search in main chain
  auto blockSegment = findMainChainSegmentContainingBlock(blockHeight);
  if (blockSegment != nullptr) {
    if (isMainChain != nullptr) {
      *isMainChain = true;
    }
    return blockSegment;
  }

  // than search in alternative chains
  if (isMainChain != nullptr) {
    *isMainChain = false;
  }
  return findAlternativeSegmentContainingBlock(blockHeight);
}

IBlockchainCache* Core::findAlternativeSegmentContainingBlock(const Crypto::Hash& blockHash) const {
  std::set<IBlockchainCache*> alternativesPorpagated{};
  for (auto it = ++chainsLeaves.begin(); it != chainsLeaves.end(); ++it) {
    auto cache = *it;
    while (alternativesPorpagated.count(cache) == 0 && mainChainSet.count(cache) == 0) {
      if (cache->hasBlock(blockHash)) {
        return cache;
      }

      alternativesPorpagated.insert(cache);
      cache = cache->getParent();
    }
  }
  return nullptr;
}

IBlockchainCache* Core::findMainChainSegmentContainingBlock(const Crypto::Hash& blockHash) const {
  return findIndexInChain(chainsLeaves[0], blockHash);
}

IBlockchainCache* Core::findMainChainSegmentContainingBlock(uint32_t blockIndex) const {
  return findIndexInChain(chainsLeaves[0], blockIndex);
}

IBlockchainCache* Core::findAlternativeSegmentContainingBlock(uint32_t blockIndex) const {
  for (auto it = ++chainsLeaves.begin(); it != chainsLeaves.end(); ++it) {
    auto cache = findIndexInChain(*it, blockIndex);
    if (cache != nullptr) {
      return cache;
    }
  }
  return nullptr;
}

BlockTemplate Core::restoreBlockTemplate(IBlockchainCache* blockchainCache, uint32_t blockIndex) const {
  RawBlock rawBlock = blockchainCache->getBlockByIndex(blockIndex);

  BlockTemplate block;
  if (!fromBinaryArray(block, rawBlock.blockTemplate)) {
    throw std::runtime_error("Coulnd't deserialize BlockTemplate");
  }

  return block;
}

std::vector<Crypto::Hash> Core::doBuildSparseChain(const Crypto::Hash& blockHash) const {
  IBlockchainCache* chain = findSegmentContainingBlock(blockHash);

  uint32_t blockIndex = chain->getBlockIndex(blockHash);

  // TODO reserve ceil(log(blockIndex))
  std::vector<Crypto::Hash> sparseChain;
  sparseChain.push_back(blockHash);

  for (uint32_t i = 1; i < blockIndex; i *= 2) {
    sparseChain.push_back(chain->getBlockHash(blockIndex - i));
  }

  auto genesisBlockHash = chain->getBlockHash(0);
  if (sparseChain[0] != genesisBlockHash) {
    sparseChain.push_back(genesisBlockHash);
  }

  return sparseChain;
}

RawBlock Core::getRawBlock(IBlockchainCache* segment, uint32_t blockIndex) const {
  assert(blockIndex >= segment->getStartBlockIndex() && blockIndex <= segment->getTopBlockIndex());

  return segment->getBlockByIndex(blockIndex);
}

size_t Core::pushBlockHashes(uint32_t startIndex, uint32_t fullOffset, size_t maxItemsCount,
                             std::vector<BlockShortInfo>& entries) const {
  assert(fullOffset >= startIndex);

  uint32_t itemsCount = std::min(fullOffset - startIndex, static_cast<uint32_t>(maxItemsCount));
  if (itemsCount == 0) {
    return 0;
  }

  std::vector<Crypto::Hash> blockIds = getBlockHashes(startIndex, itemsCount);

  entries.reserve(entries.size() + blockIds.size());
  for (auto& blockHash : blockIds) {
    BlockShortInfo entry;
    entry.block_hash = std::move(blockHash);
    entries.emplace_back(std::move(entry));
  }

  return blockIds.size();
}

// TODO: decompose these three methods
size_t Core::pushBlockHashes(uint32_t startIndex, uint32_t fullOffset, size_t maxItemsCount,
                             std::vector<BlockDetails>& entries) const {
  assert(fullOffset >= startIndex);

  uint32_t itemsCount = std::min(fullOffset - startIndex, static_cast<uint32_t>(maxItemsCount));

  if (itemsCount == 0) {
    return 0;
  }

  std::vector<Crypto::Hash> blockIds = getBlockHashes(startIndex, itemsCount);

  entries.reserve(entries.size() + blockIds.size());
  for (auto& blockHash : blockIds) {
    BlockDetails entry;
    entry.hash = std::move(blockHash);
    entries.emplace_back(std::move(entry));
  }

  return blockIds.size();
}

// TODO: decompose these three methods
size_t Core::pushBlockHashes(uint32_t startIndex, uint32_t fullOffset, size_t maxItemsCount,
                             std::vector<BlockFullInfo>& entries) const {
  assert(fullOffset >= startIndex);

  uint32_t itemsCount = std::min(fullOffset - startIndex, static_cast<uint32_t>(maxItemsCount));
  if (itemsCount == 0) {
    return 0;
  }

  std::vector<Crypto::Hash> blockIds = getBlockHashes(startIndex, itemsCount);

  entries.reserve(entries.size() + blockIds.size());
  for (auto& blockHash : blockIds) {
    BlockFullInfo entry;
    entry.block_id = std::move(blockHash);
    entries.emplace_back(std::move(entry));
  }

  return blockIds.size();
}

void Core::fillQueryBlockFullInfo(uint32_t fullOffset, uint32_t currentIndex, size_t maxItemsCount,
                                  std::vector<BlockFullInfo>& entries) const {
  assert(currentIndex >= fullOffset);

  uint32_t fullBlocksCount =
      static_cast<uint32_t>(std::min(static_cast<uint32_t>(maxItemsCount), currentIndex - fullOffset));
  entries.reserve(entries.size() + fullBlocksCount);

  for (uint32_t blockIndex = fullOffset; blockIndex < fullOffset + fullBlocksCount; ++blockIndex) {
    IBlockchainCache* segment = findMainChainSegmentContainingBlock(blockIndex);

    BlockFullInfo blockFullInfo;
    blockFullInfo.block_id = segment->getBlockHash(blockIndex);
    static_cast<RawBlock&>(blockFullInfo) = getRawBlock(segment, blockIndex);

    entries.emplace_back(std::move(blockFullInfo));
  }
}

void Core::fillQueryBlockShortInfo(uint32_t fullOffset, uint32_t currentIndex, size_t maxItemsCount,
                                   std::vector<BlockShortInfo>& entries) const {
  assert(currentIndex >= fullOffset);

  uint32_t fullBlocksCount =
      static_cast<uint32_t>(std::min(static_cast<uint32_t>(maxItemsCount), currentIndex - fullOffset + 1));
  entries.reserve(entries.size() + fullBlocksCount);

  for (uint32_t blockIndex = fullOffset; blockIndex < fullOffset + fullBlocksCount; ++blockIndex) {
    IBlockchainCache* segment = findMainChainSegmentContainingBlock(blockIndex);
    RawBlock rawBlock = getRawBlock(segment, blockIndex);

    BlockShortInfo blockShortInfo;
    blockShortInfo.block = std::move(rawBlock.blockTemplate);
    blockShortInfo.block_hash = segment->getBlockHash(blockIndex);

    blockShortInfo.transaction_prefixes.reserve(rawBlock.transactions.size());
    for (auto& rawTransaction : rawBlock.transactions) {
      auto transaction = fromBinaryArray<Transaction>(rawTransaction);
      TransactionPrefixInfo prefixInfo;
      // TODO: is there faster way to get hash without calculation?
      prefixInfo.hash = transaction.hash();
      prefixInfo.prefix = std::move(static_cast<TransactionPrefix&>(transaction));
      blockShortInfo.transaction_prefixes.emplace_back(std::move(prefixInfo));
    }

    entries.emplace_back(std::move(blockShortInfo));
  }
}

bool Core::fillQueryBlockDetails(uint32_t fullOffset, uint32_t currentIndex, size_t maxItemsCount,
                                 std::vector<BlockDetails>& entries) const {
  assert(currentIndex >= fullOffset);

  uint32_t fullBlocksCount =
      static_cast<uint32_t>(std::min(static_cast<uint32_t>(maxItemsCount), currentIndex - fullOffset + 1));
  entries.reserve(entries.size() + fullBlocksCount);

  for (uint32_t blockIndex = fullOffset; blockIndex < fullOffset + fullBlocksCount; ++blockIndex) {
    IBlockchainCache* segment = findMainChainSegmentContainingBlock(blockIndex);
    XI_RETURN_EC_IF(segment == nullptr, false);
    Crypto::Hash blockHash = segment->getBlockHash(blockIndex);
    auto block = getBlockDetails(blockHash);
    XI_RETURN_EC_IF_NOT(block, false);
    entries.emplace_back(std::move(*block));
  }
  return true;
}

void Core::getTransactionPoolDifference(const std::vector<Crypto::Hash>& knownHashes,
                                        std::vector<Crypto::Hash>& newTransactions,
                                        std::vector<Crypto::Hash>& deletedTransactions) const {
  throwIfNotInitialized();
  auto t = m_transactionPool->getTransactionHashes();

  std::unordered_set<Crypto::Hash> poolTransactions(t.begin(), t.end());
  std::unordered_set<Crypto::Hash> knownTransactions(knownHashes.begin(), knownHashes.end());

  for (auto it = poolTransactions.begin(), end = poolTransactions.end(); it != end;) {
    auto knownTransactionIt = knownTransactions.find(*it);
    if (knownTransactionIt != knownTransactions.end()) {
      knownTransactions.erase(knownTransactionIt);
      it = poolTransactions.erase(it);
    } else {
      ++it;
    }
  }

  newTransactions.assign(poolTransactions.begin(), poolTransactions.end());
  deletedTransactions.assign(knownTransactions.begin(), knownTransactions.end());
}

BlockVersion Core::getBlockVersionForIndex(uint32_t height) const { return m_upgradeManager->getBlockVersion(height); }

size_t Core::calculateCumulativeBlocksizeLimit(uint32_t height) const {
  const auto nextBlockVersion = getBlockVersionForIndex(height);
  size_t nextBlockGrantedFullRewardZone = m_currency.blockGrantedFullRewardZoneByBlockVersion(nextBlockVersion);

  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());
  auto sizes = chainsLeaves[0]->getLastBlocksSizes(m_currency.rewardBlocksWindowByBlockVersion(nextBlockVersion),
                                                   height - 1, UseGenesis{true});
  uint64_t median = Common::medianValue(sizes);
  if (median <= nextBlockGrantedFullRewardZone) {
    median = nextBlockGrantedFullRewardZone;
  }

  return median * 2;
}

void Core::fillBlockTemplate(BlockTemplate& block, uint32_t index, size_t fullRewardZone, size_t maxCumulativeSize,
                             size_t& transactionsSize, uint64_t& fee) const {
  throwIfNotInitialized();
  transactionsSize = 0;
  fee = 0;

  // What would be our reward without any transaction used
  const auto generatedCoins = chainsLeaves[0]->getAlreadyGeneratedCoins(index - 1);
  int64_t emissionChange = 0;
  uint64_t currentReward = 0;
  m_currency.getBlockReward(block.version, fullRewardZone, 0, generatedCoins, fee, currentReward, emissionChange);

  size_t cumulativeSize = m_currency.minerTxBlobReservedSize();

  const EligibleIndex blockIndex{index, block.timestamp};
  // We assume eligible transactions are ordered, such that the first transactions are most profitible. Using this
  // we now implement a greedy search algorithm to fill the transaction.
  std::vector<CachedTransaction> poolTransactions = m_transactionPool->eligiblePoolTransactions(blockIndex);

  // Assumption Reward(iter) >= Reward(iter++), TransactionPriorityComperator
  for (auto iter = poolTransactions.begin(); iter != poolTransactions.end(); ++iter) {
    // The transation binary array is included in the body and the hash in the header, both are part of the block size
    const size_t iSize = iter->getTransactionBinaryArray().size();
    if (cumulativeSize + iSize > maxCumulativeSize) {
      break;
    }
    const uint64_t iFee = iter->getTransactionFee();
    if (cumulativeSize + iSize > fullRewardZone) {
      // Lets reevaluate if its still plausible to add transactions.
      if (iFee == 0) {
        break;
      }

      uint64_t newReward = 0;
      m_currency.getBlockReward(block.version, fullRewardZone, cumulativeSize + iSize, generatedCoins, fee + iFee,
                                newReward, emissionChange);
      if (newReward < currentReward) {
        break;
      }
    }

    // If we land here all checks have passed and we can add the transaction
    fee += iFee;
    cumulativeSize += iSize;
    block.transactionHashes.emplace_back(iter->getTransactionHash());
    transactionsSize += iter->getTransactionBinaryArray().size();
  }
}

void Core::deleteAlternativeChains() {
  while (chainsLeaves.size() > 1) {
    deleteLeaf(1);
  }
}

void Core::deleteLeaf(size_t leafIndex) {
  assert(leafIndex < chainsLeaves.size());

  IBlockchainCache* leaf = chainsLeaves[leafIndex];

  IBlockchainCache* parent = leaf->getParent();
  if (parent != nullptr) {
    bool r = parent->deleteChild(leaf);
    if (r) {
    }
    assert(r);
  }

  auto segmentIt =
      std::find_if(chainsStorage.begin(), chainsStorage.end(),
                   [&leaf](const std::shared_ptr<IBlockchainCache>& segment) { return segment.get() == leaf; });

  assert(segmentIt != chainsStorage.end());

  if (leafIndex != 0) {
    if (parent->getChildCount() == 0) {
      chainsLeaves.push_back(parent);
    }

    chainsLeaves.erase(chainsLeaves.begin() + leafIndex);
  } else {
    if (parent != nullptr) {
      chainsLeaves[0] = parent;
    } else {
      chainsLeaves.erase(chainsLeaves.begin());
    }
  }

  chainsStorage.erase(segmentIt);
}

void Core::mergeMainChainSegments() {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());

  std::vector<IBlockchainCache*> chain;
  IBlockchainCache* segment = chainsLeaves[0];
  while (segment != nullptr) {
    chain.push_back(segment);
    segment = segment->getParent();
  }

  IBlockchainCache* rootSegment = chain.back();
  for (auto it = ++chain.rbegin(); it != chain.rend(); ++it) {
    mergeSegments(rootSegment, *it);
  }

  auto rootIt = std::find_if(
      chainsStorage.begin(), chainsStorage.end(),
      [&rootSegment](const std::shared_ptr<IBlockchainCache>& segment) { return segment.get() == rootSegment; });

  assert(rootIt != chainsStorage.end());

  if (rootIt != chainsStorage.begin()) {
    *chainsStorage.begin() = std::move(*rootIt);
  }

  chainsStorage.erase(++chainsStorage.begin(), chainsStorage.end());
  chainsLeaves.clear();
  chainsLeaves.push_back(chainsStorage.begin()->get());
}

void Core::mergeSegments(IBlockchainCache* acceptingSegment, IBlockchainCache* segment) {
  assert(segment->getStartBlockIndex() == acceptingSegment->getStartBlockIndex() + acceptingSegment->getBlockCount());

  auto startIndex = segment->getStartBlockIndex();
  auto blockCount = segment->getBlockCount();
  for (auto blockIndex = startIndex; blockIndex < startIndex + blockCount; ++blockIndex) {
    PushedBlockInfo info = segment->getPushedBlockInfo(blockIndex);

    BlockTemplate block;
    if (!fromBinaryArray(block, info.rawBlock.blockTemplate)) {
      logger(Logging::WARNING) << "mergeSegments error: Couldn't deserialize block";
      throw std::runtime_error("Couldn't deserialize block");
    }

    std::vector<CachedTransaction> transactions;
    if (!Utils::restoreCachedTransactions(info.rawBlock.transactions, transactions)) {
      logger(Logging::WARNING) << "mergeSegments error: Couldn't deserialize transactions";
      throw std::runtime_error("Couldn't deserialize transactions");
    }

    acceptingSegment->pushBlock(CachedBlock(block), transactions, info.validatorState, info.blockSize,
                                info.generatedCoins, info.blockDifficulty, std::move(info.rawBlock));
  }
}

std::optional<BlockDetails> Core::getBlockDetails(const uint32_t blockHeight) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  IBlockchainCache* segment = findSegmentContainingBlock(blockHeight);
  if (segment == nullptr) {
    return std::nullopt;
  }

  return getBlockDetails(segment->getBlockHash(blockHeight));
}

std::optional<BlockDetails> Core::getBlockDetails(const Crypto::Hash& blockHash) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  IBlockchainCache* segment = findSegmentContainingBlock(blockHash);
  if (segment == nullptr) {
    return std::nullopt;
  }

  uint32_t blockIndex = segment->getBlockIndex(blockHash);
  BlockTemplate blockTemplate = restoreBlockTemplate(segment, blockIndex);

  BlockDetails blockDetails;
  blockDetails.version = blockTemplate.version;
  blockDetails.upgradeVote = blockTemplate.upgradeVote;
  blockDetails.timestamp = blockTemplate.timestamp;
  blockDetails.prevBlockHash = blockTemplate.previousBlockHash;
  blockDetails.nonce = blockTemplate.nonce;
  blockDetails.hash = blockHash;

  blockDetails.reward = 0;
  for (const TransactionOutput& out : blockTemplate.baseTransaction.outputs) {
    blockDetails.reward += out.amount;
  }
  blockDetails.staticReward = m_currency.staticRewardAmountForBlockVersion(blockDetails.version);

  blockDetails.height = BlockHeight::fromIndex(blockIndex);
  blockDetails.isAlternative = mainChainSet.count(segment) == 0;

  blockDetails.difficulty = getBlockDifficulty(blockIndex);

  std::vector<uint64_t> sizes = segment->getLastBlocksSizes(1, blockIndex, UseGenesis{true});
  assert(sizes.size() == 1);
  blockDetails.transactionsCumulativeSize = sizes.front();

  uint64_t blockBlobSize = segment->getCurrentBlockSize(blockIndex);
  uint64_t coinbaseTransactionSize = blockTemplate.baseTransaction.binarySize();
  blockDetails.blockSize = blockBlobSize + blockDetails.transactionsCumulativeSize - coinbaseTransactionSize;

  blockDetails.alreadyGeneratedCoins = segment->getAlreadyGeneratedCoins(blockIndex);
  blockDetails.alreadyGeneratedTransactions = segment->getAlreadyGeneratedTransactions(blockIndex);

  uint64_t prevBlockGeneratedCoins = 0;
  blockDetails.sizeMedian = 0;
  auto lastBlocksSizes = segment->getLastBlocksSizes(m_currency.rewardBlocksWindowByBlockVersion(blockDetails.version),
                                                     blockIndex, UseGenesis{true});
  blockDetails.sizeMedian = Common::medianValue(lastBlocksSizes);
  prevBlockGeneratedCoins = segment->getAlreadyGeneratedCoins(blockIndex);

  int64_t emissionChange = 0;
  bool result = m_currency.getBlockReward(blockDetails.version, blockDetails.sizeMedian, 0, prevBlockGeneratedCoins, 0,
                                          blockDetails.baseReward, emissionChange);
  if (result) {
  }
  assert(result);

  uint64_t currentReward = 0;
  result =
      m_currency.getBlockReward(blockDetails.version, blockDetails.sizeMedian, blockDetails.transactionsCumulativeSize,
                                prevBlockGeneratedCoins, 0, currentReward, emissionChange);
  assert(result);

  if (blockDetails.baseReward == 0 && currentReward == 0) {
    blockDetails.penalty = static_cast<double>(0);
  } else {
    assert(blockDetails.baseReward >= currentReward);
    blockDetails.penalty =
        static_cast<double>(blockDetails.baseReward - currentReward) / static_cast<double>(blockDetails.baseReward);
  }

  blockDetails.transactions.reserve(blockTemplate.transactionHashes.size() + 2);

  {
    auto staticReward = m_currency.constructStaticRewardTx(blockTemplate).takeOrThrow();
    if (staticReward.has_value()) {
      CachedTransaction cachedStaticRewardTx(std::move(*staticReward));
      blockDetails.transactions.push_back(
          getTransactionDetails(cachedStaticRewardTx.getTransactionHash(), segment, false));
    }
  }

  CachedTransaction cachedBaseTx(std::move(blockTemplate.baseTransaction));
  blockDetails.transactions.push_back(getTransactionDetails(cachedBaseTx.getTransactionHash(), segment, false));

  blockDetails.totalFeeAmount = 0;
  for (const Crypto::Hash& transactionHash : blockTemplate.transactionHashes) {
    blockDetails.transactions.push_back(getTransactionDetails(transactionHash, segment, false));
    blockDetails.totalFeeAmount += blockDetails.transactions.back().fee;
  }

  return std::make_optional<BlockDetails>(std::move(blockDetails));
}

TransactionDetails Core::getTransactionDetails(const Crypto::Hash& transactionHash) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  IBlockchainCache* segment = findSegmentContainingTransaction(transactionHash);
  bool foundInPool = m_transactionPool->checkIfTransactionPresent(transactionHash);
  if (segment == nullptr && !foundInPool) {
    throw std::runtime_error("Requested transaction wasn't found.");
  }

  return getTransactionDetails(transactionHash, segment, foundInPool);
}

TransactionDetails Core::getTransactionDetails(const Crypto::Hash& transactionHash, IBlockchainCache* segment,
                                               bool foundInPool) const {
  throwIfNotInitialized();
  assert((segment != nullptr) != foundInPool);
  if (segment == nullptr) {
    segment = chainsLeaves[0];
  }

  std::unique_ptr<ITransactionBuilder> transaction;
  Transaction rawTransaction;
  TransactionDetails transactionDetails;
  if (!foundInPool) {
    std::vector<Crypto::Hash> transactionsHashes;
    std::vector<BinaryArray> rawTransactions;
    std::vector<Crypto::Hash> missedTransactionsHashes;
    transactionsHashes.push_back(transactionHash);

    for (auto iSegment = segment; iSegment != nullptr; iSegment = iSegment->getParent()) {
      missedTransactionsHashes.clear();
      iSegment->getRawTransactions(transactionsHashes, rawTransactions, missedTransactionsHashes);
      if (missedTransactionsHashes.empty()) {
        break;
      }
    }
    Xi::exceptional_if<Xi::NotFoundError>((!missedTransactionsHashes.empty()) || rawTransactions.empty(),
                                          transactionHash.toString());
    assert(rawTransactions.size() == 1);

    std::vector<CachedTransaction> transactions;
    Utils::restoreCachedTransactions(rawTransactions, transactions);
    assert(transactions.size() == 1);

    transactionDetails.inBlockchain = true;
    const auto blockIndex = segment->getBlockIndexContainingTx(transactionHash);
    transactionDetails.blockHeight = BlockHeight::fromIndex(blockIndex);
    transactionDetails.blockHash = segment->getBlockHash(transactionDetails.blockHeight.toIndex());

    auto timestamps = segment->getLastTimestamps(1, blockIndex, addGenesisBlock);
    assert(timestamps.size() == 1);
    transactionDetails.timestamp = timestamps.back();

    transactionDetails.size = transactions.back().getTransactionBinaryArray().size();
    transactionDetails.fee = transactions.back().getTransactionFee();

    rawTransaction = transactions.back().getTransaction();
    transaction = createTransaction(rawTransaction);
  } else {
    transactionDetails.inBlockchain = false;
    transactionDetails.timestamp = m_transactionPool->getTransactionReceiveTime(transactionHash);

    transactionDetails.size = m_transactionPool->getTransaction(transactionHash).getTransactionBinaryArray().size();
    transactionDetails.fee = m_transactionPool->getTransaction(transactionHash).getTransactionFee();

    rawTransaction = m_transactionPool->getTransaction(transactionHash).getTransaction();
    transaction = createTransaction(rawTransaction);
  }

  transactionDetails.hash = transactionHash;
  transactionDetails.unlockTime = transaction->getUnlockTime();

  transactionDetails.totalOutputsAmount = transaction->getOutputTotalAmount();
  transactionDetails.totalInputsAmount = transaction->getInputTotalAmount();

  transactionDetails.mixin = 0;
  for (size_t i = 0; i < transaction->getInputCount(); ++i) {
    if (transaction->getInputType(i) != TransactionTypes::InputType::Key) {
      continue;
    }

    KeyInput input;
    transaction->getInput(i, input);
    uint64_t currentMixin = input.outputIndices.size();
    if (currentMixin > transactionDetails.mixin) {
      transactionDetails.mixin = currentMixin;
    }
  }

  transactionDetails.paymentId = boost::value_initialized<PaymentId>();
  if (transaction->getPaymentId(transactionDetails.paymentId)) {
    transactionDetails.hasPaymentId = true;
  }
  transactionDetails.extra.publicKey = transaction->getTransactionPublicKey();
  transaction->getExtraNonce(transactionDetails.extra.nonce);

  transactionDetails.signatures = rawTransaction.signatures;

  transactionDetails.inputs.reserve(transaction->getInputCount());
  for (size_t i = 0; i < transaction->getInputCount(); ++i) {
    TransactionInputDetails txInDetails;

    if (transaction->getInputType(i) == TransactionTypes::InputType::Generating) {
      BaseInputDetails baseDetails;
      baseDetails.input = std::get<BaseInput>(rawTransaction.inputs[i]);
      baseDetails.amount = transaction->getOutputTotalAmount();
      txInDetails = baseDetails;
    } else if (transaction->getInputType(i) == TransactionTypes::InputType::Key) {
      KeyInputDetails txInToKeyDetails;
      txInToKeyDetails.input = std::get<KeyInput>(rawTransaction.inputs[i]);
      std::vector<std::pair<Crypto::Hash, size_t>> outputReferences;
      outputReferences.reserve(txInToKeyDetails.input.outputIndices.size());
      std::vector<uint32_t> globalIndexes = relativeOutputOffsetsToAbsolute(txInToKeyDetails.input.outputIndices);
      ExtractOutputKeysResult result = segment->extractKeyOtputReferences(
          txInToKeyDetails.input.amount, {globalIndexes.data(), globalIndexes.size()}, outputReferences);
      assert(result == ExtractOutputKeysResult::SUCCESS);
      (void)result;
      assert(txInToKeyDetails.input.outputIndices.size() == outputReferences.size());

      txInToKeyDetails.mixin = txInToKeyDetails.input.outputIndices.size();
      txInToKeyDetails.output.number = outputReferences.back().second;
      txInToKeyDetails.output.transactionHash = outputReferences.back().first;
      txInDetails = txInToKeyDetails;
    }

    transactionDetails.inputs.push_back(std::move(txInDetails));
  }

  transactionDetails.outputs.reserve(transaction->getOutputCount());
  std::vector<uint32_t> globalIndexes;
  globalIndexes.reserve(transaction->getOutputCount());
  if (!transactionDetails.inBlockchain || getTransactionGlobalIndexes(transactionDetails.hash, globalIndexes)) {
    globalIndexes.clear();
    globalIndexes.resize(transaction->getOutputCount(), 0);
  }

  assert(transaction->getOutputCount() == globalIndexes.size());
  for (size_t i = 0; i < transaction->getOutputCount(); ++i) {
    TransactionOutputDetails txOutDetails;
    txOutDetails.output = rawTransaction.outputs[i];
    txOutDetails.globalIndex = globalIndexes[i];
    transactionDetails.outputs.push_back(std::move(txOutDetails));
  }

  return transactionDetails;
}

std::vector<Crypto::Hash> Core::getAlternativeBlockHashesByIndex(uint32_t blockIndex) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  std::vector<Crypto::Hash> alternativeBlockHashes;
  for (size_t chain = 1; chain < chainsLeaves.size(); ++chain) {
    IBlockchainCache* segment = chainsLeaves[chain];
    if (segment->getTopBlockIndex() < blockIndex) {
      continue;
    }

    do {
      if (segment->getTopBlockIndex() - segment->getBlockCount() + 1 <= blockIndex) {
        alternativeBlockHashes.push_back(segment->getBlockHash(blockIndex));
        break;
      } else if (segment->getTopBlockIndex() - segment->getBlockCount() - 1 > blockIndex) {
        segment = segment->getParent();
        assert(segment != nullptr);
      }
    } while (mainChainSet.count(segment) == 0);
  }
  return alternativeBlockHashes;
}

std::vector<Crypto::Hash> Core::getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  logger(Logging::DEBUGGING) << "getBlockHashesByTimestamps request with timestamp " << timestampBegin
                             << " and seconds count " << secondsCount;

  auto mainChain = chainsLeaves[0];

  if (timestampBegin + static_cast<uint64_t>(secondsCount) < timestampBegin) {
    logger(Logging::WARNING) << "Timestamp overflow occured. Timestamp begin: " << timestampBegin
                             << ", timestamp end: " << (timestampBegin + static_cast<uint64_t>(secondsCount));

    throw std::runtime_error("Timestamp overflow");
  }

  return mainChain->getBlockHashesByTimestamps(timestampBegin, secondsCount);
}

std::vector<Crypto::Hash> Core::getTransactionHashesByPaymentId(const PaymentId& paymentId) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  logger(Logging::DEBUGGING) << "getTransactionHashesByPaymentId request with paymentId " << paymentId;

  auto mainChain = chainsLeaves[0];

  std::vector<Crypto::Hash> hashes = mainChain->getTransactionHashesByPaymentId(paymentId);
  std::vector<Crypto::Hash> poolHashes = m_transactionPool->getTransactionHashesByPaymentId(paymentId);

  hashes.reserve(hashes.size() + poolHashes.size());
  std::move(poolHashes.begin(), poolHashes.end(), std::back_inserter(hashes));

  return hashes;
}

SegmentReferenceVector<BlockHash> Core::queryBlockSegments(ConstBlockHashSpan hashes) const {
  using segment_reference = SegmentReference<BlockHash>;

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  std::map<const IBlockchainCache*, segment_reference> references{};
  std::set<BlockHash> processed{};
  for (const auto& blockHash : hashes) {
    if (!processed.insert(blockHash).second) {
      continue;
    }
    bool isMainChain = false;
    const auto segment = findSegmentContainingBlock(blockHash, &isMainChain);
    if (segment != nullptr) {
      auto search = references.find(segment);
      if (search == references.end()) {
        search =
            references
                .emplace(std::piecewise_construct, std::forward_as_tuple(segment),
                         std::forward_as_tuple(segment->shared_from_this(),
                                               isMainChain ? BlockSource::MainChain : BlockSource::AlternativeChain))
                .first;
      }
      search->second.content.push_back(blockHash);
    }
  }

  SegmentReferenceVector<BlockHash> reval{};
  reval.reserve(references.size());
  for (auto& ref : references) {
    reval.emplace_back(std::move(ref.second));
  }
  return reval;
}

SegmentReferenceVector<BlockHeight> Core::queryBlockSegments(ConstBlockHeightSpan heights) const {
  using segment_reference = SegmentReference<BlockHeight>;

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  std::map<const IBlockchainCache*, segment_reference> references{};
  std::set<BlockHeight> processed{};
  for (const auto& blockHeight : heights) {
    if (blockHeight.isNull()) {
      continue;
    } else if (!processed.insert(blockHeight).second) {
      continue;
    }

    bool isMainChain = false;
    const auto segment = findSegmentContainingBlock(blockHeight.toIndex(), &isMainChain);
    if (segment != nullptr) {
      auto search = references.find(segment);
      if (search == references.end()) {
        search =
            references
                .emplace(std::piecewise_construct, std::forward_as_tuple(segment),
                         std::forward_as_tuple(segment->shared_from_this(),
                                               isMainChain ? BlockSource::MainChain : BlockSource::AlternativeChain))
                .first;
      }
      search->second.content.push_back(blockHeight);
    }
  }

  SegmentReferenceVector<BlockHeight> reval{};
  reval.reserve(references.size());
  for (auto& ref : references) {
    reval.emplace_back(std::move(ref.second));
  }
  return reval;
}

SegmentReferenceVector<TransactionHash> Core::queryTransactionSegments(ConstTransactionHashSpan hashes) const {
  using segment_reference = SegmentReference<TransactionHash>;

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  std::map<const IBlockchainCache*, segment_reference> references{};
  std::set<BlockHash> processed{};
  for (const auto& blockHash : hashes) {
    if (!processed.insert(blockHash).second) {
      continue;
    }
    bool isMainChain = false;
    const auto segment = findSegmentContainingTransaction(blockHash, &isMainChain);
    if (segment != nullptr) {
      auto search = references.find(segment);
      if (search == references.end()) {
        search =
            references
                .emplace(std::piecewise_construct, std::forward_as_tuple(segment),
                         std::forward_as_tuple(segment->shared_from_this(),
                                               isMainChain ? BlockSource::MainChain : BlockSource::AlternativeChain))
                .first;
      }
      search->second.content.push_back(blockHash);
    }
  }

  SegmentReferenceVector<BlockHash> reval{};
  reval.reserve(references.size());
  for (auto& ref : references) {
    reval.emplace_back(std::move(ref.second));
  }
  return reval;
}

void Core::throwIfNotInitialized() const {
  if (!initialized) {
    throw std::system_error(make_error_code(error::CoreErrorCode::NOT_INITIALIZED));
  }
}

IBlockchainCache* Core::findSegmentContainingTransaction(const Crypto::Hash& transactionHash, bool* isMainChain) const {
  assert(!chainsLeaves.empty());
  assert(!chainsStorage.empty());

  IBlockchainCache* segment = chainsLeaves[0];
  assert(segment != nullptr);

  // find in main chain
  do {
    if (segment->hasTransaction(transactionHash)) {
      if (isMainChain != nullptr) {
        *isMainChain = true;
      }
      return segment;
    }

    segment = segment->getParent();
  } while (segment != nullptr);

  // find in alternative chains
  std::set<IBlockchainCache*> alternativesPropagated{};
  for (size_t chain = 1; chain < chainsLeaves.size(); ++chain) {
    segment = chainsLeaves[chain];

    while (mainChainSet.count(segment) == 0 && alternativesPropagated.count(segment) == 0) {
      if (segment->hasTransaction(transactionHash)) {
        if (isMainChain != nullptr) {
          *isMainChain = false;
        }
        return segment;
      }

      alternativesPropagated.insert(segment);
      segment = segment->getParent();
    }
  }

  return nullptr;
}

bool Core::hasTransaction(const Crypto::Hash& transactionHash) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  return findSegmentContainingTransaction(transactionHash) != nullptr ||
         m_transactionPool->checkIfTransactionPresent(transactionHash);
}

void Core::transactionPoolCleaningProcedure() {
  throwIfNotInitialized();
  System::Timer timer(dispatcher);

  try {
    for (;;) {
      timer.sleep(OUTDATED_TRANSACTION_POLLING_INTERVAL);

      auto deletedTransactions = static_cast<TransactionPoolCleanWrapper*>(m_transactionPool.get())->clean();
      notifyObservers(
          makeDelTransactionMessage(std::move(deletedTransactions), Messages::DeleteTransaction::Reason::Outdated));
    }
  } catch (System::InterruptedException&) {
    logger(Logging::DEBUGGING) << "transactionPoolCleaningProcedure has been interrupted";
  } catch (std::exception& e) {
    logger(Logging::ERROR) << "Error occurred while cleaning transactions pool: " << e.what();
  }
}

void Core::updateBlockMedianSize() {
  auto mainChain = chainsLeaves[0];

  size_t nextBlockGrantedFullRewardZone = m_currency.blockGrantedFullRewardZoneByBlockVersion(
      m_upgradeManager->getBlockVersion(mainChain->getTopBlockIndex() + 1));

  auto lastBlockSizes = mainChain->getLastBlocksSizes(m_currency.rewardBlocksWindowByBlockVersion(
      m_upgradeManager->getBlockVersion(mainChain->getTopBlockIndex() + 1)));

  blockMedianSize =
      std::max(Common::medianValue(lastBlockSizes), static_cast<uint64_t>(nextBlockGrantedFullRewardZone));
}

BlockHeight Core::get_current_blockchain_height() const {
  XI_CONCURRENT_RLOCK(m_access);
  return BlockHeight::fromIndex(getTopBlockIndex());
}

std::time_t Core::getStartTime() const { return start_time; }

const ITransactionPool& Core::transactionPool() const { return *m_transactionPool; }
ITransactionPool& Core::transactionPool() { return *m_transactionPool; }

}  // namespace CryptoNote
