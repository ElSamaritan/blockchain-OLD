// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "BlockchainCache.h"

#include <fstream>
#include <tuple>

#include <boost/functional/hash.hpp>

#include <Xi/Global.hh>
#include <Xi/Crypto/Random/Engine.hpp>

#include "Common/StdInputStream.h"
#include "Common/StdOutputStream.h"
#include "Common/ShuffleGenerator.h"

#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "CryptoNoteCore/CryptoNoteSerialization.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/BlockchainStorage.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"
#include "CryptoNoteCore/Transactions/TransactionValidatiorState.h"

#include "Serialization/SerializationOverloads.h"

namespace CryptoNote {

namespace {

const UseGenesis addGenesisBlock = UseGenesis(true);
const UseGenesis skipGenesisBlock = UseGenesis(false);

template <class T, class F>
void splitGlobalIndexes(T& sourceContainer, T& destinationContainer, uint32_t splitBlockIndex, F lowerBoundFunction) {
  for (auto it = sourceContainer.begin(); it != sourceContainer.end();) {
    auto newCacheOutputsIteratorStart =
        lowerBoundFunction(it->second.outputs.begin(), it->second.outputs.end(), splitBlockIndex);

    auto& indexesForAmount = destinationContainer[it->first];
    auto newCacheOutputsCount =
        static_cast<uint32_t>(std::distance(newCacheOutputsIteratorStart, it->second.outputs.end()));
    indexesForAmount.outputs.reserve(newCacheOutputsCount);

    indexesForAmount.startIndex =
        it->second.startIndex + static_cast<uint32_t>(it->second.outputs.size()) - newCacheOutputsCount;

    std::move(newCacheOutputsIteratorStart, it->second.outputs.end(), std::back_inserter(indexesForAmount.outputs));
    it->second.outputs.erase(newCacheOutputsIteratorStart, it->second.outputs.end());

    if (indexesForAmount.outputs.empty()) {
      destinationContainer.erase(it->first);
    }

    if (it->second.outputs.empty()) {
      // if we gave all of our outputs we don't need this amount entry any more
      it = sourceContainer.erase(it);
    } else {
      ++it;
    }
  }
}
}  // namespace

bool SpentKeyImage::serialize(ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(blockIndex, "block_index"), false);
  XI_RETURN_EC_IF_NOT(s(keyImage, "key_image"), false);
  return true;
}

bool CachedTransactionInfo::serialize(ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(blockIndex, "block_index"), false);
  XI_RETURN_EC_IF_NOT(s(transactionIndex, "transaction_index"), false);
  XI_RETURN_EC_IF_NOT(s(transactionHash, "transaction_hash"), false);
  XI_RETURN_EC_IF_NOT(s(unlockTime, "unlock_time"), false);
  XI_RETURN_EC_IF_NOT(s(outputs, "outputs"), false);
  XI_RETURN_EC_IF_NOT(s(globalIndexes, "global_indexes"), false);
  XI_RETURN_EC_IF_NOT(s(isDeterministicallyGenerated, "is_deterministically_generated"), false);
  return true;
}

bool CachedBlockInfo::serialize(ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(blockHash, "block_hash"), false);
  XI_RETURN_EC_IF_NOT(s(version, "version"), false);
  XI_RETURN_EC_IF_NOT(s(upgradeVote, "upgrade_vote"), false);
  XI_RETURN_EC_IF_NOT(s(timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(s(blobSize, "blob_size"), false);
  XI_RETURN_EC_IF_NOT(s(cumulativeDifficulty, "cumulative_difficulty"), false);
  XI_RETURN_EC_IF_NOT(s(alreadyGeneratedCoins, "already_generated_coins"), false);
  XI_RETURN_EC_IF_NOT(s(alreadyGeneratedTransactions, "already_generated_transaction_count"), false);
  return true;
}

bool OutputGlobalIndexesForAmount::serialize(ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(startIndex, "start_index"), false);
  XI_RETURN_EC_IF_NOT(s(outputs, "outputs"), false);
  return true;
}

bool PaymentIdTransactionHashPair::serialize(ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(paymentId, "payment_id"), false);
  XI_RETURN_EC_IF_NOT(s(transactionHash, "transaction_hash"), false);
  return true;
}

[[nodiscard]] bool serialize(PackedOutIndex& value, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializer(value.packedValue, name);
}

BlockchainCache::BlockchainCache(const std::string& filename, const Currency& currency, Logging::ILogger& logger_,
                                 IBlockchainCache* parent, uint32_t splitBlockIndex)
    : CommonBlockchainCache(logger_, currency),
      filename(filename),
      currency(currency),
      logger(logger_, "BlockchainCache"),
      parent(parent),
      storage(new BlockchainStorage(100)) {
  if (parent == nullptr) {
    startIndex = 0;

    const CachedBlock genesisBlock(currency.genesisBlock());
    const auto& genesisRawBlock = genesisBlock.getBlock();

    uint64_t genesisGeneratedCoins = 0;
    for (const TransactionOutput& output : genesisBlock.getBlock().baseTransaction.outputs) {
      genesisGeneratedCoins += output.amount;
    }

    boost::optional<Transaction> staticReward = currency.constructStaticRewardTx(genesisRawBlock).takeOrThrow();
    if (staticReward.has_value()) {
      for (const TransactionOutput& output : staticReward->outputs) {
        genesisGeneratedCoins += output.amount;
      }
    }

    assert(genesisGeneratedCoins > 0);

    uint64_t genesisBlockSize = genesisBlock.getBlock().baseTransaction.binarySize();
    assert(genesisBlockSize < std::numeric_limits<uint64_t>::max());

    std::vector<CachedTransaction> cachedTransactions;
    TransactionValidatorState validatorState;
    doPushBlock(genesisBlock, cachedTransactions, validatorState, genesisBlockSize, genesisGeneratedCoins, 1,
                {toBinaryArray(genesisBlock.getBlock()), {}});
  } else {
    startIndex = splitBlockIndex;
  }

  logger(Logging::Debugging) << "BlockchainCache with start block index: " << startIndex << " created";
}

void BlockchainCache::pushBlock(const CachedBlock& cachedBlock,
                                const std::vector<CachedTransaction>& cachedTransactions,
                                const TransactionValidatorState& validatorState, size_t blockSize,
                                uint64_t generatedCoins, uint64_t blockDifficulty, RawBlock&& rawBlock) {
  // we have to call this function from constructor so it has to be non-virtual
  doPushBlock(cachedBlock, cachedTransactions, validatorState, blockSize, generatedCoins, blockDifficulty,
              std::move(rawBlock));
}

void BlockchainCache::doPushBlock(const CachedBlock& cachedBlock,
                                  const std::vector<CachedTransaction>& cachedTransactions,
                                  const TransactionValidatorState& validatorState, size_t blockSize,
                                  uint64_t generatedCoins, uint64_t blockDifficulty, RawBlock&& rawBlock) {
  logger(Logging::Debugging) << "Pushing block " << cachedBlock.getBlockHash() << " at index "
                             << cachedBlock.getBlockIndex();

  assert(blockSize > 0);
  assert(blockDifficulty > 0);

  uint64_t cumulativeDifficulty = 0;
  uint64_t alreadyGeneratedCoins = 0;
  uint64_t alreadyGeneratedTransactions = 0;

  boost::optional<Transaction> staticReward = currency.constructStaticRewardTx(cachedBlock).takeOrThrow();
  const auto index = cachedBlock.getBlockIndex();

  if (getBlockCount() == 0) {
    if (parent != nullptr && index > 0) {
      cumulativeDifficulty = parent->getCurrentCumulativeDifficulty(index - 1);
      alreadyGeneratedCoins = parent->getAlreadyGeneratedCoins(index - 1);
      alreadyGeneratedTransactions = parent->getAlreadyGeneratedTransactions(index - 1);
    }

    cumulativeDifficulty += blockDifficulty;
    alreadyGeneratedCoins += generatedCoins;
    alreadyGeneratedTransactions += cachedTransactions.size() + 1;
    if (staticReward.has_value()) {
      alreadyGeneratedTransactions += 1;
    }
  } else {
    auto& lastBlockInfo = blockInfos.get<BlockIndexTag>().back();

    cumulativeDifficulty = lastBlockInfo.cumulativeDifficulty + blockDifficulty;
    alreadyGeneratedCoins = lastBlockInfo.alreadyGeneratedCoins + generatedCoins;
    alreadyGeneratedTransactions = lastBlockInfo.alreadyGeneratedTransactions + cachedTransactions.size() + 1;
    if (staticReward.has_value()) {
      alreadyGeneratedTransactions += 1;
    }
  }

  CachedBlockInfo blockInfo;
  blockInfo.blockHash = cachedBlock.getBlockHash();
  blockInfo.version = cachedBlock.getBlock().version;
  blockInfo.upgradeVote = cachedBlock.getBlock().upgradeVote;
  blockInfo.timestamp = cachedBlock.getBlock().timestamp;
  blockInfo.blobSize = blockSize;
  blockInfo.cumulativeDifficulty = cumulativeDifficulty;
  blockInfo.alreadyGeneratedCoins = alreadyGeneratedCoins;
  blockInfo.alreadyGeneratedTransactions = alreadyGeneratedTransactions;

  assert(!hasBlock(blockInfo.blockHash));

  blockInfos.get<BlockIndexTag>().emplace_back(std::move(blockInfo));

  auto blockIndex = cachedBlock.getBlockIndex();
  assert(blockIndex == blockInfos.size() + startIndex - 1);

  for (const auto& keyImage : validatorState.spentKeyImages) {
    addSpentKeyImage(keyImage, blockIndex);
  }

  logger(Logging::Debugging) << "Added " << validatorState.spentKeyImages.size() << " spent key images";

  assert(cachedTransactions.size() <= std::numeric_limits<uint16_t>::max());

  uint16_t transactionBlockIndex = 0;
  auto baseTransaction = cachedBlock.getBlock().baseTransaction;
  pushTransaction(CachedTransaction(std::move(baseTransaction)), blockIndex, transactionBlockIndex++, false);
  if (staticReward.has_value()) {
    pushTransaction(CachedTransaction(std::move(*staticReward)), blockIndex, transactionBlockIndex++, true);
  }

  for (const auto& transaction : cachedTransactions) {
    pushTransaction(transaction, blockIndex, transactionBlockIndex++, false);
  }

  storage->pushBlock(std::move(rawBlock));

  logger(Logging::Debugging) << "Block " << cachedBlock.getBlockHash() << " successfully pushed";
}

PushedBlockInfo BlockchainCache::getPushedBlockInfo(uint32_t blockIndex) const {
  assert(blockIndex >= startIndex);
  assert(blockIndex < startIndex + getBlockCount());

  auto localIndex = blockIndex - startIndex;
  const auto& cachedBlock = blockInfos.get<BlockIndexTag>()[localIndex];

  PushedBlockInfo pushedBlockInfo;
  pushedBlockInfo.rawBlock = storage->getBlockByIndex(localIndex);
  pushedBlockInfo.blockSize = cachedBlock.blobSize;
  pushedBlockInfo.timestamp = cachedBlock.timestamp;

  if (blockIndex > startIndex) {
    const auto& previousBlock = blockInfos.get<BlockIndexTag>()[localIndex - 1];
    pushedBlockInfo.blockDifficulty = cachedBlock.cumulativeDifficulty - previousBlock.cumulativeDifficulty;
    pushedBlockInfo.generatedCoins = cachedBlock.alreadyGeneratedCoins - previousBlock.alreadyGeneratedCoins;
  } else {
    if (parent == nullptr || blockIndex == 0) {
      assert(blockIndex == 0);
      pushedBlockInfo.blockDifficulty = cachedBlock.cumulativeDifficulty;
      pushedBlockInfo.generatedCoins = cachedBlock.alreadyGeneratedCoins;
    } else {
      uint64_t cumulativeDifficulty = parent->getLastCumulativeDifficulties(1, blockIndex - 1, addGenesisBlock)[0];
      uint64_t alreadyGeneratedCoins = parent->getAlreadyGeneratedCoins(blockIndex - 1);

      pushedBlockInfo.blockDifficulty = cachedBlock.cumulativeDifficulty - cumulativeDifficulty;
      pushedBlockInfo.generatedCoins = cachedBlock.alreadyGeneratedCoins - alreadyGeneratedCoins;
    }
  }

  pushedBlockInfo.validatorState = fillOutputsSpentByBlock(blockIndex);

  return pushedBlockInfo;
}

// Returns upper part of segment. [this] remains lower part.
// All of indexes on blockIndex == splitBlockIndex belong to upper part
// TODO: first move containers to new cache, then copy elements back. This can be much more effective, cause we usualy
// split blockchain near its top.
std::shared_ptr<IBlockchainCache> BlockchainCache::split(uint32_t splitBlockIndex) {
  logger(Logging::Debugging) << "Splitting at block index: " << splitBlockIndex
                             << ", top block index: " << getTopBlockIndex();

  assert(splitBlockIndex > startIndex);
  assert(splitBlockIndex <= getTopBlockIndex());

  std::unique_ptr<BlockchainStorage> newStorage = storage->splitStorage(splitBlockIndex - startIndex);

  std::shared_ptr<BlockchainCache> newCache(
      new BlockchainCache(filename, currency, logger.getLogger(), this, splitBlockIndex));

  newCache->storage = std::move(newStorage);

  splitSpentKeyImages(*newCache, splitBlockIndex);
  splitTransactions(*newCache, splitBlockIndex);
  splitBlocks(*newCache, splitBlockIndex);
  splitKeyOutputsGlobalIndexes(*newCache, splitBlockIndex);

  fixChildrenParent(newCache.get());
  newCache->children = children;
  children = {newCache.get()};

  logger(Logging::Debugging) << "Split successfully completed";
  return std::move(newCache);
}

void BlockchainCache::splitSpentKeyImages(BlockchainCache& newCache, uint32_t splitBlockIndex) {
  // Key images with blockIndex == splitBlockIndex remain in upper segment
  auto& imagesIndex = spentKeyImages.get<BlockIndexTag>();
  auto lowerBound = imagesIndex.lower_bound(splitBlockIndex);

  newCache.spentKeyImages.get<BlockIndexTag>().insert(lowerBound, imagesIndex.end());
  imagesIndex.erase(lowerBound, imagesIndex.end());

  logger(Logging::Debugging) << "Spent key images split completed";
}

void BlockchainCache::splitTransactions(BlockchainCache& newCache, uint32_t splitBlockIndex) {
  auto& transactionsIndex = transactions.get<BlockIndexTag>();
  auto lowerBound = transactionsIndex.lower_bound(splitBlockIndex);

  for (auto it = lowerBound; it != transactionsIndex.end(); ++it) {
    removePaymentId(it->transactionHash, newCache);
  }

  newCache.transactions.get<BlockIndexTag>().insert(lowerBound, transactionsIndex.end());
  transactionsIndex.erase(lowerBound, transactionsIndex.end());

  logger(Logging::Debugging) << "Transactions split completed";
}

void BlockchainCache::removePaymentId(const Crypto::Hash& transactionHash, BlockchainCache& newCache) {
  auto& index = paymentIds.get<TransactionHashTag>();
  auto it = index.find(transactionHash);

  if (it == index.end()) {
    return;
  }

  newCache.paymentIds.emplace(*it);
  index.erase(it);
}

void BlockchainCache::splitBlocks(BlockchainCache& newCache, uint32_t splitBlockIndex) {
  auto& blocksIndex = blockInfos.get<BlockIndexTag>();
  auto bound = std::next(blocksIndex.begin(), splitBlockIndex - startIndex);
  std::move(bound, blocksIndex.end(), std::back_inserter(newCache.blockInfos.get<BlockIndexTag>()));
  blocksIndex.erase(bound, blocksIndex.end());

  logger(Logging::Debugging) << "Blocks split completed";
}

void BlockchainCache::splitKeyOutputsGlobalIndexes(BlockchainCache& newCache, uint32_t splitBlockIndex) {
  auto lowerBoundFunction = [](std::vector<PackedOutIndex>::iterator begin, std::vector<PackedOutIndex>::iterator end,
                               uint32_t splitBlockIndex) -> std::vector<PackedOutIndex>::iterator {
    return std::lower_bound(begin, end, splitBlockIndex, [](PackedOutIndex outputIndex, uint32_t splitIndex) {
      // all outputs in it->second.outputs are sorted according to blockIndex + transactionIndex
      return outputIndex.data.blockIndex < splitIndex;
    });
  };

  splitGlobalIndexes(keyOutputsGlobalIndexes, newCache.keyOutputsGlobalIndexes, splitBlockIndex, lowerBoundFunction);
  logger(Logging::Debugging) << "Key output global indexes split successfully completed";
}

void BlockchainCache::addSpentKeyImage(const Crypto::KeyImage& keyImage, uint32_t blockIndex) {
  assert(!checkIfSpent(keyImage, blockIndex - 1));  // Changed from "assert(!checkIfSpent(keyImage, blockIndex));"
                                                    // to prevent fail when pushing block from DatabaseBlockchainCache.
                                                    // In case of pushing external block double spend within block
                                                    // should be checked by Core.
  spentKeyImages.get<BlockIndexTag>().emplace(SpentKeyImage{blockIndex, keyImage});
}

std::vector<Crypto::Hash> BlockchainCache::getTransactionHashes() const {
  auto& txInfos = transactions.get<TransactionHashTag>();
  std::vector<Crypto::Hash> hashes;
  for (auto& tx : txInfos) {
    // skip base transaction
    if (tx.transactionIndex != 0) {
      hashes.emplace_back(tx.transactionHash);
    }
  }
  return hashes;
}

void BlockchainCache::pushTransaction(const CachedTransaction& cachedTransaction, uint32_t blockIndex,
                                      uint16_t transactionInBlockIndex, bool generated) {
  logger(Logging::Debugging) << "Adding transaction " << cachedTransaction.getTransactionHash() << " at block "
                             << blockIndex << ", index in block " << transactionInBlockIndex;

  const auto& tx = cachedTransaction.getTransaction();

  CachedTransactionInfo transactionCacheInfo;
  transactionCacheInfo.blockIndex = blockIndex;
  transactionCacheInfo.transactionIndex = transactionInBlockIndex;
  transactionCacheInfo.transactionHash = cachedTransaction.getTransactionHash();
  transactionCacheInfo.unlockTime = tx.unlockTime;
  transactionCacheInfo.isDeterministicallyGenerated = generated;

  assert(tx.outputs.size() <= std::numeric_limits<uint16_t>::max());

  transactionCacheInfo.globalIndexes.reserve(tx.outputs.size());
  transactionCacheInfo.outputs.reserve(tx.outputs.size());

  logger(Logging::Debugging) << "Adding " << tx.outputs.size() << " transaction outputs";
  uint16_t outputCount = 0;
  for (auto& output : tx.outputs) {
    transactionCacheInfo.outputs.push_back(output.target);

    PackedOutIndex poi;
    poi.data.blockIndex = blockIndex;
    poi.data.transactionIndex = transactionInBlockIndex;
    poi.data.outputIndex = outputCount++;

    if (std::holds_alternative<KeyOutput>(output.target)) {
      transactionCacheInfo.globalIndexes.push_back(insertKeyOutputToGlobalIndex(output.amount, poi, blockIndex));
    }
  }

  assert(transactions.get<TransactionHashTag>().count(transactionCacheInfo.transactionHash) == 0);
  transactions.get<TransactionInBlockTag>().emplace(std::move(transactionCacheInfo));

  PaymentIdTransactionHashPair paymentIdTransactionHash;
  if (getPaymentIdFromTxExtra(tx.extra, paymentIdTransactionHash.paymentId)) {
    logger(Logging::Debugging) << "Payment id found: " << paymentIdTransactionHash.paymentId;
    paymentIdTransactionHash.transactionHash = cachedTransaction.getTransactionHash();
    paymentIds.emplace(std::move(paymentIdTransactionHash));
  }

  logger(Logging::Debugging) << "Transaction " << cachedTransaction.getTransactionHash() << " successfully added";
}

uint32_t BlockchainCache::insertKeyOutputToGlobalIndex(uint64_t amount, PackedOutIndex output, uint32_t blockIndex) {
  auto pair = keyOutputsGlobalIndexes.emplace(amount, OutputGlobalIndexesForAmount{});
  auto& indexEntry = pair.first->second;
  indexEntry.outputs.push_back(output);
  if (pair.second && parent != nullptr) {
    indexEntry.startIndex = static_cast<uint32_t>(parent->getKeyOutputsCountForAmount(amount, blockIndex));
    logger(Logging::Debugging) << "Key output count for amount " << amount
                               << " requested from parent. Returned count: " << indexEntry.startIndex;
  }

  return indexEntry.startIndex + static_cast<uint32_t>(indexEntry.outputs.size()) - 1;
}

bool BlockchainCache::checkIfSpent(const Crypto::KeyImage& keyImage, uint32_t blockIndex) const {
  if (blockIndex < startIndex) {
    assert(parent != nullptr);
    return parent->checkIfSpent(keyImage, blockIndex);
  }

  auto it = spentKeyImages.get<KeyImageTag>().find(keyImage);
  if (it == spentKeyImages.get<KeyImageTag>().end()) {
    return parent != nullptr ? parent->checkIfSpent(keyImage, blockIndex) : false;
  }

  return it->blockIndex <= blockIndex;
}

bool BlockchainCache::checkIfSpent(const Crypto::KeyImage& keyImage) const {
  if (spentKeyImages.get<KeyImageTag>().count(keyImage) != 0) {
    return true;
  }

  return parent != nullptr && parent->checkIfSpent(keyImage);
}

uint32_t BlockchainCache::getBlockCount() const { return static_cast<uint32_t>(blockInfos.size()); }

bool BlockchainCache::hasBlock(const Crypto::Hash& blockHash) const {
  return blockInfos.get<BlockHashTag>().count(blockHash) != 0;
}

uint32_t BlockchainCache::getBlockIndex(const Crypto::Hash& blockHash) const {
  //  assert(blockInfos.get<BlockHashTag>().count(blockHash) > 0);
  const auto hashIt = blockInfos.get<BlockHashTag>().find(blockHash);
  if (hashIt == blockInfos.get<BlockHashTag>().end()) {
    throw std::runtime_error("no such block");
  }

  const auto rndIt = blockInfos.project<BlockIndexTag>(hashIt);
  return static_cast<uint32_t>(std::distance(blockInfos.get<BlockIndexTag>().begin(), rndIt)) + startIndex;
}

BlockHeightVector BlockchainCache::getBlockHeights(ConstBlockHashSpan hashes) const {
  BlockHeightVector reval{};
  reval.resize(hashes.size(), BlockHeight::Null);
  size_t i = 0;
  for (const auto& hash : hashes) {
    const auto it = blockInfos.get<BlockHashTag>().find(hash);
    if (it == blockInfos.get<BlockHashTag>().end()) {
      i += 1;
      continue;
    }
    const auto indexIt = blockInfos.project<BlockIndexTag>(it);
    const auto offset = static_cast<uint32_t>(std::distance(blockInfos.get<BlockIndexTag>().begin(), indexIt));
    reval[i++] = BlockHeight::fromIndex(offset + startIndex);
  }
  return reval;
}

Crypto::Hash BlockchainCache::getBlockHash(uint32_t blockIndex) const {
  if (blockIndex < startIndex) {
    assert(parent != nullptr);
    return parent->getBlockHash(blockIndex);
  }

  assert(blockIndex - startIndex < blockInfos.size());
  return blockInfos.get<BlockIndexTag>()[blockIndex - startIndex].blockHash;
}

std::vector<Crypto::Hash> BlockchainCache::getBlockHashes(uint32_t startBlockIndex, size_t maxCount) const {
  size_t blocksLeft;
  size_t start = 0;
  std::vector<Crypto::Hash> hashes;

  if (startBlockIndex < startIndex) {
    assert(parent != nullptr);
    hashes = parent->getBlockHashes(startBlockIndex, maxCount);
    blocksLeft = std::min(maxCount - hashes.size(), blockInfos.size());
  } else {
    start = startBlockIndex - startIndex;
    blocksLeft = std::min(blockInfos.size() - start, maxCount);
  }

  for (auto i = start; i < start + blocksLeft; ++i) {
    hashes.push_back(blockInfos.get<BlockIndexTag>()[i].blockHash);
  }

  return hashes;
}

IBlockchainCache* BlockchainCache::getParent() const { return parent; }

void BlockchainCache::setParent(IBlockchainCache* p) { parent = p; }

uint32_t BlockchainCache::getStartBlockIndex() const { return startIndex; }

size_t BlockchainCache::getKeyOutputsCountForAmount(uint64_t amount, uint32_t blockIndex) const {
  auto it = keyOutputsGlobalIndexes.find(amount);
  if (it == keyOutputsGlobalIndexes.end()) {
    if (parent == nullptr) {
      return 0;
    }

    return parent->getKeyOutputsCountForAmount(amount, blockIndex);
  }

  auto lowerBound = std::lower_bound(
      it->second.outputs.begin(), it->second.outputs.end(), blockIndex,
      [](const PackedOutIndex& output, uint32_t blockIndex) { return output.data.blockIndex < blockIndex; });

  return it->second.startIndex + static_cast<size_t>(std::distance(it->second.outputs.begin(), lowerBound));
}

uint32_t BlockchainCache::getTimestampLowerBoundBlockIndex(uint64_t timestamp) const {
  assert(!blockInfos.empty());

  auto& index = blockInfos.get<BlockIndexTag>();
  if (index.back().timestamp < timestamp) {
    // we don't have it
    throw std::runtime_error("no blocks for this timestamp, too large");
  }

  if (index.front().timestamp < timestamp) {
    // we know the timestamp is in current segment for sure
    auto bound =
        std::lower_bound(index.begin(), index.end(), timestamp,
                         [](const CachedBlockInfo& blockInfo, uint64_t value) { return blockInfo.timestamp < value; });

    return startIndex + static_cast<uint32_t>(std::distance(index.begin(), bound));
  }

  // if index.front().timestamp >= timestamp we can't be sure the timestamp is in current segment
  // so we ask parent. If it doesn't have it then index.front() is the block being searched for.

  if (parent == nullptr) {
    // if given timestamp is less or equal genesis block timestamp
    return 0;
  }

  try {
    uint32_t blockIndex = parent->getTimestampLowerBoundBlockIndex(timestamp);
    return blockIndex != INVALID_BLOCK_INDEX ? blockIndex : startIndex;
  } catch (std::runtime_error&) {
    return startIndex;
  }
}

bool BlockchainCache::getTransactionGlobalIndexes(const Crypto::Hash& transactionHash,
                                                  std::vector<uint32_t>& globalIndexes) const {
  auto it = transactions.get<TransactionHashTag>().find(transactionHash);
  if (it == transactions.get<TransactionHashTag>().end()) {
    return false;
  }

  globalIndexes = it->globalIndexes;
  return true;
}

size_t BlockchainCache::getTransactionCount() const {
  size_t count = 0;

  if (parent != nullptr) {
    count = parent->getTransactionCount();
  }

  count += transactions.size();
  return count;
}

RawBlock BlockchainCache::getBlockByIndex(uint32_t index) const {
  return index < startIndex ? parent->getBlockByIndex(index) : storage->getBlockByIndex(index - startIndex);
}

RawBlockVector BlockchainCache::getBlocks(ConstBlockHeightSpan heights) const {
  using namespace Xi;

  RawBlockVector reval{};
  reval.reserve(heights.size());

  for (const auto& height : heights) {
    exceptional_if<InvalidArgumentError>(height.isNull(), "null height provided");
    const auto index = height.toIndex();
    exceptional_if<OutOfRangeError>(index < startIndex || index + 1 > startIndex + storage->getBlockCount(),
                                    "height out of range");
    reval.emplace_back(storage->getBlockByIndex(index - startIndex));
  }

  return reval;
}

CachedBlockInfoVector BlockchainCache::getBlockInfos(ConstBlockHeightSpan heights) const {
  using namespace Xi;

  CachedBlockInfoVector reval{};
  reval.resize(heights.size());

  const auto topBlockIndex = getTopBlockIndex();

  size_t i = 0;
  for (const auto& height : heights) {
    exceptional_if<InvalidArgumentError>(height.isNull(), "null height provided");

    const auto index = height.toIndex();
    exceptional_if<OutOfRangeError>(index < startIndex || index > topBlockIndex, "height out of range");

    reval[i++] = blockInfos.get<BlockIndexTag>().at(index - startIndex);
  }

  return reval;
}

CachedTransactionVector BlockchainCache::getTransactions(ConstTransactionHashSpan ids) const {
  using namespace Xi;

  CachedTransactionVector reval{};
  reval.reserve(ids.size());

  auto& index = transactions.get<TransactionHashTag>();
  for (const auto& id : ids) {
    auto it = index.find(id);
    exceptional_if<NotFoundError>(it == index.end(), "transaction not contained by cache");
    const auto blockIndex = it->blockIndex;
    exceptional_if<OutOfRangeError>(blockIndex < startIndex || blockIndex > getTopBlockIndex());
    CachedRawBlock rawBlock{storage->getBlockByIndex(blockIndex - startIndex)};

    const auto transactionIndex = it->transactionIndex;
    if (transactionIndex == 0) {
      reval.emplace_back(rawBlock.block().coinbase());
      continue;
    } else {
      size_t transactionOffset = 1;
      if (rawBlock.block().hasStaticReward()) {
        transactionOffset += 1;
        if (transactionIndex == 1) {
          reval.emplace_back(*currency.constructStaticRewardTx(rawBlock.block()).takeOrThrow());
          continue;
        }
      }
      const auto internalIndex = transactionIndex - transactionOffset;
      exceptional_if_not<OutOfRangeError>(internalIndex < rawBlock.transferCount());
      reval.emplace_back(rawBlock[internalIndex]);
      continue;
    }
  }

  return reval;
}

CachedTransactionInfoVector BlockchainCache::getTransactionInfos(ConstTransactionHashSpan ids) const {
  using namespace Xi;

  CachedTransactionInfoVector reval{};
  reval.resize(ids.size());

  const auto& txByHash = transactions.get<TransactionHashTag>();

  size_t i = 0;
  for (const auto& id : ids) {
    auto search = txByHash.find(id);
    exceptional_if<NotFoundError>(search == txByHash.end(), "queried missing transaction hash");
    reval[i++] = *search;
  }

  return reval;
}

BinaryArray BlockchainCache::getRawTransaction(uint32_t index, uint32_t transactionIndex) const {
  if (index < startIndex) {
    return parent->getRawTransaction(index, transactionIndex);
  } else {
    auto rawBlock = storage->getBlockByIndex(index - startIndex);
    auto block = fromBinaryArray<BlockTemplate>(rawBlock.blockTemplate);
    const bool hasStaticReward = currency.isStaticRewardEnabledForBlockVersion(block.version);
    if (transactionIndex == 0) {
      return toBinaryArray(block.baseTransaction);
    } else if (transactionIndex == 1 && hasStaticReward) {
      return toBinaryArray(*currency.constructStaticRewardTx(block).takeOrThrow());
    } else {
      const size_t staticTransactionsOffset = hasStaticReward ? 2 : 1;
      assert(rawBlock.transactions.size() >= transactionIndex - staticTransactionsOffset);
      return rawBlock.transactions[transactionIndex - staticTransactionsOffset];
    }
  }
}

std::vector<BinaryArray> BlockchainCache::getRawTransactions(
    const std::vector<Crypto::Hash>& requestedTransactions) const {
  std::vector<Crypto::Hash> misses;
  auto ret = getRawTransactions(requestedTransactions, misses);
  assert(misses.empty());
  return ret;
}

std::vector<BinaryArray> BlockchainCache::getRawTransactions(const std::vector<Crypto::Hash>& requestedTransactions,
                                                             std::vector<Crypto::Hash>& missedTransactions) const {
  std::vector<BinaryArray> res;
  getRawTransactions(requestedTransactions, res, missedTransactions);
  return res;
}

void BlockchainCache::getRawTransactions(const std::vector<Crypto::Hash>& requestedTransactions,
                                         std::vector<BinaryArray>& foundTransactions,
                                         std::vector<Crypto::Hash>& missedTransactions) const {
  auto& index = transactions.get<TransactionHashTag>();
  for (const auto& transactionHash : requestedTransactions) {
    auto it = index.find(transactionHash);
    if (it == index.end()) {
      missedTransactions.emplace_back(transactionHash);
    } else {
      // assert(startIndex <= it->blockIndex);
      foundTransactions.emplace_back(getRawTransaction(it->blockIndex, it->transactionIndex));
    }
  }
}

size_t BlockchainCache::getChildCount() const { return children.size(); }

void BlockchainCache::addChild(IBlockchainCache* child) {
  assert(std::find(children.begin(), children.end(), child) == children.end());
  children.push_back(child);
}

bool BlockchainCache::deleteChild(IBlockchainCache* child) {
  auto it = std::find(children.begin(), children.end(), child);
  if (it == children.end()) {
    return false;
  }

  children.erase(it);
  return true;
}

bool BlockchainCache::serialize(ISerializer& s) {
  assert(s.type() == ISerializer::OUTPUT);

  uint32_t version = CURRENT_SERIALIZATION_VERSION;

  XI_RETURN_EC_IF_NOT(s(version, "version"), false);

  if (s.type() == ISerializer::OUTPUT) {
    XI_RETURN_EC_IF_NOT(
        writeSequence<CachedTransactionInfo>(transactions.begin(), transactions.end(), "transactions", s), false);
    XI_RETURN_EC_IF_NOT(
        writeSequence<SpentKeyImage>(spentKeyImages.begin(), spentKeyImages.end(), "spent_key_images", s), false);
    XI_RETURN_EC_IF_NOT(writeSequence<CachedBlockInfo>(blockInfos.begin(), blockInfos.end(), "block_hash_indexes", s),
                        false);
    XI_RETURN_EC_IF_NOT(
        writeSequence<PaymentIdTransactionHashPair>(paymentIds.begin(), paymentIds.end(), "payment_id_indexes", s),
        false);
    XI_RETURN_EC_IF_NOT(s(keyOutputsGlobalIndexes, "key_outputs_global_indexes"), false);
    return true;
  } else {
    TransactionsCacheContainer restoredTransactions;
    SpentKeyImagesContainer restoredSpentKeyImages;
    BlockInfoContainer restoredBlockHashIndex;
    OutputsGlobalIndexesContainer restoredKeyOutputsGlobalIndexes;
    PaymentIdContainer restoredPaymentIds;

    XI_RETURN_EC_IF_NOT(readSequence<CachedTransactionInfo>(
                            std::inserter(restoredTransactions, restoredTransactions.end()), "transactions", s),
                        false);
    XI_RETURN_EC_IF_NOT(readSequence<SpentKeyImage>(std::inserter(restoredSpentKeyImages, restoredSpentKeyImages.end()),
                                                    "spent_key_images", s),
                        false);
    XI_RETURN_EC_IF_NOT(
        readSequence<CachedBlockInfo>(std::back_inserter(restoredBlockHashIndex), "block_hash_indexes", s), false);
    XI_RETURN_EC_IF_NOT(readSequence<PaymentIdTransactionHashPair>(
                            std::inserter(restoredPaymentIds, restoredPaymentIds.end()), "payment_id_indexes", s),
                        false);

    XI_RETURN_EC_IF_NOT(s(restoredKeyOutputsGlobalIndexes, "key_outputs_global_indexes"), false);

    transactions = std::move(restoredTransactions);
    spentKeyImages = std::move(restoredSpentKeyImages);
    blockInfos = std::move(restoredBlockHashIndex);
    keyOutputsGlobalIndexes = std::move(restoredKeyOutputsGlobalIndexes);
    paymentIds = std::move(restoredPaymentIds);
    return true;
  }
}

bool BlockchainCache::save() {
  std::ofstream file(filename.c_str());
  Common::StdOutputStream stream(file);
  CryptoNote::BinaryOutputStreamSerializer s(stream);
  return serialize(s);
}

bool BlockchainCache::load() {
  std::ifstream file(filename.c_str());
  Common::StdInputStream stream(file);
  CryptoNote::BinaryInputStreamSerializer s(stream);
  return serialize(s);
}

ExtractOutputKeysResult BlockchainCache::extractKeyOutputKeys(uint64_t amount,
                                                              Common::ArrayView<uint32_t> globalIndexes,
                                                              std::vector<Crypto::PublicKey>& publicKeys) const {
  return extractKeyOutputKeys(amount, getTopBlockIndex(), globalIndexes, publicKeys);
}

std::vector<uint32_t> BlockchainCache::getRandomOutsByAmount(Amount amount, size_t count, uint32_t blockIndex) const {
  std::vector<uint32_t> offs;
  auto it = keyOutputsGlobalIndexes.find(amount);
  if (it == keyOutputsGlobalIndexes.end()) {
    return parent != nullptr ? parent->getRandomOutsByAmount(amount, count, blockIndex) : offs;
  }

  auto& outs = it->second.outputs;
  auto end = std::find_if(outs.rbegin(), outs.rend(), [&](PackedOutIndex index) {
               return index.data.blockIndex <= blockIndex - currency.minedMoneyUnlockWindow();
             }).base();
  uint32_t dist = static_cast<uint32_t>(std::distance(outs.begin(), end));
  dist = std::min(static_cast<uint32_t>(count), dist);
  ShuffleGenerator<uint32_t, Xi::Crypto::Random::Engine<uint32_t>> generator(dist);
  while (dist--) {
    auto offset = generator();
    auto& outIndex = it->second.outputs[offset];
    auto transactionIterator = transactions.get<TransactionInBlockTag>().find(
        boost::make_tuple<uint32_t, uint32_t>(outIndex.data.blockIndex, outIndex.data.transactionIndex));
    if (isTransactionSpendTimeUnlocked(transactionIterator->unlockTime, blockIndex)) {
      offs.push_back(it->second.startIndex + offset);
    }
  }

  if (offs.size() < count && parent != nullptr) {
    auto prevs = parent->getRandomOutsByAmount(amount, count - offs.size(), blockIndex);
    offs.reserve(prevs.size() + offs.size());
    std::copy(prevs.begin(), prevs.end(), std::back_inserter(offs));
  }

  return offs;
}

ExtractOutputKeysResult BlockchainCache::extractKeyOutputKeys(uint64_t amount, uint32_t blockIndex,
                                                              Common::ArrayView<uint32_t> globalIndexes,
                                                              std::vector<Crypto::PublicKey>& publicKeys) const {
  const auto timestamp = time(nullptr);
  if (timestamp < 0) {
    return ExtractOutputKeysResult::TIME_PROVIDER_FAILED;
  } else {
    return extractKeyOutputKeys(amount, blockIndex, globalIndexes, publicKeys, static_cast<uint64_t>(timestamp));
  }
}

ExtractOutputKeysResult BlockchainCache::extractKeyOutputKeys(uint64_t amount, uint32_t blockIndex,
                                                              Common::ArrayView<uint32_t> globalIndexes,
                                                              std::vector<Crypto::PublicKey>& publicKeys,
                                                              uint64_t timestamp) const {
  assert(!globalIndexes.isEmpty());
  assert(std::is_sorted(globalIndexes.begin(), globalIndexes.end()));                             // sorted
  assert(std::adjacent_find(globalIndexes.begin(), globalIndexes.end()) == globalIndexes.end());  // unique

  return extractKeyOutputs(amount, blockIndex, globalIndexes,
                           [&](const CachedTransactionInfo& info, PackedOutIndex index, uint32_t globalIndex) {
                             XI_UNUSED(globalIndex);
                             if (!isTransactionSpendTimeUnlocked(info.unlockTime, blockIndex, timestamp)) {
                               return ExtractOutputKeysResult::OUTPUT_LOCKED;
                             }

                             const auto& keyOutput = std::get<KeyOutput>(info.outputs[index.data.outputIndex]);
                             publicKeys.push_back(keyOutput.key);
                             return ExtractOutputKeysResult::SUCCESS;
                           });
}

ExtractOutputKeysResult BlockchainCache::extractKeyOtputReferences(
    uint64_t amount, Common::ArrayView<uint32_t> globalIndexes,
    std::vector<std::pair<Crypto::Hash, size_t>>& outputReferences) const {
  assert(!globalIndexes.isEmpty());
  assert(std::is_sorted(globalIndexes.begin(), globalIndexes.end()));                             // sorted
  assert(std::adjacent_find(globalIndexes.begin(), globalIndexes.end()) == globalIndexes.end());  // unique

  return extractKeyOutputs(amount, getTopBlockIndex(), globalIndexes,
                           [&](const CachedTransactionInfo& info, PackedOutIndex index, uint32_t globalIndex) {
                             XI_UNUSED(globalIndex);
                             outputReferences.push_back(std::make_pair(info.transactionHash, index.data.outputIndex));
                             return ExtractOutputKeysResult::SUCCESS;
                           });
}

// TODO: start from index
ExtractOutputKeysResult BlockchainCache::extractKeyOutputs(
    uint64_t amount, uint32_t blockIndex, Common::ArrayView<uint32_t> globalIndexes,
    std::function<ExtractOutputKeysResult(const CachedTransactionInfo& info, PackedOutIndex index,
                                          uint32_t globalIndex)>
        pred) const {
  assert(!globalIndexes.isEmpty());
  assert(std::is_sorted(globalIndexes.begin(), globalIndexes.end()));                             // sorted
  assert(std::adjacent_find(globalIndexes.begin(), globalIndexes.end()) == globalIndexes.end());  // unique

  auto globalIndexesIterator = keyOutputsGlobalIndexes.find(amount);
  if (globalIndexesIterator == keyOutputsGlobalIndexes.end() || blockIndex < startIndex) {
    return parent != nullptr ? parent->extractKeyOutputs(amount, blockIndex, globalIndexes, std::move(pred))
                             : ExtractOutputKeysResult::INVALID_GLOBAL_INDEX;
  }

  auto startGlobalIndex = globalIndexesIterator->second.startIndex;
  auto parentIndexesIterator = std::lower_bound(globalIndexes.begin(), globalIndexes.end(), startGlobalIndex);

  auto offset = std::distance(globalIndexes.begin(), parentIndexesIterator);
  if (parentIndexesIterator != globalIndexes.begin()) {
    assert(parent != nullptr);
    auto result = parent->extractKeyOutputs(amount, blockIndex,
                                            globalIndexes.head(parentIndexesIterator - globalIndexes.begin()), pred);
    if (result != ExtractOutputKeysResult::SUCCESS) {
      return result;
    }
  }

  auto myGlobalIndexes = globalIndexes.unhead(offset);
  auto& outputs = globalIndexesIterator->second.outputs;
  assert(!outputs.empty());
  for (auto globalIndex : myGlobalIndexes) {
    if (globalIndex - startGlobalIndex >= outputs.size()) {
      logger(Logging::Debugging) << "Couldn't extract key output for amount " << amount << " with global index "
                                 << globalIndex << " because global index is greater than the last available: "
                                 << (startGlobalIndex + outputs.size());
      return ExtractOutputKeysResult::INVALID_GLOBAL_INDEX;
    }

    auto outputIndex = outputs[globalIndex - startGlobalIndex];

    assert(outputIndex.data.blockIndex >= startIndex);
    assert(outputIndex.data.blockIndex <= blockIndex);

    auto txIt = transactions.get<TransactionInBlockTag>().find(
        boost::make_tuple<uint32_t, uint32_t>(outputIndex.data.blockIndex, outputIndex.data.transactionIndex));
    if (txIt == transactions.get<TransactionInBlockTag>().end()) {
      logger(Logging::Debugging) << "Couldn't extract key output for amount " << amount << " with global index "
                                 << globalIndex << " because containing transaction doesn't exist in index "
                                 << "(block index: " << outputIndex.data.blockIndex
                                 << ", transaction index: " << outputIndex.data.transactionIndex << ")";
      return ExtractOutputKeysResult::INVALID_GLOBAL_INDEX;
    }

    auto ret = pred(*txIt, outputIndex, globalIndex);
    if (ret != ExtractOutputKeysResult::SUCCESS) {
      logger(Logging::Debugging) << "Couldn't extract key output for amount " << amount << " with global index "
                                 << globalIndex << " because callback returned fail status (block index: "
                                 << outputIndex.data.blockIndex
                                 << ", transaction index: " << outputIndex.data.transactionIndex << ")";
      return ret;
    }
  }

  return ExtractOutputKeysResult::SUCCESS;
}

std::vector<Crypto::Hash> BlockchainCache::getTransactionHashesByPaymentId(const PaymentId& paymentId) const {
  std::vector<Crypto::Hash> transactionHashes;

  if (parent != nullptr) {
    transactionHashes = parent->getTransactionHashesByPaymentId(paymentId);
  }

  auto& index = paymentIds.get<PaymentIdTag>();
  auto range = index.equal_range(paymentId);

  transactionHashes.reserve(transactionHashes.size() + std::distance(range.first, range.second));
  for (auto it = range.first; it != range.second; ++it) {
    transactionHashes.emplace_back(it->transactionHash);
  }

  logger(Logging::Debugging) << "Found " << transactionHashes.size() << " transactions with payment id " << paymentId;
  return transactionHashes;
}

std::vector<Crypto::Hash> BlockchainCache::getBlockHashesByTimestamps(uint64_t timestampBegin,
                                                                      size_t secondsCount) const {
  std::vector<Crypto::Hash> blockHashes;
  if (secondsCount == 0) {
    return blockHashes;
  }

  if (parent != nullptr) {
    blockHashes = parent->getBlockHashesByTimestamps(timestampBegin, secondsCount);
  }

  auto& index = blockInfos.get<TimestampTag>();
  auto begin = index.lower_bound(timestampBegin);
  auto end = index.upper_bound(timestampBegin + static_cast<uint64_t>(secondsCount) - 1);

  blockHashes.reserve(blockHashes.size() + std::distance(begin, end));
  for (auto it = begin; it != end; ++it) {
    blockHashes.push_back(it->blockHash);
  }

  logger(Logging::Debugging) << "Found " << blockHashes.size() << " within timestamp interval "
                             << "[" << timestampBegin << ":" << (timestampBegin + secondsCount) << "]";
  return blockHashes;
}

ExtractOutputKeysResult BlockchainCache::extractKeyOtputIndexes(uint64_t amount,
                                                                Common::ArrayView<uint32_t> globalIndexes,
                                                                std::vector<PackedOutIndex>& outIndexes) const {
  assert(!globalIndexes.isEmpty());
  return extractKeyOutputs(
      amount, getTopBlockIndex(), globalIndexes,
      [&outIndexes](const CachedTransactionInfo& info, PackedOutIndex index, uint32_t globalIndex) {
        XI_UNUSED(info, globalIndex);
        outIndexes.push_back(index);
        return ExtractOutputKeysResult::SUCCESS;
      });
}

uint64_t BlockchainCache::getAvailableMixinsCount(IBlockchainCache::Amount amount, uint32_t blockIndex,
                                                  uint64_t threshold) const {
  std::vector<uint32_t> offs;
  auto it = keyOutputsGlobalIndexes.find(amount);
  if (it == keyOutputsGlobalIndexes.end()) {
    return parent != nullptr ? parent->getAvailableMixinsCount(amount, blockIndex, threshold) : 0;
  }

  auto& outs = it->second.outputs;
  auto end = std::find_if(outs.rbegin(), outs.rend(), [&](PackedOutIndex index) {
               return index.data.blockIndex <= blockIndex - currency.minedMoneyUnlockWindow();
             }).base();
  uint64_t dist = static_cast<uint64_t>(std::distance(outs.begin(), end));
  if (dist >= threshold) {
    return dist;
  } else {
    return parent != nullptr ? dist + parent->getAvailableMixinsCount(amount, blockIndex, threshold - dist) : dist;
  }
}

uint32_t BlockchainCache::getTopBlockIndex() const {
  assert(!blockInfos.empty());
  return startIndex + static_cast<uint32_t>(blockInfos.size()) - 1;
}

const Crypto::Hash& BlockchainCache::getTopBlockHash() const {
  assert(!blockInfos.empty());
  return blockInfos.get<BlockIndexTag>().back().blockHash;
}

BlockVersion BlockchainCache::getTopBlockVersion() const {
  assert(!blockInfos.empty());
  return blockInfos.get<BlockIndexTag>().back().version;
}

std::vector<uint64_t> BlockchainCache::getLastUnits(size_t count, uint32_t blockIndex, UseGenesis useGenesis,
                                                    std::function<uint64_t(const CachedBlockInfo&)> pred) const {
  XI_RETURN_EC_IF(blockIndex > getTopBlockIndex(), {});

  size_t to = blockIndex < startIndex ? 0 : blockIndex - startIndex + 1;
  auto realCount = std::min(count, to);
  auto from = to - realCount;
  if (!useGenesis && from == 0 && realCount != 0 && parent == nullptr) {
    from += 1;
    realCount -= 1;
  }

  auto& blocksIndex = blockInfos.get<BlockIndexTag>();

  std::vector<uint64_t> result;
  if (realCount < count && parent != nullptr) {
    result =
        parent->getLastUnits(count - realCount, std::min(blockIndex, parent->getTopBlockIndex()), useGenesis, pred);
  }

  std::transform(std::next(blocksIndex.begin(), from), std::next(blocksIndex.begin(), to), std::back_inserter(result),
                 std::move(pred));
  return result;
}

TransactionValidatorState BlockchainCache::fillOutputsSpentByBlock(uint32_t blockIndex) const {
  TransactionValidatorState spentOutputs;
  auto& keyImagesIndex = spentKeyImages.get<BlockIndexTag>();

  auto range = keyImagesIndex.equal_range(blockIndex);
  for (auto it = range.first; it != range.second; ++it) {
    spentOutputs.spentKeyImages.insert(it->keyImage);
  }

  return spentOutputs;
}

bool BlockchainCache::hasTransaction(const Crypto::Hash& transactionHash) const {
  auto& index = transactions.get<TransactionHashTag>();
  auto it = index.find(transactionHash);
  return it != index.end();
}

uint32_t BlockchainCache::getBlockIndexContainingTx(const Crypto::Hash& transactionHash) const {
  auto& index = transactions.get<TransactionHashTag>();
  auto it = index.find(transactionHash);
  assert(it != index.end());
  return it->blockIndex;
}

BlockVersion BlockchainCache::getBlockVersionForHeight(uint32_t height) const {
  // TODO this only changes during compile time
  UpgradeManager upgradeManager;
  for (auto version : Xi::Config::BlockVersion::versions())
    upgradeManager.addBlockVersion(version, currency.upgradeHeight(version));
  return upgradeManager.getBlockVersion(height);
}

void BlockchainCache::fixChildrenParent(IBlockchainCache* p) {
  for (auto child : children) {
    child->setParent(p);
  }
}

}  // namespace CryptoNote
