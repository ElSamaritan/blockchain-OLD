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

#include <Xi/Global.h>
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
#include "CryptoNoteCore/Transactions/TransactionExtra.h"
#include "CryptoNoteCore/Transactions/TransactionPool.h"
#include "CryptoNoteCore/Transactions/TransactionPoolCleaner.h"
#include "CryptoNoteCore/Transactions/TransactionApi.h"
#include "CryptoNoteCore/Transactions/Mixins.h"
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
  if (!fromBinaryArray(blockTemplate, block.block)) {
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
    if (input.type() == typeid(KeyInput)) {
      const KeyInput& in = boost::get<KeyInput>(input);
      bool r = spentOutputs.spentKeyImages.insert(in.keyImage).second;
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
  const uint8_t majorVersion = cachedBlock.getBlock().majorVersion;
  auto alreadyGeneratedCoins = segment.getAlreadyGeneratedCoins(previousBlockIndex);
  auto lastBlocksSizes = segment.getLastBlocksSizes(currency.rewardBlocksWindowByBlockVersion(majorVersion),
                                                    previousBlockIndex, addGenesisBlock);
  auto blocksSizeMedian = Common::medianValue(lastBlocksSizes);
  if (!currency.getBlockReward(majorVersion, blocksSizeMedian, cumulativeSize, alreadyGeneratedCoins, cumulativeFee,
                               reward, emissionChange)) {
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

const std::chrono::seconds OUTDATED_TRANSACTION_POLLING_INTERVAL = std::chrono::seconds(60);

}  // namespace

Core::Core(const Currency& currency, Logging::ILogger& logger, Checkpoints&& checkpoints,
           System::Dispatcher& dispatcher, std::unique_ptr<IBlockchainCacheFactory>&& blockchainCacheFactory,
           std::unique_ptr<IMainChainStorage>&& mainchainStorage)
    : m_currency(currency),
      dispatcher(dispatcher),
      contextGroup(dispatcher),
      logger(logger, "Core"),
      checkpoints(std::move(checkpoints)),
      m_upgradeManager(new UpgradeManager()),
      blockchainCacheFactory(std::move(blockchainCacheFactory)),
      mainChainStorage(std::move(mainchainStorage)),
      initialized(false),
      m_access{} {
  for (auto version : Xi::Config::BlockVersion::versions())
    m_upgradeManager->addMajorBlockVersion(version, currency.upgradeHeight(version));
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
    case Reason::BlockMajorVersionUpgrade:
    case Reason::Forced:
      notifyObservers(makeDelTransactionMessage({hash}, Messages::DeleteTransaction::Reason::NotActual));
      break;
    case Reason::AddedToMainChain:
      /* swallow handled by addBlockRoutine */
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

bool Core::hasBlock(const Crypto::Hash& blockHash) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  return findSegmentContainingBlock(blockHash) != nullptr;
}

BlockTemplate Core::getBlockByIndex(uint32_t index) const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());
  assert(index <= getTopBlockIndex());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  IBlockchainCache* segment = findMainChainSegmentContainingBlock(index);
  assert(segment != nullptr);

  return restoreBlockTemplate(segment, index);
}

BlockTemplate Core::getBlockByHash(const Crypto::Hash& blockHash) const {
  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());

  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  IBlockchainCache* segment =
      findMainChainSegmentContainingBlock(blockHash);  // TODO should it be requested from the main chain?
  if (segment == nullptr) {
    throw std::runtime_error("Requested hash wasn't found in main blockchain");
  }

  uint32_t blockIndex = segment->getBlockIndex(blockHash);

  return restoreBlockTemplate(segment, blockIndex);
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

    fillQueryBlockDetails(fullOffset, currentIndex, Xi::Config::Network::blocksSynchronizationBatchSize(), entries);

    return true;
  } catch (std::exception& e) {
    logger(Logging::ERROR) << "Failed to query blocks: " << e.what();
    return false;
  }
}

void Core::getTransactions(const std::vector<Crypto::Hash>& transactionHashes, std::vector<BinaryArray>& transactions,
                           std::vector<Crypto::Hash>& missedHashes) const {
  XI_CONCURRENT_RLOCK(m_access);

  assert(!chainsLeaves.empty());
  assert(!chainsStorage.empty());
  throwIfNotInitialized();

  IBlockchainCache* segment = chainsLeaves[0];
  assert(segment != nullptr);

  if (transactions.size() < transactionHashes.size()) {
    transactions.reserve(transactionHashes.size());
  }

  std::vector<Crypto::Hash> leftTransactions = transactionHashes;

  // find in main chain
  do {
    std::vector<Crypto::Hash> missedTransactions;
    segment->getRawTransactions(leftTransactions, transactions, missedTransactions);

    leftTransactions = std::move(missedTransactions);
    segment = segment->getParent();
  } while (segment != nullptr && !leftTransactions.empty());

  if (leftTransactions.empty()) {
    return;
  }

  // find in alternative chains
  for (size_t chain = 1; chain < chainsLeaves.size(); ++chain) {
    segment = chainsLeaves[chain];

    while (segment != nullptr && !leftTransactions.empty()) {
      std::vector<Crypto::Hash> missedTransactions;
      segment->getRawTransactions(leftTransactions, transactions, missedTransactions);

      leftTransactions = std::move(missedTransactions);
      segment = segment->getParent();
    }
  }

  if (leftTransactions.empty()) {
    return;
  }

  for (const auto& hash : leftTransactions) {
    auto poolQuery = transactionPool().queryTransaction(hash);
    if (poolQuery.get() != nullptr) {
      transactions.emplace_back(poolQuery->transaction().getTransactionBinaryArray());
    } else {
      missedHashes.emplace_back(hash);
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
  IBlockchainCache* mainChain = chainsLeaves[0];

  uint32_t topBlockIndex = mainChain->getTopBlockIndex();

  uint8_t nextBlockMajorVersion = getBlockMajorVersionForHeight(topBlockIndex);

  size_t blocksCount =
      std::min(static_cast<size_t>(topBlockIndex), m_currency.difficultyBlocksCountByVersion(nextBlockMajorVersion));

  auto timestamps = mainChain->getLastTimestamps(blocksCount);
  auto difficulties = mainChain->getLastCumulativeDifficulties(blocksCount);

  return m_currency.nextDifficulty(nextBlockMajorVersion, topBlockIndex, timestamps, difficulties);
}

Xi::Result<std::vector<Crypto::Hash>> Core::findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds,
                                                                     size_t maxCount, uint32_t& totalBlockCount,
                                                                     uint32_t& startBlockIndex) const {
  assert(!remoteBlockIds.empty());
  assert(remoteBlockIds.back() == getBlockHashByIndex(0));
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  totalBlockCount = getTopBlockIndex() + 1;
  startBlockIndex = (uint32_t)-1;
  auto startIndexResult = findBlockchainSupplement(remoteBlockIds);
  if (startIndexResult.isError()) return startIndexResult.error();
  startBlockIndex = startIndexResult.value();

  return getBlockHashes(startBlockIndex, static_cast<uint32_t>(maxCount));
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

  if (hasBlock(cachedBlock.getBlockHash())) {
    logger(Logging::DEBUGGING) << "Block " << blockStr << " already exists";
    return error::AddBlockErrorCode::ALREADY_EXISTS;
  }

  const auto& blockTemplate = cachedBlock.getBlock();
  const auto& previousBlockHash = blockTemplate.previousBlockHash;

  if (rawBlock.transactions.size() != blockTemplate.transactionHashes.size()) {
    return error::BlockValidationError::TRANSACTION_INCONSISTENCY;
  }

  auto cache = findSegmentContainingBlock(previousBlockHash);
  if (cache == nullptr) {
    logger(Logging::DEBUGGING) << "Block " << blockStr << " rejected as orphaned";
    return error::AddBlockErrorCode::REJECTED_AS_ORPHANED;
  }

  std::vector<CachedTransaction> transactions;
  uint64_t cumulativeSize = 0;
  if (!extractTransactions(rawBlock.transactions, transactions, cumulativeSize)) {
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

  auto coinbaseTransactionSize = getObjectBinarySize(blockTemplate.baseTransaction);
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

  auto currentDifficulty = cache->getDifficultyForNextBlock(previousBlockIndex);
  if (currentDifficulty == 0) {
    logger(Logging::DEBUGGING) << "Block " << blockStr << " has difficulty overhead";
    return error::BlockValidationError::DIFFICULTY_OVERHEAD;
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

  bool success;
  std::string error;
  std::tie(success, error) = Mixins::validate(transactions, blockIndex);
  if (!success) {
    logger(Logging::DEBUGGING) << error;
    return error::TransactionValidationError::INVALID_MIXIN;
  }

  TransactionValidatorState validatorState;
  uint64_t cumulativeFee = 0;
  for (const auto& transaction : transactions) {
    uint64_t fee = 0;
    auto transactionValidationResult = validateTransaction(transaction, validatorState, cache, fee, previousBlockIndex);
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
      cache->getLastBlocksSizes(m_currency.rewardBlocksWindowByBlockVersion(cachedBlock.getBlock().majorVersion),
                                previousBlockIndex, addGenesisBlock);
  auto blocksSizeMedian = Common::medianValue(lastBlocksSizes);

  if (!m_currency.getBlockReward(cachedBlock.getBlock().majorVersion, blocksSizeMedian, cumulativeBlockSize,
                                 alreadyGeneratedCoins, cumulativeFee, reward, emissionChange)) {
    logger(Logging::DEBUGGING) << "Block " << blockStr << " has too big cumulative size";
    return error::BlockValidationError::CUMULATIVE_BLOCK_SIZE_TOO_BIG;
  }

  if (minerReward != reward) {
    logger(Logging::DEBUGGING) << "Block reward mismatch for block " << blockStr << ". Expected reward: " << reward
                               << ", got reward: " << minerReward;
    return error::BlockValidationError::BLOCK_REWARD_MISMATCH;
  }

  if (checkpoints.isInCheckpointZone(cachedBlock.getBlockIndex())) {
    if (!checkpoints.checkBlock(cachedBlock.getBlockIndex(), cachedBlock.getBlockHash())) {
      logger(Logging::WARNING) << "Checkpoint block hash mismatch for block " << blockStr;
      return error::BlockValidationError::CHECKPOINT_BLOCK_HASH_MISMATCH;
    }
  } else if (!m_currency.checkProofOfWork(cachedBlock, currentDifficulty)) {
    logger(Logging::WARNING) << "Proof of work too weak for block " << blockStr;
    return error::BlockValidationError::PROOF_OF_WORK_TOO_WEAK;
  }

  auto ret = error::AddBlockErrorCode::ADDED_TO_ALTERNATIVE;

  if (addOnTop) {
    if (cache->getChildCount() == 0) {
      // add block on top of leaf segment.
      auto hashes = preallocateVector<Crypto::Hash>(transactions.size());

      // TODO: exception safety
      if (cache == chainsLeaves[0]) {
        mainChainStorage->pushBlock(rawBlock);

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
  assert(mainChainStorage->getBlockCount() > splitBlockIndex);

  auto blocksToPop = mainChainStorage->getBlockCount() - splitBlockIndex;
  for (size_t i = 0; i < blocksToPop; ++i) {
    mainChainStorage->popBlock();
  }

  for (uint32_t index = splitBlockIndex; index <= newChain.getTopBlockIndex(); ++index) {
    mainChainStorage->pushBlock(newChain.getBlockByIndex(index));
  }
}

void Core::notifyOnSuccess(error::AddBlockErrorCode opResult, uint32_t previousBlockIndex,
                           const CachedBlock& cachedBlock, const IBlockchainCache& cache) {
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
  bool result = fromBinaryArray(blockTemplate, rawBlock.block);
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
    return Xi::make_error(error::AddBlockErrorCode::DESERIALIZATION_FAILED);
  }

  // Quick check, we can skip everything if this fails
  if (findSegmentContainingBlock(blockTemplate.previousBlockHash) == nullptr) {
    logger(Logging::TRACE) << "Lite block rejected as orphaned";
    return Xi::make_error(error::AddBlockErrorCode::REJECTED_AS_ORPHANED);
  }

  RawBlock filledRawBlock;
  filledRawBlock.block = toBinaryArray(blockTemplate);
  filledRawBlock.transactions.reserve(blockTemplate.transactionHashes.size());
  std::set<Crypto::Hash> providedTxs;
  std::transform(txs.begin(), txs.end(), std::inserter(providedTxs, providedTxs.begin()),
                 [](auto& iTx) { return iTx.getTransactionHash(); });

  std::vector<Crypto::Hash> missingTXs{};
  for (const auto& iTxHash : blockTemplate.transactionHashes) {
    if (providedTxs.find(iTxHash) != providedTxs.end()) {
      auto iTx =
          std::find_if(txs.begin(), txs.end(), [&](const auto& tx) { return tx.getTransactionHash() == iTxHash; });
      assert(iTx != txs.end());
      filledRawBlock.transactions.emplace_back(iTx->getTransactionBinaryArray());
    } else {
      auto poolQueryResult = transactionPool().queryTransaction(iTxHash);
      // We are not validating, even if the transaction is not contained by the pool we may have alternative chains
      // containing it.
      if (poolQueryResult.get() == nullptr) {
        auto segment = findSegmentContainingTransaction(iTxHash);
        if (segment == nullptr) {
          missingTXs.push_back(iTxHash);
        } else {
          filledRawBlock.transactions.push_back(segment->getRawTransactions({iTxHash})[0]);
        }
      } else {
        filledRawBlock.transactions.push_back(poolQueryResult->transaction().getTransactionBinaryArray());
      }
    }
  }

  if (missingTXs.empty()) {
    logger(Logging::TRACE) << "Lite block is fully known and will be tried to add";
    return Xi::make_error(addBlock(CachedBlock{std::move(blockTemplate)}, std::move(filledRawBlock)));
  } else {
    logger(Logging::TRACE) << "Lite block has missing transactions.";
    return Xi::make_result<std::vector<Crypto::Hash>>(std::move(missingTXs));
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
  rawBlock.block = std::move(rawBlockTemplate);

  rawBlock.transactions.reserve(blockTemplate.transactionHashes.size());
  for (const auto& transactionHash : blockTemplate.transactionHashes) {
    auto txSearchResult = m_transactionPool->queryTransaction(transactionHash);
    if (!txSearchResult) {
      logger(Logging::WARNING) << "The transaction " << Common::podToHex(transactionHash)
                               << " is absent in transaction pool";
      return error::BlockValidationError::TRANSACTION_ABSENT_IN_POOL;
    } else {
      rawBlock.transactions.emplace_back(txSearchResult->transaction().getTransactionBinaryArray());
    }
  }

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
                          std::vector<BinaryArray>& addedTransactions,
                          std::vector<Crypto::Hash>& deletedTransactions) const {
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
    transactionPrefixInfo.txHash = hash;
    transactionPrefixInfo.txPrefix =
        static_cast<const TransactionPrefix&>(m_transactionPool->getTransaction(hash).getTransaction());
    addedTransactions.emplace_back(std::move(transactionPrefixInfo));
  }

  return getTopBlockHash() == lastBlockHash;
}

bool Core::getBlockTemplate(BlockTemplate& b, const AccountPublicAddress& adr, const BinaryArray& extraNonce,
                            uint64_t& difficulty, uint32_t& height) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);
  XI_UNUSED_REVAL(transactionPool().acquireExclusiveAccess());

  height = getTopBlockIndex() + 1;
  difficulty = getDifficultyForNextBlock();
  if (difficulty == 0) {
    logger(Logging::ERROR) << "difficulty overhead.";
    return false;
  }

  b = boost::value_initialized<BlockTemplate>();
  b.majorVersion = getBlockMajorVersionForHeight(height);
  b.minorVersion = Xi::Config::BlockVersion::expectedMinorVersion();
  if (b.majorVersion == Xi::Config::BlockVersion::BlockVersionCheckpoint<0>::version()) {
  } else if (b.majorVersion >= Xi::Config::BlockVersion::BlockVersionCheckpoint<1>::version()) {
    b.parentBlock.majorVersion = Xi::Config::BlockVersion::BlockVersionCheckpoint<0>::version();
    b.parentBlock.minorVersion = Xi::Config::BlockVersion::expectedMinorVersion();
    b.parentBlock.transactionCount = 1;

    TransactionExtraMergeMiningTag mmTag = boost::value_initialized<decltype(mmTag)>();
    if (!appendMergeMiningTagToExtra(b.parentBlock.baseTransaction.extra, mmTag)) {
      logger(Logging::ERROR) << "Failed to append merge mining tag to extra of the parent block miner transaction";
      return false;
    }
  }

  b.previousBlockHash = getTopBlockHash();
  auto timestamp = timeProvider().posixNow();
  if (timestamp.isError()) {
    logger(Logging::ERROR) << "Failed to receive timestamp: " << timestamp.error().message();
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
  uint32_t blockchain_timestamp_check_window = m_currency.timestampCheckWindow(height, b.majorVersion);

  /* Skip the first N blocks, we don't have enough blocks to calculate a
     proper median yet */
  if (height >= blockchain_timestamp_check_window) {
    std::vector<uint64_t> timestamps;

    /* For the last N blocks, get their timestamps */
    for (uint32_t offset = height - blockchain_timestamp_check_window; offset < height; offset++) {
      timestamps.push_back(getBlockTimestampByIndex(offset));
    }

    uint64_t medianTimestamp = Common::medianValue(timestamps);

    if (b.timestamp < medianTimestamp) {
      b.timestamp = medianTimestamp;
    }
  }

  size_t medianSize = calculateCumulativeBlocksizeLimit(height) / 2;
  const size_t maxBlockSize = std::max(2 * medianSize, m_currency.maxBlockCumulativeSize(height));

  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());
  uint64_t alreadyGeneratedCoins = chainsLeaves[0]->getAlreadyGeneratedCoins();

  size_t transactionsSize;
  uint64_t fee;
  fillBlockTemplate(b, height, medianSize, maxBlockSize, transactionsSize, fee);

  /*
     two-phase miner transaction generation: we don't know exact block size until we prepare block, but we don't know
     reward until we know
     block size, so first miner transaction generated with fake amount of money, and with phase we know think we know
     expected block size
  */
  // make blocks coin-base tx looks close to real coinbase tx to get truthful blob size
  bool r = m_currency.constructMinerTx(b.majorVersion, height, medianSize, alreadyGeneratedCoins, transactionsSize, fee,
                                       adr, b.baseTransaction, extraNonce, 11);
  if (!r) {
    logger(Logging::ERROR) << "Failed to construct miner tx, first chance";
    return false;
  }

  size_t cumulativeSize = transactionsSize + getObjectBinarySize(b.baseTransaction);
  const size_t TRIES_COUNT = 10;
  for (size_t tryCount = 0; tryCount < TRIES_COUNT; ++tryCount) {
    r = m_currency.constructMinerTx(b.majorVersion, height, medianSize, alreadyGeneratedCoins, cumulativeSize, fee, adr,
                                    b.baseTransaction, extraNonce, 11);
    if (!r) {
      logger(Logging::ERROR) << "Failed to construct miner tx, second chance";
      return false;
    }

    size_t coinbaseBlobSize = getObjectBinarySize(b.baseTransaction);
    if (coinbaseBlobSize > cumulativeSize - transactionsSize) {
      cumulativeSize = transactionsSize + coinbaseBlobSize;
      continue;
    }

    if (coinbaseBlobSize < cumulativeSize - transactionsSize) {
      size_t delta = cumulativeSize - transactionsSize - coinbaseBlobSize;
      b.baseTransaction.extra.insert(b.baseTransaction.extra.end(), delta, 0);
      // here  could be 1 byte difference, because of extra field counter is varint, and it can become from 1-byte len
      // to 2-bytes len.
      if (cumulativeSize != transactionsSize + getObjectBinarySize(b.baseTransaction)) {
        if (!(cumulativeSize + 1 == transactionsSize + getObjectBinarySize(b.baseTransaction))) {
          logger(Logging::ERROR) << "unexpected case: cumulative_size=" << cumulativeSize
                                 << " + 1 is not equal txs_cumulative_size=" << transactionsSize
                                 << " + get_object_blobsize(b.baseTransaction)="
                                 << getObjectBinarySize(b.baseTransaction);
          return false;
        }

        b.baseTransaction.extra.resize(b.baseTransaction.extra.size() - 1);
        if (cumulativeSize != transactionsSize + getObjectBinarySize(b.baseTransaction)) {
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
    if (!(cumulativeSize == transactionsSize + getObjectBinarySize(b.baseTransaction))) {
      logger(Logging::ERROR) << "unexpected case: cumulative_size=" << cumulativeSize
                             << " is not equal txs_cumulative_size=" << transactionsSize
                             << " + get_object_blobsize(b.baseTransaction)=" << getObjectBinarySize(b.baseTransaction);
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
  result.transactionPoolState = Common::toHex(&txHash, sizeof(Crypto::Hash));
  result.blockchainHeight = getTopBlockIndex();
  result.alternativeBlockCount = getAlternativeBlockCount();
  auto hash = getTopBlockHash();
  result.topBlockHashString = Common::toHex(&hash, sizeof(Crypto::Hash));
  return result;
}

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
      alternativeBlocks.push_back(fromBinaryArray<BlockTemplate>(cache->getBlockByIndex(index).block));
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
                               std::vector<CachedTransaction>& transactions, uint64_t& cumulativeSize) {
  try {
    for (const auto& rawTransaction : rawTransactions) {
      if (rawTransaction.size() > m_currency.maxTxSize()) {
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
                                          IBlockchainCache* cache, uint64_t& fee, uint32_t blockIndex) {
  // TransactionValidatorState currentState;
  const auto& transaction = cachedTransaction.getTransaction();
  if (transaction.version > m_currency.maxTxVersion() || transaction.version < m_currency.minTxVersion()) {
    return error::TransactionValidationError::INVALID_VERSION;
  }
  auto error = validateSemantic(transaction, fee, blockIndex);
  if (error != error::TransactionValidationError::VALIDATION_SUCCESS) {
    return error;
  }

  size_t inputIndex = 0;
  for (const auto& input : transaction.inputs) {
    if (input.type() == typeid(KeyInput)) {
      const KeyInput& in = boost::get<KeyInput>(input);
      if (!state.spentKeyImages.insert(in.keyImage).second) {
        return error::TransactionValidationError::INPUT_KEYIMAGE_ALREADY_SPENT;
      }

      if (!checkpoints.isInCheckpointZone(blockIndex + 1)) {
        if (cache->checkIfSpent(in.keyImage, blockIndex)) {
          return error::TransactionValidationError::INPUT_KEYIMAGE_ALREADY_SPENT;
        }

        std::vector<PublicKey> outputKeys;
        assert(!in.outputIndexes.empty());

        std::vector<uint32_t> globalIndexes(in.outputIndexes.size());
        globalIndexes[0] = in.outputIndexes[0];
        for (size_t i = 1; i < in.outputIndexes.size(); ++i) {
          globalIndexes[i] = globalIndexes[i - 1] + in.outputIndexes[i];
        }

        auto result = cache->extractKeyOutputKeys(in.amount, blockIndex, {globalIndexes.data(), globalIndexes.size()},
                                                  outputKeys);
        if (result == ExtractOutputKeysResult::INVALID_GLOBAL_INDEX) {
          return error::TransactionValidationError::INPUT_INVALID_GLOBAL_INDEX;
        }

        if (result == ExtractOutputKeysResult::OUTPUT_LOCKED) {
          return error::TransactionValidationError::INPUT_SPEND_LOCKED_OUT;
        }

        std::vector<const Crypto::PublicKey*> outputKeyPointers;
        outputKeyPointers.reserve(outputKeys.size());
        std::for_each(outputKeys.begin(), outputKeys.end(),
                      [&outputKeyPointers](const Crypto::PublicKey& key) { outputKeyPointers.push_back(&key); });
        if (!Crypto::check_ring_signature(cachedTransaction.getTransactionPrefixHash(), in.keyImage,
                                          outputKeyPointers.data(), outputKeyPointers.size(),
                                          transaction.signatures[inputIndex].data(), true)) {
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
  if (transaction.extra.size() > TX_EXTRA_NONCE_MAX_COUNT) {
    return error::TransactionValidationError::EXTRA_NONCE_TOO_LARGE;
  }

  uint64_t summaryOutputAmount = 0;
  for (const auto& output : transaction.outputs) {
    if (output.amount == 0) {
      return error::TransactionValidationError::OUTPUT_ZERO_AMOUNT;
    }

    if (output.target.type() == typeid(KeyOutput)) {
      if (!check_key(boost::get<KeyOutput>(output.target).key)) {
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

  // parameters used for the additional key_image check
  static const Crypto::KeyImage Z = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
  XI_UNUSED(Z);
  static const Crypto::KeyImage I = {{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
  static const Crypto::KeyImage L = {{0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7,
                                      0xa2, 0xde, 0xf9, 0xde, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10}};

  uint64_t summaryInputAmount = 0;
  std::unordered_set<Crypto::KeyImage> ki;
  std::set<std::pair<uint64_t, uint32_t>> outputsUsage;
  for (const auto& input : transaction.inputs) {
    uint64_t amount = 0;
    if (input.type() == typeid(KeyInput)) {
      const KeyInput& in = boost::get<KeyInput>(input);
      amount = in.amount;
      if (!ki.insert(in.keyImage).second) {
        return error::TransactionValidationError::INPUT_IDENTICAL_KEYIMAGES;
      }

      if (in.outputIndexes.empty()) {
        return error::TransactionValidationError::INPUT_EMPTY_OUTPUT_USAGE;
      }

      // outputIndexes are packed here, first is absolute, others are offsets to previous,
      // so first can be zero, others can't
      // Fix discovered by Monero Lab and suggested by "fluffypony" (bitcointalk.org)
      if (!(scalarmultKey(in.keyImage, L) == I)) {
        return error::TransactionValidationError::INPUT_INVALID_DOMAIN_KEYIMAGES;
      }

      if (std::find(++std::begin(in.outputIndexes), std::end(in.outputIndexes), 0u) != std::end(in.outputIndexes)) {
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

  assert(transaction.signatures.size() == transaction.inputs.size());
  fee = summaryInputAmount - summaryOutputAmount;
  return error::TransactionValidationError::VALIDATION_SUCCESS;
}

Xi::Result<uint32_t> Core::findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds) const {
  for (auto& hash : remoteBlockIds) {
    IBlockchainCache* blockchainSegment = findMainChainSegmentContainingBlock(hash);
    if (blockchainSegment != nullptr) {
      if (findIndexInChain(blockchainSegment, m_currency.genesisBlockHash()) == nullptr) {
        return Xi::make_error(error::CoreErrorCode::GENESIS_BLOCK_NOT_FOUND);
      } else {
        return Xi::make_result<uint32_t>(blockchainSegment->getBlockIndex(hash));
      }
    }
  }
  return Xi::make_error(error::CoreErrorCode::GENESIS_BLOCK_NOT_FOUND);
}

std::vector<Crypto::Hash> CryptoNote::Core::getBlockHashes(uint32_t startBlockIndex, uint32_t maxCount) const {
  return chainsLeaves[0]->getBlockHashes(startBlockIndex, maxCount);
}

std::error_code Core::validateBlock(const CachedBlock& cachedBlock, IBlockchainCache* cache, uint64_t& minerReward) {
  const auto& block = cachedBlock.getBlock();
  auto previousBlockIndex = cache->getBlockIndex(block.previousBlockHash);
  // assert(block.previousBlockHash == cache->getBlockHash(previousBlockIndex));

  minerReward = 0;

  if (m_upgradeManager->getBlockMajorVersion(cachedBlock.getBlockIndex()) != block.majorVersion) {
    return error::BlockValidationError::WRONG_MAJOR_VERSION;
  }
  if (!Xi::Config::BlockVersion::validateMinorVersion(block.minorVersion)) {
    return error::BlockValidationError::WRONG_MINOR_VERSION;
  }

  if (block.majorVersion >= Xi::Config::BlockVersion::BlockVersionCheckpoint<1>::version()) {
    if (block.majorVersion == Xi::Config::BlockVersion::BlockVersionCheckpoint<1>::version() &&
        block.parentBlock.majorVersion > Xi::Config::BlockVersion::BlockVersionCheckpoint<0>::version()) {
      logger(Logging::ERROR) << "Parent block of block " << cachedBlock.getBlockHash()
                             << " has wrong major version: " << static_cast<int>(block.parentBlock.majorVersion)
                             << ", at index " << cachedBlock.getBlockIndex() << " expected version is "
                             << static_cast<int>(Xi::Config::BlockVersion::BlockVersionCheckpoint<0>::version());
      return error::BlockValidationError::PARENT_BLOCK_WRONG_VERSION;
    }

    if (cachedBlock.getParentBlockBinaryArray(false).size() > 2048) {
      return error::BlockValidationError::PARENT_BLOCK_SIZE_TOO_BIG;
    }
  }

  if (block.timestamp >
      getAdjustedTime() + m_currency.blockFutureTimeLimit(previousBlockIndex + 1, block.majorVersion)) {
    return error::BlockValidationError::TIMESTAMP_TOO_FAR_IN_FUTURE;
  }

  auto timestamps = cache->getLastTimestamps(
      m_currency.timestampCheckWindow(previousBlockIndex + 1, block.majorVersion), previousBlockIndex, addGenesisBlock);
  if (timestamps.size() >= m_currency.timestampCheckWindow(previousBlockIndex + 1, block.majorVersion)) {
    auto median_ts = Common::medianValue(timestamps);
    if (block.timestamp < median_ts) {
      return error::BlockValidationError::TIMESTAMP_TOO_FAR_IN_PAST;
    }
  }

  if (block.baseTransaction.extra.size() > TX_EXTRA_NONCE_MAX_COUNT) {
    return error::TransactionValidationError::EXTRA_NONCE_TOO_LARGE;
  }

  if (block.baseTransaction.inputs.size() != 1) {
    return error::TransactionValidationError::BASE_INPUT_WRONG_COUNT;
  }

  if (block.baseTransaction.inputs[0].type() != typeid(BaseInput)) {
    return error::TransactionValidationError::BASE_INPUT_UNEXPECTED_TYPE;
  }

  if (boost::get<BaseInput>(block.baseTransaction.inputs[0]).blockIndex != previousBlockIndex + 1) {
    return error::TransactionValidationError::BASE_INPUT_WRONG_BLOCK_INDEX;
  }

  if (!(block.baseTransaction.unlockTime == previousBlockIndex + 1 + m_currency.minedMoneyUnlockWindow())) {
    return error::TransactionValidationError::BASE_TRANSACTION_WRONG_UNLOCK_TIME;
  }

  for (const auto& output : block.baseTransaction.outputs) {
    if (output.amount == 0) {
      return error::TransactionValidationError::OUTPUT_ZERO_AMOUNT;
    }

    if (output.target.type() == typeid(KeyOutput)) {
      if (!check_key(boost::get<KeyOutput>(output.target).key)) {
        return error::TransactionValidationError::OUTPUT_INVALID_KEY;
      }
    } else {
      return error::TransactionValidationError::OUTPUT_UNKNOWN_TYPE;
    }

    if (std::numeric_limits<uint64_t>::max() - output.amount < minerReward) {
      return error::TransactionValidationError::OUTPUTS_AMOUNT_OVERFLOW;
    }

    minerReward += output.amount;
  }

  return error::BlockValidationError::VALIDATION_SUCCESS;
}

uint64_t CryptoNote::Core::getAdjustedTime() const { return time(NULL); }

const Currency& Core::getCurrency() const { return m_currency; }

void Core::save() {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  deleteAlternativeChains();
  mergeMainChainSegments();
  chainsLeaves[0]->save();
}

void Core::load() {
  if (initialized) {
    throw std::runtime_error{"core is already initialized"};
  }
  XI_CONCURRENT_RLOCK(m_access);

  initRootSegment();

  start_time = std::time(nullptr);

  auto dbBlocksCount = chainsLeaves[0]->getTopBlockIndex() + 1;
  auto storageBlocksCount = mainChainStorage->getBlockCount();

  logger(Logging::DEBUGGING) << "Blockchain storage blocks count: " << storageBlocksCount
                             << ", DB blocks count: " << dbBlocksCount;

  assert(storageBlocksCount != 0);  // we assume the storage has at least genesis block

  if (storageBlocksCount > dbBlocksCount) {
    logger(Logging::INFO) << "Importing blocks from blockchain storage";
    importBlocksFromStorage();
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
    importBlocksFromStorage();
  } else {
    logger(Logging::DEBUGGING) << "Blockchain storage and root segment are on the same height and chain";
  }

  initialized = true;
}

void Core::initRootSegment() {
  std::unique_ptr<IBlockchainCache> cache = this->blockchainCacheFactory->createRootBlockchainCache(m_currency);

  mainChainSet.emplace(cache.get());

  chainsLeaves.push_back(cache.get());
  chainsStorage.push_back(std::move(cache));

  contextGroup.spawn(std::bind(&Core::transactionPoolCleaningProcedure, this));

  updateBlockMedianSize();

  chainsLeaves[0]->load();
}

void Core::importBlocksFromStorage() {
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
    uint64_t cumulativeSize = 0;
    if (!extractTransactions(rawBlock.transactions, transactions, cumulativeSize)) {
      logger(Logging::ERROR) << "Couldn't deserialize raw block transactions in block " << cachedBlock.getBlockHash();
      throw std::system_error(make_error_code(error::AddBlockErrorCode::DESERIALIZATION_FAILED));
    }

    cumulativeSize += getObjectBinarySize(blockTemplate.baseTransaction);
    TransactionValidatorState spentOutputs = extractSpentOutputs(transactions);
    auto currentDifficulty = chainsLeaves[0]->getDifficultyForNextBlock(i - 1);

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
}

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

IBlockchainCache* Core::findSegmentContainingBlock(const Crypto::Hash& blockHash) const {
  assert(chainsLeaves.size() > 0);

  // first search in main chain
  auto blockSegment = findMainChainSegmentContainingBlock(blockHash);
  if (blockSegment != nullptr) {
    return blockSegment;
  }

  // than search in alternative chains
  return findAlternativeSegmentContainingBlock(blockHash);
}

IBlockchainCache* Core::findSegmentContainingBlock(uint32_t blockHeight) const {
  assert(chainsLeaves.size() > 0);

  // first search in main chain
  auto blockSegment = findMainChainSegmentContainingBlock(blockHeight);
  if (blockSegment != nullptr) {
    return blockSegment;
  }

  // than search in alternative chains
  return findAlternativeSegmentContainingBlock(blockHeight);
}

IBlockchainCache* Core::findAlternativeSegmentContainingBlock(const Crypto::Hash& blockHash) const {
  IBlockchainCache* cache = nullptr;
  std::find_if(++chainsLeaves.begin(), chainsLeaves.end(),
               [&](IBlockchainCache* chain) { return cache = findIndexInChain(chain, blockHash); });
  return cache;
}

IBlockchainCache* Core::findMainChainSegmentContainingBlock(const Crypto::Hash& blockHash) const {
  return findIndexInChain(chainsLeaves[0], blockHash);
}

IBlockchainCache* Core::findMainChainSegmentContainingBlock(uint32_t blockIndex) const {
  return findIndexInChain(chainsLeaves[0], blockIndex);
}

IBlockchainCache* Core::findAlternativeSegmentContainingBlock(uint32_t blockIndex) const {
  IBlockchainCache* cache = nullptr;
  std::find_if(++chainsLeaves.begin(), chainsLeaves.end(),
               [&](IBlockchainCache* chain) { return cache = findIndexInChain(chain, blockIndex); });
  return cache;
}

BlockTemplate Core::restoreBlockTemplate(IBlockchainCache* blockchainCache, uint32_t blockIndex) const {
  RawBlock rawBlock = blockchainCache->getBlockByIndex(blockIndex);

  BlockTemplate block;
  if (!fromBinaryArray(block, rawBlock.block)) {
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
    entry.blockId = std::move(blockHash);
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
    blockShortInfo.block = std::move(rawBlock.block);
    blockShortInfo.blockId = segment->getBlockHash(blockIndex);

    blockShortInfo.txPrefixes.reserve(rawBlock.transactions.size());
    for (auto& rawTransaction : rawBlock.transactions) {
      TransactionPrefixInfo prefixInfo;
      prefixInfo.txHash =
          getBinaryArrayHash(rawTransaction);  // TODO: is there faster way to get hash without calculation?

      Transaction transaction;
      if (!fromBinaryArray(transaction, rawTransaction)) {
        // TODO: log it
        throw std::runtime_error("Couldn't deserialize transaction");
      }

      prefixInfo.txPrefix = std::move(static_cast<TransactionPrefix&>(transaction));
      blockShortInfo.txPrefixes.emplace_back(std::move(prefixInfo));
    }

    entries.emplace_back(std::move(blockShortInfo));
  }
}

void Core::fillQueryBlockDetails(uint32_t fullOffset, uint32_t currentIndex, size_t maxItemsCount,
                                 std::vector<BlockDetails>& entries) const {
  assert(currentIndex >= fullOffset);

  uint32_t fullBlocksCount =
      static_cast<uint32_t>(std::min(static_cast<uint32_t>(maxItemsCount), currentIndex - fullOffset + 1));
  entries.reserve(entries.size() + fullBlocksCount);

  for (uint32_t blockIndex = fullOffset; blockIndex < fullOffset + fullBlocksCount; ++blockIndex) {
    IBlockchainCache* segment = findMainChainSegmentContainingBlock(blockIndex);
    Crypto::Hash blockHash = segment->getBlockHash(blockIndex);
    BlockDetails block = getBlockDetails(blockHash);
    entries.emplace_back(std::move(block));
  }
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

uint8_t Core::getBlockMajorVersionForHeight(uint32_t height) const {
  return m_upgradeManager->getBlockMajorVersion(height);
}

size_t Core::calculateCumulativeBlocksizeLimit(uint32_t height) const {
  uint8_t nextBlockMajorVersion = getBlockMajorVersionForHeight(height);
  size_t nextBlockGrantedFullRewardZone = m_currency.blockGrantedFullRewardZoneByBlockVersion(nextBlockMajorVersion);

  assert(!chainsStorage.empty());
  assert(!chainsLeaves.empty());
  // FIXME: skip gensis here?
  auto sizes = chainsLeaves[0]->getLastBlocksSizes(m_currency.rewardBlocksWindowByBlockVersion(nextBlockMajorVersion));
  uint64_t median = Common::medianValue(sizes);
  if (median <= nextBlockGrantedFullRewardZone) {
    median = nextBlockGrantedFullRewardZone;
  }

  return median * 2;
}

void Core::fillBlockTemplate(BlockTemplate& block, uint32_t height, size_t fullRewardZone, size_t maxCumulativeSize,
                             size_t& transactionsSize, uint64_t& fee) const {
  throwIfNotInitialized();
  transactionsSize = 0;
  fee = 0;

  // What would be our reward without any transaction used
  const auto generatedCoins = chainsLeaves[0]->getAlreadyGeneratedCoins(height - 1);
  int64_t emissionChange = 0;
  uint64_t currentReward = 0;
  m_currency.getBlockReward(block.majorVersion, fullRewardZone, 0, generatedCoins, fee, currentReward, emissionChange);

  size_t cumulativeSize = m_currency.minerTxBlobReservedSize();

  const TransactionValidationResult::EligibleIndex blockIndex{height, block.timestamp};
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
      m_currency.getBlockReward(block.majorVersion, fullRewardZone, cumulativeSize + iSize, generatedCoins, fee + iFee,
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
                   [&leaf](const std::unique_ptr<IBlockchainCache>& segment) { return segment.get() == leaf; });

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
      [&rootSegment](const std::unique_ptr<IBlockchainCache>& segment) { return segment.get() == rootSegment; });

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
    if (!fromBinaryArray(block, info.rawBlock.block)) {
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

BlockDetails Core::getBlockDetails(const uint32_t blockHeight) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  IBlockchainCache* segment = findSegmentContainingBlock(blockHeight);
  if (segment == nullptr) {
    throw std::runtime_error("Requested block height wasn't found in blockchain.");
  }

  return getBlockDetails(segment->getBlockHash(blockHeight));
}

BlockDetails Core::getBlockDetails(const Crypto::Hash& blockHash) const {
  throwIfNotInitialized();
  XI_CONCURRENT_RLOCK(m_access);

  IBlockchainCache* segment = findSegmentContainingBlock(blockHash);
  if (segment == nullptr) {
    throw std::runtime_error("Requested hash wasn't found in blockchain.");
  }

  uint32_t blockIndex = segment->getBlockIndex(blockHash);
  BlockTemplate blockTemplate = restoreBlockTemplate(segment, blockIndex);

  BlockDetails blockDetails;
  blockDetails.majorVersion = blockTemplate.majorVersion;
  blockDetails.minorVersion = blockTemplate.minorVersion;
  blockDetails.timestamp = blockTemplate.timestamp;
  blockDetails.prevBlockHash = blockTemplate.previousBlockHash;
  blockDetails.nonce = blockTemplate.nonce;
  blockDetails.hash = blockHash;

  blockDetails.reward = 0;
  for (const TransactionOutput& out : blockTemplate.baseTransaction.outputs) {
    blockDetails.reward += out.amount;
  }

  blockDetails.index = blockIndex;
  blockDetails.isAlternative = mainChainSet.count(segment) == 0;

  blockDetails.difficulty = getBlockDifficulty(blockIndex);

  std::vector<uint64_t> sizes = segment->getLastBlocksSizes(1, blockDetails.index, addGenesisBlock);
  assert(sizes.size() == 1);
  blockDetails.transactionsCumulativeSize = sizes.front();

  uint64_t blockBlobSize = getObjectBinarySize(blockTemplate);
  uint64_t coinbaseTransactionSize = getObjectBinarySize(blockTemplate.baseTransaction);
  blockDetails.blockSize = blockBlobSize + blockDetails.transactionsCumulativeSize - coinbaseTransactionSize;

  blockDetails.alreadyGeneratedCoins = segment->getAlreadyGeneratedCoins(blockDetails.index);
  blockDetails.alreadyGeneratedTransactions = segment->getAlreadyGeneratedTransactions(blockDetails.index);

  uint64_t prevBlockGeneratedCoins = 0;
  blockDetails.sizeMedian = 0;
  if (blockDetails.index > 0) {
    auto lastBlocksSizes =
        segment->getLastBlocksSizes(m_currency.rewardBlocksWindowByBlockVersion(blockDetails.majorVersion),
                                    blockDetails.index - 1, addGenesisBlock);
    blockDetails.sizeMedian = Common::medianValue(lastBlocksSizes);
    prevBlockGeneratedCoins = segment->getAlreadyGeneratedCoins(blockDetails.index - 1);
  }

  int64_t emissionChange = 0;
  bool result = m_currency.getBlockReward(blockDetails.majorVersion, blockDetails.sizeMedian, 0,
                                          prevBlockGeneratedCoins, 0, blockDetails.baseReward, emissionChange);
  if (result) {
  }
  assert(result);

  uint64_t currentReward = 0;
  result = m_currency.getBlockReward(blockDetails.majorVersion, blockDetails.sizeMedian,
                                     blockDetails.transactionsCumulativeSize, prevBlockGeneratedCoins, 0, currentReward,
                                     emissionChange);
  assert(result);

  if (blockDetails.baseReward == 0 && currentReward == 0) {
    blockDetails.penalty = static_cast<double>(0);
  } else {
    assert(blockDetails.baseReward >= currentReward);
    blockDetails.penalty =
        static_cast<double>(blockDetails.baseReward - currentReward) / static_cast<double>(blockDetails.baseReward);
  }

  blockDetails.transactions.reserve(blockTemplate.transactionHashes.size() + 1);
  CachedTransaction cachedBaseTx(std::move(blockTemplate.baseTransaction));
  blockDetails.transactions.push_back(getTransactionDetails(cachedBaseTx.getTransactionHash(), segment, false));

  blockDetails.totalFeeAmount = 0;
  for (const Crypto::Hash& transactionHash : blockTemplate.transactionHashes) {
    blockDetails.transactions.push_back(getTransactionDetails(transactionHash, segment, false));
    blockDetails.totalFeeAmount += blockDetails.transactions.back().fee;
  }

  return blockDetails;
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

    segment->getRawTransactions(transactionsHashes, rawTransactions, missedTransactionsHashes);
    assert(missedTransactionsHashes.empty());
    assert(rawTransactions.size() == 1);

    std::vector<CachedTransaction> transactions;
    Utils::restoreCachedTransactions(rawTransactions, transactions);
    assert(transactions.size() == 1);

    transactionDetails.inBlockchain = true;
    transactionDetails.blockIndex = segment->getBlockIndexContainingTx(transactionHash);
    transactionDetails.blockHash = segment->getBlockHash(transactionDetails.blockIndex);

    auto timestamps = segment->getLastTimestamps(1, transactionDetails.blockIndex, addGenesisBlock);
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
    uint64_t currentMixin = input.outputIndexes.size();
    if (currentMixin > transactionDetails.mixin) {
      transactionDetails.mixin = currentMixin;
    }
  }

  transactionDetails.paymentId = boost::value_initialized<Crypto::Hash>();
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
      baseDetails.input = boost::get<BaseInput>(rawTransaction.inputs[i]);
      baseDetails.amount = transaction->getOutputTotalAmount();
      txInDetails = baseDetails;
    } else if (transaction->getInputType(i) == TransactionTypes::InputType::Key) {
      KeyInputDetails txInToKeyDetails;
      txInToKeyDetails.input = boost::get<KeyInput>(rawTransaction.inputs[i]);
      std::vector<std::pair<Crypto::Hash, size_t>> outputReferences;
      outputReferences.reserve(txInToKeyDetails.input.outputIndexes.size());
      std::vector<uint32_t> globalIndexes = relativeOutputOffsetsToAbsolute(txInToKeyDetails.input.outputIndexes);
      ExtractOutputKeysResult result = segment->extractKeyOtputReferences(
          txInToKeyDetails.input.amount, {globalIndexes.data(), globalIndexes.size()}, outputReferences);
      assert(result == ExtractOutputKeysResult::SUCCESS);
      (void)result;
      assert(txInToKeyDetails.input.outputIndexes.size() == outputReferences.size());

      txInToKeyDetails.mixin = txInToKeyDetails.input.outputIndexes.size();
      txInToKeyDetails.output.number = outputReferences.back().second;
      txInToKeyDetails.output.transactionHash = outputReferences.back().first;
      txInDetails = txInToKeyDetails;
    }

    assert(!txInDetails.empty());
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

std::vector<Crypto::Hash> Core::getTransactionHashesByPaymentId(const Hash& paymentId) const {
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

void Core::throwIfNotInitialized() const {
  if (!initialized) {
    throw std::system_error(make_error_code(error::CoreErrorCode::NOT_INITIALIZED));
  }
}

IBlockchainCache* Core::findSegmentContainingTransaction(const Crypto::Hash& transactionHash) const {
  assert(!chainsLeaves.empty());
  assert(!chainsStorage.empty());

  IBlockchainCache* segment = chainsLeaves[0];
  assert(segment != nullptr);

  // find in main chain
  do {
    if (segment->hasTransaction(transactionHash)) {
      return segment;
    }

    segment = segment->getParent();
  } while (segment != nullptr);

  // find in alternative chains
  for (size_t chain = 1; chain < chainsLeaves.size(); ++chain) {
    segment = chainsLeaves[chain];

    while (mainChainSet.count(segment) == 0) {
      if (segment->hasTransaction(transactionHash)) {
        return segment;
      }

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

      auto deletedTransactions =
          static_cast<TransactionPoolCleanWrapper*>(m_transactionPool.get())->clean(getTopBlockIndex());
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
      m_upgradeManager->getBlockMajorVersion(mainChain->getTopBlockIndex() + 1));

  auto lastBlockSizes = mainChain->getLastBlocksSizes(m_currency.rewardBlocksWindowByBlockVersion(
      m_upgradeManager->getBlockMajorVersion(mainChain->getTopBlockIndex() + 1)));

  blockMedianSize =
      std::max(Common::medianValue(lastBlockSizes), static_cast<uint64_t>(nextBlockGrantedFullRewardZone));
}

uint64_t Core::get_current_blockchain_height() const {
  // TODO: remove when GetCoreStatistics is implemented
  XI_CONCURRENT_RLOCK(m_access);
  return mainChainStorage->getBlockCount();
}

std::time_t Core::getStartTime() const { return start_time; }

const ITransactionPool& Core::transactionPool() const { return *m_transactionPool; }
ITransactionPool& Core::transactionPool() { return *m_transactionPool; }

}  // namespace CryptoNote
