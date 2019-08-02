// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include <CryptoNoteCore/DatabaseBlockchainCache.h>

#include <ctime>
#include <cstdlib>
#include <deque>
#include <string>
#include <tuple>
#include <set>
#include <algorithm>
#include <map>
#include <utility>
#include <vector>
#include <limits>

#include <boost/iterator/iterator_facade.hpp>

#include <Xi/Global.hh>
#include <Xi/Crypto/Random/Engine.hpp>
#include <Common/ShuffleGenerator.h>

#include "BlockchainUtils.h"

#include <CryptoNoteCore/BlockchainStorage.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/CryptoNoteBasicImpl.h>
#include "CryptoNoteCore/Transactions/TransactionExtra.h"

namespace CryptoNote {

namespace {

const uint32_t ONE_DAY_SECONDS = 60 * 60 * 24;
const CachedBlockInfo NULL_CACHED_BLOCK_INFO{Crypto::Hash::Null, BlockVersion::Null, BlockFeature::None, 0, 0, 0, 0, 0};

bool requestPackedOutputs(IBlockchainCache::Amount amount, Common::ArrayView<uint32_t> globalIndexes,
                          IDataBase& database, std::vector<PackedOutIndex>& result) {
  BlockchainReadBatch readBatch;
  result.reserve(result.size() + globalIndexes.getSize());

  for (auto globalIndex : globalIndexes) {
    readBatch.requestKeyOutputGlobalIndexForAmount(amount, globalIndex);
  }

  auto dbResult = database.read(readBatch);
  if (dbResult) {
    XI_PRINT_EC("[%s:%i] Packed Outputs request failed: %s\n", __FILE__, __LINE__, dbResult.message().c_str());
    XI_RETURN_EC(false);
  }

  try {
    auto readResult = readBatch.extractResult();
    const auto& packedOutsMap = readResult.getKeyOutputGlobalIndexesForAmounts();
    for (auto globalIndex : globalIndexes) {
      result.push_back(packedOutsMap.at(std::make_pair(amount, globalIndex)));
    }
  } catch (std::exception& e) {
    (void)e;
    XI_PRINT_EC("[%s:%i] Packed Outputs request failed: %s\n", __FILE__, __LINE__, e.what());
    XI_RETURN_EC(false);
  }

  XI_RETURN_SC(true);
}

bool requestTransactionHashesForGlobalOutputIndexes(const std::vector<PackedOutIndex>& packedOuts, IDataBase& database,
                                                    std::vector<Crypto::Hash>& transactionHashes) {
  BlockchainReadBatch readHashesBatch;

  std::set<uint32_t> blockIndexes;
  std::for_each(packedOuts.begin(), packedOuts.end(),
                [&blockIndexes](PackedOutIndex out) { blockIndexes.insert(out.data.blockIndex); });
  std::for_each(blockIndexes.begin(), blockIndexes.end(), [&readHashesBatch](uint32_t blockIndex) {
    readHashesBatch.requestTransactionHashesByBlock(blockIndex);
  });

  auto dbResult = database.read(readHashesBatch);
  if (dbResult) {
    return false;
  }

  auto readResult = readHashesBatch.extractResult();
  const auto& transactionHashesMap = readResult.getTransactionHashesByBlocks();

  if (transactionHashesMap.size() != blockIndexes.size()) {
    return false;
  }

  transactionHashes.reserve(transactionHashes.size() + packedOuts.size());
  for (const auto& output : packedOuts) {
    if (output.data.transactionIndex >= transactionHashesMap.at(output.data.blockIndex).size()) {
      return false;
    }

    transactionHashes.push_back(transactionHashesMap.at(output.data.blockIndex)[output.data.transactionIndex]);
  }

  return true;
}

bool requestCachedTransactionInfos(const std::vector<Crypto::Hash>& transactionHashes, IDataBase& database,
                                   std::vector<CachedTransactionInfo>& result) {
  result.reserve(result.size() + transactionHashes.size());

  BlockchainReadBatch transactionsBatch;
  std::for_each(transactionHashes.begin(), transactionHashes.end(),
                [&transactionsBatch](const Crypto::Hash& hash) { transactionsBatch.requestCachedTransaction(hash); });
  auto dbResult = database.read(transactionsBatch);
  if (dbResult) {
    return false;
  }

  auto readResult = transactionsBatch.extractResult();
  const auto& transactions = readResult.getCachedTransactions();
  if (transactions.size() != transactionHashes.size()) {
    return false;
  }

  for (const auto& hash : transactionHashes) {
    result.push_back(transactions.at(hash));
  }

  return true;
}

// returns CachedTransactionInfos in the same or as packedOuts are
/*
bool requestCachedTransactionInfos(const std::vector<PackedOutIndex>& packedOuts, IDataBase& database,
std::vector<CachedTransactionInfo>& result) { std::vector<Crypto::Hash> transactionHashes; if
(!requestTransactionHashesForGlobalOutputIndexes(packedOuts, database, transactionHashes)) { return false;
  }

  return requestCachedTransactionInfos(transactionHashes, database, result);
}
*/

bool requestExtendedTransactionInfos(const std::vector<Crypto::Hash>& transactionHashes, IDataBase& database,
                                     std::vector<ExtendedTransactionInfo>& result) {
  result.reserve(result.size() + transactionHashes.size());

  BlockchainReadBatch transactionsBatch;
  std::for_each(transactionHashes.begin(), transactionHashes.end(),
                [&transactionsBatch](const Crypto::Hash& hash) { transactionsBatch.requestCachedTransaction(hash); });
  auto dbResult = database.read(transactionsBatch);
  if (dbResult) {
    return false;
  }

  auto readResult = transactionsBatch.extractResult();
  const auto& transactions = readResult.getCachedTransactions();

  std::unordered_set<Crypto::Hash> uniqueTransactionHashes(transactionHashes.begin(), transactionHashes.end());
  if (transactions.size() != uniqueTransactionHashes.size()) {
    return false;
  }

  for (const auto& hash : transactionHashes) {
    result.push_back(transactions.at(hash));
  }

  return true;
}

// returns ExtendedTransactionInfos in the same order as packedOuts are
bool requestExtendedTransactionInfos(const std::vector<PackedOutIndex>& packedOuts, IDataBase& database,
                                     std::vector<ExtendedTransactionInfo>& result) {
  std::vector<Crypto::Hash> transactionHashes;
  if (!requestTransactionHashesForGlobalOutputIndexes(packedOuts, database, transactionHashes)) {
    return false;
  }

  return requestExtendedTransactionInfos(transactionHashes, database, result);
}

uint64_t roundToMidnight(uint64_t timestamp) {
  if (timestamp > static_cast<uint64_t>(std::numeric_limits<time_t>::max())) {
    throw std::runtime_error("Timestamp is too big");
  }

  return static_cast<uint64_t>((timestamp / ONE_DAY_SECONDS) * ONE_DAY_SECONDS);
}

std::pair<boost::optional<uint32_t>, bool> requestClosestBlockIndexByTimestamp(uint64_t timestamp,
                                                                               IDataBase& database) {
  std::pair<boost::optional<uint32_t>, bool> result = {{}, false};

  BlockchainReadBatch readBatch;
  readBatch.requestClosestTimestampBlockIndex(timestamp);
  auto dbResult = database.read(readBatch);
  if (dbResult) {
    return result;
  }

  result.second = true;
  auto readResult = readBatch.extractResult();
  if (readResult.getClosestTimestampBlockIndex().count(timestamp)) {
    result.first = readResult.getClosestTimestampBlockIndex().at(timestamp);
  }

  return result;
}

bool requestRawBlock(IDataBase& database, uint32_t blockIndex, RawBlock& block) {
  auto batch = BlockchainReadBatch().requestRawBlock(blockIndex);

  auto error = database.read(batch);
  if (error) {
    // may be throw in all similiar functions???
    return false;
  }

  auto result = batch.extractResult();
  if (result.getRawBlocks().count(blockIndex) == 0) {
    return false;
  }

  block = result.getRawBlocks().at(blockIndex);
  return true;
}

Transaction extractTransaction(const RawBlock& block, uint32_t blockIndex, uint32_t transactionIndex,
                               const Currency& currency) {
  XI_UNUSED(blockIndex);
  assert(transactionIndex < block.transactions.size() + 2);

  BlockTemplate blockTemplate;
  bool r = fromBinaryArray(blockTemplate, block.blockTemplate);
  assert(r);
  XI_UNUSED(r);

  const bool hasStaticReward = currency.isStaticRewardEnabledForBlockVersion(blockTemplate.version);
  if (transactionIndex == 0) {
    return blockTemplate.baseTransaction;
  } else if (hasStaticReward && transactionIndex == 1) {
    return *currency.constructStaticRewardTx(blockTemplate).takeOrThrow();
  } else {
    const size_t staticTransactionsOffset = hasStaticReward ? 2 : 1;
    assert(transactionIndex < block.transactions.size() + staticTransactionsOffset);
    Transaction transaction;
    bool ir = fromBinaryArray(transaction, block.transactions[transactionIndex - staticTransactionsOffset]);
    assert(ir);
    XI_UNUSED(ir);
    return transaction;
  }
}

size_t requestPaymentIdTransactionsCount(IDataBase& database, const PaymentId& paymentId) {
  auto batch = BlockchainReadBatch().requestTransactionCountByPaymentId(paymentId);
  auto error = database.read(batch);
  if (error) {
    throw std::system_error(error, "Error while reading transactions count by payment id");
  }

  auto result = batch.extractResult();
  if (result.getTransactionCountByPaymentIds().count(paymentId) == 0) {
    return 0;
  }

  return result.getTransactionCountByPaymentIds().at(paymentId);
}

bool requestPaymentId(IDataBase& database, const Crypto::Hash& transactionHash, PaymentId& paymentId,
                      const Currency& currency) {
  std::vector<CachedTransactionInfo> cachedTransactions;

  if (!requestCachedTransactionInfos({transactionHash}, database, cachedTransactions)) {
    return false;
  }

  if (cachedTransactions.empty()) {
    return false;
  }

  RawBlock block;
  if (!requestRawBlock(database, cachedTransactions[0].blockIndex, block)) {
    return false;
  }

  Transaction transaction =
      extractTransaction(block, cachedTransactions[0].blockIndex, cachedTransactions[0].transactionIndex, currency);
  return getPaymentIdFromTxExtra(transaction.extra, paymentId);
}

uint32_t requestKeyOutputGlobalIndexesCountForAmount(IBlockchainCache::Amount amount, IDataBase& database) {
  auto batch = BlockchainReadBatch().requestKeyOutputGlobalIndexesCountForAmount(amount);
  auto dbError = database.read(batch);
  if (dbError) {
    throw std::system_error(dbError, "Cannot perform requestKeyOutputGlobalIndexesCountForAmount query");
  }

  auto result = batch.extractResult();

  if (result.getKeyOutputGlobalIndexesCountForAmounts().count(amount) != 0) {
    return result.getKeyOutputGlobalIndexesCountForAmounts().at(amount);
  } else {
    return 0;
  }
}

class DbOutputConstIterator
    : public boost::iterator_facade<DbOutputConstIterator, const PackedOutIndex,
                                    boost::random_access_traversal_tag /*boost::forward_traversal_tag*/> {
 public:
  DbOutputConstIterator(
      std::function<PackedOutIndex(IBlockchainCache::Amount amount, uint32_t globalOutputIndex)> retriever_,
      IBlockchainCache::Amount amount_, uint32_t globalOutputIndex_)
      : retriever(retriever_), amount(amount_), globalOutputIndex(globalOutputIndex_) {
  }

  const PackedOutIndex& dereference() const {
    cachedValue = retriever(amount, globalOutputIndex);
    return cachedValue;
  }

  bool equal(const DbOutputConstIterator& other) const {
    return globalOutputIndex == other.globalOutputIndex;
  }

  void increment() {
    ++globalOutputIndex;
  }

  void decrement() {
    --globalOutputIndex;
  }

  void advance(difference_type n) {
    assert(n >= -static_cast<difference_type>(globalOutputIndex));
    globalOutputIndex += static_cast<uint32_t>(n);
  }

  difference_type distance_to(const DbOutputConstIterator& to) const {
    return static_cast<difference_type>(to.globalOutputIndex) - static_cast<difference_type>(globalOutputIndex);
  }

 private:
  std::function<PackedOutIndex(IBlockchainCache::Amount amount, uint32_t globalOutputIndex)> retriever;
  IBlockchainCache::Amount amount;
  uint32_t globalOutputIndex;
  mutable PackedOutIndex cachedValue;
};

PackedOutIndex retrieveKeyOutput(IBlockchainCache::Amount amount, uint32_t globalOutputIndex, IDataBase& database) {
  BlockchainReadBatch batch;
  auto dbError = database.read(batch.requestKeyOutputGlobalIndexForAmount(amount, globalOutputIndex));
  if (dbError) {
    throw std::system_error(dbError, "Error during retrieving key output by global output index");
  }

  auto result = batch.extractResult();

  try {
    return result.getKeyOutputGlobalIndexesForAmounts().at(std::make_pair(amount, globalOutputIndex));
  } catch (std::exception&) {
    assert(false);
    throw std::runtime_error("Couldn't find key output for amount " + std::to_string(amount) +
                             " with global output index " + std::to_string(globalOutputIndex));
  }
}

std::map<IBlockchainCache::Amount, IBlockchainCache::GlobalOutputIndex> getMinGlobalIndexesByAmount(
    const std::map<IBlockchainCache::Amount, std::vector<IBlockchainCache::GlobalOutputIndex>>& outputIndexes) {
  std::map<IBlockchainCache::Amount, IBlockchainCache::GlobalOutputIndex> minIndexes;
  for (const auto& kv : outputIndexes) {
    auto min = std::min_element(kv.second.begin(), kv.second.end());
    if (min == kv.second.end()) {
      continue;
    }

    minIndexes.emplace(kv.first, *min);
  }

  return minIndexes;
}

void mergeOutputsSplitBoundaries(std::map<IBlockchainCache::Amount, IBlockchainCache::GlobalOutputIndex>& dest,
                                 const std::map<IBlockchainCache::Amount, IBlockchainCache::GlobalOutputIndex>& src) {
  for (const auto& elem : src) {
    auto it = dest.find(elem.first);
    if (it == dest.end()) {
      dest.emplace(elem.first, elem.second);
      continue;
    }

    if (it->second > elem.second) {
      it->second = elem.second;
    }
  }
}

void cutTail(std::deque<CachedBlockInfo>& cache, size_t count) {
  if (count >= cache.size()) {
    cache.clear();
    return;
  }

  cache.erase(std::next(cache.begin(), cache.size() - count), cache.end());
}

const std::string DB_VERSION_KEY = "db_scheme_version";

class DatabaseVersionReadBatch : public IReadBatch {
 public:
  virtual ~DatabaseVersionReadBatch() override {
  }

  virtual std::vector<std::string> getRawKeys() const override {
    return {DB_VERSION_KEY};
  }

  virtual void submitRawResult(const std::vector<std::string>& values, const std::vector<bool>& resultStates) override {
    assert(values.size() == 1);
    assert(resultStates.size() == values.size());

    if (!resultStates[0]) {
      return;
    }

    version = static_cast<uint32_t>(std::atoi(values[0].c_str()));
  }

  boost::optional<uint32_t> getDbSchemeVersion() {
    return version;
  }

 private:
  boost::optional<uint32_t> version;
};

class DatabaseVersionWriteBatch : public IWriteBatch {
 public:
  DatabaseVersionWriteBatch(uint32_t version) : schemeVersion(version) {
  }
  virtual ~DatabaseVersionWriteBatch() {
  }

  virtual std::vector<std::pair<std::string, std::string>> extractRawDataToInsert() override {
    return {make_pair(DB_VERSION_KEY, std::to_string(schemeVersion))};
  }

  virtual std::vector<std::string> extractRawKeysToRemove() override {
    return {};
  }

 private:
  uint32_t schemeVersion;
};

const uint32_t CURRENT_DB_SCHEME_VERSION = 2;

}  // namespace

struct DatabaseBlockchainCache::ExtendedPushedBlockInfo {
  PushedBlockInfo pushedBlockInfo;
  uint64_t timestamp;
};

DatabaseBlockchainCache::DatabaseBlockchainCache(const Currency& curr, IDataBase& dataBase,
                                                 IBlockchainCacheFactory& blockchainCacheFactory,
                                                 Logging::ILogger& _logger)
    : CommonBlockchainCache(_logger, curr),
      currency(curr),
      database(dataBase),
      blockchainCacheFactory(blockchainCacheFactory),
      logger(_logger, "DatabaseBlockchainCache") {
  DatabaseVersionReadBatch readBatch;
  auto ec = database.read(readBatch);
  if (ec) {
    throw std::system_error(ec);
  }

  auto version = readBatch.getDbSchemeVersion();
  if (!version) {
    logger(Logging::Debugging) << "DB scheme version not found, writing: " << CURRENT_DB_SCHEME_VERSION;

    DatabaseVersionWriteBatch writeBatch(CURRENT_DB_SCHEME_VERSION);
    auto writeError = database.write(writeBatch);
    if (writeError) {
      throw std::system_error(writeError);
    }
  } else {
    logger(Logging::Debugging) << "Current db scheme version: " << *version;
  }

  if (getTopBlockIndex() == 0) {
    logger(Logging::Debugging) << "top block index is nill, add genesis block";
    addGenesisBlock(CachedBlock{currency.genesisBlock()});
  }
}

bool DatabaseBlockchainCache::checkDBSchemeVersion(IDataBase& database, Logging::ILogger& _logger) {
  Logging::LoggerRef logger(_logger, "DatabaseBlockchainCache");

  DatabaseVersionReadBatch readBatch;
  auto ec = database.read(readBatch);
  if (ec) {
    throw std::system_error(ec);
  }

  auto version = readBatch.getDbSchemeVersion();
  if (!version) {
    // DB scheme version not found. Looks like it was just created.
    return true;
  } else if (*version < CURRENT_DB_SCHEME_VERSION) {
    logger(Logging::Warning) << "DB scheme version is less than expected. Expected version "
                             << CURRENT_DB_SCHEME_VERSION << ". Actual version " << *version
                             << ". DB will be destroyed and recreated from blocks.bin file.";
    return false;
  } else if (*version > CURRENT_DB_SCHEME_VERSION) {
    logger(Logging::Error) << "DB scheme version is greater than expected. Expected version "
                           << CURRENT_DB_SCHEME_VERSION << ". Actual version " << *version
                           << ". Please update your software.";
    throw std::runtime_error("DB scheme version is greater than expected");
  } else {
    return true;
  }
}

void DatabaseBlockchainCache::deleteClosestTimestampBlockIndex(BlockchainWriteBatch& writeBatch,
                                                               uint32_t splitBlockIndex) {
  auto batch = BlockchainReadBatch().requestCachedBlock(splitBlockIndex);
  auto blockResult = readDatabase(batch);
  auto timestamp = blockResult.getCachedBlocks().at(splitBlockIndex).timestamp;

  auto midnight = roundToMidnight(timestamp);
  auto timestampResult = requestClosestBlockIndexByTimestamp(midnight, database);
  if (!timestampResult.second) {
    logger(Logging::Error)
        << "deleteClosestTimestampBlockIndex error: get closest timestamp block index, database read failed";
    throw std::runtime_error("Couldn't get closest timestamp block index");
  }

  assert(bool(timestampResult.first));

  auto blockIndex = *timestampResult.first;
  assert(splitBlockIndex >= blockIndex);

  if (splitBlockIndex != blockIndex) {
    midnight += ONE_DAY_SECONDS;
  }

  BlockchainReadBatch midnightBatch;
  while (readDatabase(midnightBatch.requestClosestTimestampBlockIndex(midnight))
             .getClosestTimestampBlockIndex()
             .count(midnight)) {
    writeBatch.removeClosestTimestampBlockIndex(midnight);
    midnight += ONE_DAY_SECONDS;
  }

  logger(Logging::Trace) << "deleted closest timestamp";
}

/*
 * This methods splits cache, upper part (ie blocks with indexes greater or equal to splitBlockIndex)
 * is copied to new BlockchainCache
 */
std::shared_ptr<IBlockchainCache> DatabaseBlockchainCache::split(uint32_t splitBlockIndex) {
  assert(splitBlockIndex <= getTopBlockIndex());
  logger(Logging::Debugging) << "split at index " << splitBlockIndex
                             << " started, top block index: " << getTopBlockIndex();

  auto cache = blockchainCacheFactory.createBlockchainCache(currency, this, splitBlockIndex);

  using DeleteBlockInfo = std::tuple<uint32_t, Crypto::Hash, TransactionValidatorState, uint64_t>;
  std::vector<DeleteBlockInfo> deletingBlocks;

  BlockchainWriteBatch writeBatch;
  auto currentTop = getTopBlockIndex();
  for (uint32_t blockIndex = splitBlockIndex; blockIndex <= currentTop; ++blockIndex) {
    ExtendedPushedBlockInfo extendedInfo = getExtendedPushedBlockInfo(blockIndex);

    auto validatorState = extendedInfo.pushedBlockInfo.validatorState;
    logger(Logging::Debugging) << "pushing block " << blockIndex << " to child segment";
    auto blockHash = pushBlockToAnotherCache(*cache, std::move(extendedInfo.pushedBlockInfo));

    deletingBlocks.emplace_back(blockIndex, blockHash, validatorState, extendedInfo.timestamp);
  }

  for (auto it = deletingBlocks.rbegin(); it != deletingBlocks.rend(); ++it) {
    auto blockIndex = std::get<0>(*it);
    auto blockHash = std::get<1>(*it);
    auto& validatorState = std::get<2>(*it);
    uint64_t timestamp = std::get<3>(*it);

    writeBatch.removeCachedBlock(blockHash, blockIndex).removeRawBlock(blockIndex);
    requestDeleteSpentOutputs(writeBatch, blockIndex, validatorState);
    requestRemoveTimestamp(writeBatch, timestamp, blockHash);
  }

  auto deletingTransactionHashes = requestTransactionHashesFromBlockIndex(splitBlockIndex);
  requestDeleteTransactions(writeBatch, deletingTransactionHashes);
  requestDeletePaymentIds(writeBatch, deletingTransactionHashes);

  std::vector<ExtendedTransactionInfo> extendedTransactions;
  if (!requestExtendedTransactionInfos(deletingTransactionHashes, database, extendedTransactions)) {
    logger(Logging::Error) << "Error while split: failed to request extended transaction info";
    throw std::runtime_error("failed to request extended transaction info");  // TODO: make error codes
  }

  std::map<IBlockchainCache::Amount, IBlockchainCache::GlobalOutputIndex> keyIndexSplitBoundaries;
  for (const auto& transaction : extendedTransactions) {
    auto txkeyBoundaries = getMinGlobalIndexesByAmount(transaction.amountToKeyIndexes);

    mergeOutputsSplitBoundaries(keyIndexSplitBoundaries, txkeyBoundaries);
  }

  requestDeleteKeyOutputs(writeBatch, keyIndexSplitBoundaries);

  deleteClosestTimestampBlockIndex(writeBatch, splitBlockIndex);

  logger(Logging::Debugging) << "Performing delete operations";
  // all data and indexes are now copied, no errors detected, can now erase data from database
  auto err = database.write(writeBatch);
  if (err) {
    logger(Logging::Error) << "split write failed, " << err.message();
    throw std::runtime_error(err.message());
  }

  cutTail(unitsCache, currentTop + 1 - splitBlockIndex);

  children.push_back(cache.get());
  logger(Logging::Trace) << "Delete successfull";

  // invalidate top block index and hash
  topBlockIndex = boost::none;
  topBlockHash = boost::none;
  topBlockVersion = boost::none;
  transactionsCount = boost::none;

  logger(Logging::Debugging) << "split completed";

  // return new cache
  return cache;
}

// returns hash of pushed block
Crypto::Hash DatabaseBlockchainCache::pushBlockToAnotherCache(IBlockchainCache& segment,
                                                              PushedBlockInfo&& pushedBlockInfo) {
  BlockTemplate block;
  [[maybe_unused]] bool br = fromBinaryArray(block, pushedBlockInfo.rawBlock.blockTemplate);
  assert(br);

  std::vector<CachedTransaction> transactions;
  [[maybe_unused]] bool tr = Utils::restoreCachedTransactions(pushedBlockInfo.rawBlock.transactions, transactions);
  assert(tr);

  CachedBlock cachedBlock(block);
  segment.pushBlock(cachedBlock, transactions, pushedBlockInfo.validatorState, pushedBlockInfo.blockSize,
                    pushedBlockInfo.generatedCoins, pushedBlockInfo.blockDifficulty,
                    std::move(pushedBlockInfo.rawBlock));

  return cachedBlock.getBlockHash();
}

std::vector<Crypto::Hash> DatabaseBlockchainCache::requestTransactionHashesFromBlockIndex(uint32_t splitBlockIndex) {
  logger(Logging::Debugging) << "Requesting transaction hashes starting from block index " << splitBlockIndex;

  BlockchainReadBatch readBatch;
  for (uint32_t blockIndex = splitBlockIndex; blockIndex <= getTopBlockIndex(); ++blockIndex) {
    readBatch.requestTransactionHashesByBlock(blockIndex);
  }

  std::vector<Crypto::Hash> transactionHashes;

  auto dbResult = readDatabase(readBatch);
  for (const auto& kv : dbResult.getTransactionHashesByBlocks()) {
    for (const auto& hash : kv.second) {
      transactionHashes.emplace_back(hash);
    }
  }

  return transactionHashes;
}

void DatabaseBlockchainCache::requestDeleteTransactions(BlockchainWriteBatch& writeBatch,
                                                        const std::vector<Crypto::Hash>& transactionHashes) {
  for (const auto& hash : transactionHashes) {
    assert(getCachedTransactionsCount() > 0);
    writeBatch.removeCachedTransaction(hash, getCachedTransactionsCount() - 1);
    transactionsCount = *transactionsCount - 1;
  }
}

void DatabaseBlockchainCache::requestDeletePaymentIds(BlockchainWriteBatch& writeBatch,
                                                      const std::vector<Crypto::Hash>& transactionHashes) {
  std::unordered_map<PaymentId, size_t> paymentCounts;

  for (const auto& hash : transactionHashes) {
    PaymentId paymentId;
    if (!requestPaymentId(database, hash, paymentId, currency)) {
      continue;
    }

    paymentCounts[paymentId] += 1;
  }

  for (const auto& kv : paymentCounts) {
    requestDeletePaymentId(writeBatch, kv.first, kv.second);
  }
}

void DatabaseBlockchainCache::requestDeletePaymentId(BlockchainWriteBatch& writeBatch, const PaymentId& paymentId,
                                                     size_t toDelete) {
  size_t count = requestPaymentIdTransactionsCount(database, paymentId);
  assert(count > 0);
  assert(count >= toDelete);

  logger(Logging::Debugging) << "Deleting last " << toDelete << " transaction hashes of payment id " << paymentId;
  writeBatch.removePaymentId(paymentId, static_cast<uint32_t>(count - toDelete));
}

void DatabaseBlockchainCache::requestDeleteSpentOutputs(BlockchainWriteBatch& writeBatch, uint32_t blockIndex,
                                                        const TransactionValidatorState& spentOutputs) {
  logger(Logging::Debugging) << "Deleting spent outputs for block index " << blockIndex;

  std::vector<Crypto::KeyImage> spentKeys(spentOutputs.spentKeyImages.begin(), spentOutputs.spentKeyImages.end());

  writeBatch.removeSpentKeyImages(blockIndex, spentKeys);
}

void DatabaseBlockchainCache::requestDeleteKeyOutputs(
    BlockchainWriteBatch& writeBatch,
    const std::map<IBlockchainCache::Amount, IBlockchainCache::GlobalOutputIndex>& boundaries) {
  if (boundaries.empty()) {
    // hardly possible
    logger(Logging::Debugging) << "No key output amounts...";
    return;
  }

  BlockchainReadBatch readBatch;
  for (auto kv : boundaries) {
    readBatch.requestKeyOutputGlobalIndexesCountForAmount(kv.first);
  }

  std::unordered_map<IBlockchainCache::Amount, uint32_t> amountCounts =
      readDatabase(readBatch).getKeyOutputGlobalIndexesCountForAmounts();
  assert(amountCounts.size() == boundaries.size());

  for (const auto& kv : amountCounts) {
    auto it = boundaries.find(kv.first);  // can't be equal end() since assert(amountCounts.size() == boundaries.size())
    requestDeleteKeyOutputsAmount(writeBatch, kv.first, it->second, kv.second);
  }
}

void DatabaseBlockchainCache::requestDeleteKeyOutputsAmount(BlockchainWriteBatch& writeBatch,
                                                            IBlockchainCache::Amount amount,
                                                            IBlockchainCache::GlobalOutputIndex boundary,
                                                            uint32_t outputsCount) {
  logger(Logging::Debugging) << "Requesting delete for key output amount " << amount << " starting from global index "
                             << boundary << " to " << (outputsCount - 1);

  writeBatch.removeKeyOutputGlobalIndexes(amount, outputsCount - boundary, boundary);
  for (GlobalOutputIndex index = boundary; index < outputsCount; ++index) {
    writeBatch.removeKeyOutputInfo(amount, index);
  }

  updateKeyOutputCount(amount, boundary - outputsCount);
}

void DatabaseBlockchainCache::requestRemoveTimestamp(BlockchainWriteBatch& batch, uint64_t timestamp,
                                                     const Crypto::Hash& blockHash) {
  auto readBatch = BlockchainReadBatch().requestBlockHashesByTimestamp(timestamp);
  auto result = readDatabase(readBatch);

  if (result.getBlockHashesByTimestamp().count(timestamp) == 0) {
    return;
  }

  auto indexes = result.getBlockHashesByTimestamp().at(timestamp);
  auto it = std::find(indexes.begin(), indexes.end(), blockHash);
  indexes.erase(it);

  if (indexes.empty()) {
    logger(Logging::Debugging) << "Deleting timestamp " << timestamp;
    batch.removeTimestamp(timestamp);
  } else {
    logger(Logging::Debugging) << "Deleting block hash " << blockHash << " from timestamp " << timestamp;
    batch.insertTimestamp(timestamp, indexes);
  }
}

void DatabaseBlockchainCache::pushTransaction(const CachedTransaction& cachedTransaction, uint32_t blockIndex,
                                              uint16_t transactionBlockIndex, BlockchainWriteBatch& batch,
                                              bool isGenerated) {
  logger(Logging::Debugging) << "push transaction with hash " << cachedTransaction.getTransactionHash();
  const auto& tx = cachedTransaction.getTransaction();

  ExtendedTransactionInfo transactionCacheInfo;
  transactionCacheInfo.blockIndex = blockIndex;
  transactionCacheInfo.transactionIndex = transactionBlockIndex;
  transactionCacheInfo.transactionHash = cachedTransaction.getTransactionHash();
  transactionCacheInfo.unlockTime = tx.unlockTime;
  transactionCacheInfo.isDeterministicallyGenerated = isGenerated;

  assert(tx.outputs.size() <= std::numeric_limits<uint16_t>::max());

  transactionCacheInfo.globalIndexes.reserve(tx.outputs.size());
  transactionCacheInfo.outputs.reserve(tx.outputs.size());
  uint16_t outputCount = 0;
  std::unordered_map<Amount, std::vector<PackedOutIndex>> keyIndexes;

  std::set<Amount> newKeyAmounts;

  for (auto& output : tx.outputs) {
    transactionCacheInfo.outputs.push_back(output);

    PackedOutIndex poi;
    poi.data.blockIndex = blockIndex;
    poi.data.transactionIndex = transactionBlockIndex;
    poi.data.outputIndex = outputCount++;

    if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(output))) {
      keyIndexes[amountOutput->amount].push_back(poi);
      auto outputCountForAmount = updateKeyOutputCount(amountOutput->amount, 1);
      if (outputCountForAmount == 1) {
        newKeyAmounts.insert(amountOutput->amount);
      }

      assert(outputCountForAmount > 0);
      auto globalIndex = outputCountForAmount - 1;
      transactionCacheInfo.globalIndexes.push_back(globalIndex);
      // output global index:
      transactionCacheInfo.amountToKeyIndexes[amountOutput->amount].push_back(globalIndex);

      if (const auto keyTarget = std::get_if<KeyOutput>(std::addressof(amountOutput->target))) {
        KeyOutputInfo outputInfo;
        outputInfo.publicKey = keyTarget->key;
        outputInfo.transactionHash = transactionCacheInfo.transactionHash;
        outputInfo.unlockTime = transactionCacheInfo.unlockTime;
        outputInfo.index = poi;
        batch.insertKeyOutputInfo(amountOutput->amount, globalIndex, outputInfo);
      }
    }
  }

  for (auto& amountToOutputs : keyIndexes) {
    batch.insertKeyOutputGlobalIndexes(amountToOutputs.first, amountToOutputs.second,
                                       updateKeyOutputCount(amountToOutputs.first, 0));  // Size already updated.
  }

  if (!newKeyAmounts.empty()) {
    assert(keyOutputAmountsCount.is_initialized());
    batch.insertKeyOutputAmounts(newKeyAmounts, *keyOutputAmountsCount);
  }

  PaymentId paymentId;
  if (getPaymentIdFromTxExtra(cachedTransaction.getTransaction().extra, paymentId)) {
    insertPaymentId(batch, cachedTransaction.getTransactionHash(), paymentId);
  }

  batch.insertCachedTransaction(transactionCacheInfo, getCachedTransactionsCount() + 1);
  transactionsCount = *transactionsCount + 1;
  logger(Logging::Debugging) << "push transaction with hash " << cachedTransaction.getTransactionHash() << " finished";
}

uint32_t DatabaseBlockchainCache::updateKeyOutputCount(Amount amount, int32_t diff) const {
  auto it = keyOutputCountsForAmounts.find(amount);
  if (it == keyOutputCountsForAmounts.end()) {
    logger(Logging::Trace) << "updateKeyOutputCount: failed to found key for amount, request database";

    BlockchainReadBatch batch;
    auto result = readDatabase(batch.requestKeyOutputGlobalIndexesCountForAmount(amount));
    auto found = result.getKeyOutputGlobalIndexesCountForAmounts().find(amount);
    auto val = found != result.getKeyOutputGlobalIndexesCountForAmounts().end() ? found->second : 0;
    it = keyOutputCountsForAmounts.insert({amount, val}).first;
    logger(Logging::Trace) << "updateKeyOutputCount: database replied: amount " << amount << " value " << val;

    if (val == 0) {
      if (!keyOutputAmountsCount) {
        auto innerResult = readDatabase(batch.requestKeyOutputAmountsCount());
        keyOutputAmountsCount = innerResult.getKeyOutputAmountsCount();
      }

      keyOutputAmountsCount = *keyOutputAmountsCount + 1;
    }
  } else if (!keyOutputAmountsCount) {
    auto innerResult = readDatabase(BlockchainReadBatch().requestKeyOutputAmountsCount());
    keyOutputAmountsCount = innerResult.getKeyOutputAmountsCount();
  }

  it->second += diff;
  assert(it->second >= 0);
  return it->second;
}

void DatabaseBlockchainCache::insertPaymentId(BlockchainWriteBatch& batch, const Crypto::Hash& transactionHash,
                                              const PaymentId& paymentId) {
  BlockchainReadBatch readBatch;
  uint32_t count = 0;

  auto readResult = readDatabase(readBatch.requestTransactionCountByPaymentId(paymentId));
  if (readResult.getTransactionCountByPaymentIds().count(paymentId) != 0) {
    count = readResult.getTransactionCountByPaymentIds().at(paymentId);
  }

  count += 1;

  batch.insertPaymentId(transactionHash, paymentId, count);
}

void DatabaseBlockchainCache::insertBlockTimestamp(BlockchainWriteBatch& batch, uint64_t timestamp,
                                                   const Crypto::Hash& blockHash) {
  BlockchainReadBatch readBatch;
  readBatch.requestBlockHashesByTimestamp(timestamp);

  std::vector<Crypto::Hash> blockHashes;
  auto readResult = readDatabase(readBatch);

  if (readResult.getBlockHashesByTimestamp().count(timestamp) != 0) {
    blockHashes = readResult.getBlockHashesByTimestamp().at(timestamp);
  }

  blockHashes.emplace_back(blockHash);

  batch.insertTimestamp(timestamp, blockHashes);
}

void DatabaseBlockchainCache::pushBlock(const CachedBlock& cachedBlock,
                                        const std::vector<CachedTransaction>& cachedTransactions,
                                        const TransactionValidatorState& validatorState, size_t blockSize,
                                        uint64_t generatedCoins, uint64_t blockDifficulty, RawBlock&& rawBlock) {
  BlockchainWriteBatch batch;
  logger(Logging::Debugging) << "push block with hash " << cachedBlock.getBlockHash() << ", and "
                             << cachedTransactions.size() + 1 << " transactions";  //+1 for base transaction

  // TODO: cache top block difficulty, size, timestamp, coins; use it here
  const auto index = cachedBlock.getBlockIndex();
  assert(index > 0);
  assert(index == getTopBlockIndex() + 1);
  auto lastBlockInfo = getCachedBlockInfo(index - 1);
  auto cumulativeDifficulty = lastBlockInfo.cumulativeDifficulty + blockDifficulty;
  auto alreadyGeneratedCoins = lastBlockInfo.alreadyGeneratedCoins + generatedCoins;
  auto alreadyGeneratedTransactions = lastBlockInfo.alreadyGeneratedTransactions + cachedTransactions.size() + 1;
  boost::optional<CachedTransaction> staticRewardTransaction{boost::none};
  {
    auto staticRewardConstruct = currency.constructStaticRewardTx(cachedBlock).takeOrThrow();
    if (staticRewardConstruct.has_value()) {
      staticRewardTransaction = CachedTransaction{std::move(*staticRewardConstruct)};
    }
  }

  if (staticRewardTransaction.has_value()) {
    alreadyGeneratedTransactions += 1;
  }

  CachedBlockInfo blockInfo;
  blockInfo.blockHash = cachedBlock.getBlockHash();
  blockInfo.version = cachedBlock.getBlock().version;
  blockInfo.features = cachedBlock.getBlock().features;
  blockInfo.timestamp = cachedBlock.getBlock().timestamp;
  blockInfo.blobSize = blockSize;
  blockInfo.cumulativeDifficulty = cumulativeDifficulty;
  blockInfo.alreadyGeneratedCoins = alreadyGeneratedCoins;
  blockInfo.alreadyGeneratedTransactions = alreadyGeneratedTransactions;

  batch.insertSpentKeyImages(index, validatorState.spentKeyImages);

  auto txHashes = cachedBlock.getBlock().transactionHashes;
  auto baseTransaction = cachedBlock.getBlock().baseTransaction;
  auto cachedBaseTransaction = CachedTransaction{std::move(baseTransaction)};

  // static reward transaction's hash is always the second one in index for this block
  if (staticRewardTransaction.has_value()) {
    txHashes.insert(txHashes.begin(), staticRewardTransaction->getTransactionHash());
  }
  // base transaction's hash is always the first one in index for this block
  txHashes.insert(txHashes.begin(), cachedBaseTransaction.getTransactionHash());

  batch.insertCachedBlock(blockInfo, index, txHashes);

  batch.insertRawBlock(index, std::move(rawBlock));

  uint16_t transactionIndex = 0;
  pushTransaction(cachedBaseTransaction, index, transactionIndex++, batch, false);
  if (staticRewardTransaction.has_value()) {
    pushTransaction(std::move(staticRewardTransaction.value()), index, transactionIndex++, batch, true);
  }

  for (const auto& transaction : cachedTransactions) {
    pushTransaction(transaction, index, transactionIndex++, batch, false);
  }

  auto closestBlockIndexDb =
      requestClosestBlockIndexByTimestamp(roundToMidnight(cachedBlock.getBlock().timestamp), database);
  if (!closestBlockIndexDb.second) {
    logger(Logging::Error) << "push block " << cachedBlock.getBlockHash()
                           << " request closest block index by timestamp failed";
    throw std::runtime_error("Couldn't get closest to timestamp block index");
  }

  if (!closestBlockIndexDb.first) {
    batch.insertClosestTimestampBlockIndex(roundToMidnight(cachedBlock.getBlock().timestamp), index);
  }

  insertBlockTimestamp(batch, cachedBlock.getBlock().timestamp, cachedBlock.getBlockHash());

  auto res = database.write(batch);
  if (res) {
    logger(Logging::Error) << "push block " << cachedBlock.getBlockHash() << " write failed: " << res.message();
    throw std::runtime_error(res.message());
  }

  topBlockIndex = *topBlockIndex + 1;
  topBlockHash = cachedBlock.getBlockHash();
  topBlockVersion = cachedBlock.getBlock().version;
  logger(Logging::Debugging) << "push block " << cachedBlock.getBlockHash() << " completed";

  unitsCache.push_back(blockInfo);
  if (unitsCache.size() > unitsCacheSize) {
    unitsCache.pop_front();
  }
}

PushedBlockInfo DatabaseBlockchainCache::getPushedBlockInfo(uint32_t blockIndex) const {
  return getExtendedPushedBlockInfo(blockIndex).pushedBlockInfo;
}

bool DatabaseBlockchainCache::checkIfSpent(const Crypto::KeyImage& keyImage, uint32_t blockIndex) const {
  auto batch = BlockchainReadBatch().requestBlockIndexBySpentKeyImage(keyImage);
  auto res = database.read(batch);
  if (res) {
    logger(Logging::Error) << "checkIfSpent failed, request to database failed: " << res.message();
    return false;
  }

  auto readResult = batch.extractResult();
  auto it = readResult.getBlockIndexesBySpentKeyImages().find(keyImage);

  return it != readResult.getBlockIndexesBySpentKeyImages().end() && it->second <= blockIndex;
}

bool DatabaseBlockchainCache::checkIfSpent(const Crypto::KeyImage& keyImage) const {
  return checkIfSpent(keyImage, getTopBlockIndex());
}

bool DatabaseBlockchainCache::checkIfAnySpent(const Crypto::KeyImageSet& keyImages, uint32_t blockIndex) const {
  BlockchainReadBatch batch{};
  for (const auto& keyImage : keyImages) {
    batch.requestBlockIndexBySpentKeyImage(keyImage);
  }

  if (const auto ec = database.read(batch)) {
    logger(Logging::Fatal) << "checkIfSpent failed, request to database failed: " << ec.message();
    XI_RETURN_EC(true);
  }

  auto readResult = batch.extractResult();
  const auto& spentKeyImages = readResult.getBlockIndexesBySpentKeyImages();

  return std::any_of(spentKeyImages.begin(), spentKeyImages.end(),
                     [blockIndex](const auto& spentKeyImage) { return blockIndex <= spentKeyImage.second; });
}

ExtractOutputKeysResult DatabaseBlockchainCache::extractKeyOutputKeys(
    uint64_t amount, Common::ArrayView<uint32_t> globalIndexes, std::vector<Crypto::PublicKey>& publicKeys) const {
  return extractKeyOutputKeys(amount, getTopBlockIndex(), globalIndexes, publicKeys);
}

ExtractOutputKeysResult DatabaseBlockchainCache::extractKeyOutputKeys(
    uint64_t amount, uint32_t blockIndex, Common::ArrayView<uint32_t> globalIndexes,
    std::vector<Crypto::PublicKey>& publicKeys) const {
  const auto timestamp = time(nullptr);
  if (timestamp < 0) {
    return ExtractOutputKeysResult::TIME_PROVIDER_FAILED;
  } else {
    return extractKeyOutputKeys(amount, blockIndex, globalIndexes, publicKeys, static_cast<uint64_t>(timestamp));
  }
}

ExtractOutputKeysResult DatabaseBlockchainCache::extractKeyOutputKeys(uint64_t amount, uint32_t blockIndex,
                                                                      Common::ArrayView<uint32_t> globalIndexes,
                                                                      std::vector<Crypto::PublicKey>& publicKeys,
                                                                      uint64_t timestamp) const {
  assert(!globalIndexes.isEmpty());
  assert(std::is_sorted(globalIndexes.begin(), globalIndexes.end()));                             // sorted
  assert(std::adjacent_find(globalIndexes.begin(), globalIndexes.end()) == globalIndexes.end());  // unique

  return extractKeyOutputs(
      amount, blockIndex, globalIndexes,
      [&](const CachedTransactionInfo& info, PackedOutIndex index, uint32_t globalIndex) {
        XI_UNUSED(globalIndex);
        if (!isTransactionSpendTimeUnlocked(info.unlockTime, blockIndex, timestamp)) {
          return ExtractOutputKeysResult::OUTPUT_LOCKED;
        }

        assert(std::holds_alternative<TransactionAmountOutput>(info.outputs[index.data.outputIndex]));
        const auto amountOutput = std::get<TransactionAmountOutput>(info.outputs[index.data.outputIndex]);
        assert(std::holds_alternative<KeyOutput>(amountOutput.target));
        publicKeys.push_back(std::get<KeyOutput>(amountOutput.target).key);
        return ExtractOutputKeysResult::SUCCESS;
      });
}

ExtractOutputKeysResult DatabaseBlockchainCache::extractKeyOtputIndexes(uint64_t amount,
                                                                        Common::ArrayView<uint32_t> globalIndexes,
                                                                        std::vector<PackedOutIndex>& outIndexes) const {
  if (!requestPackedOutputs(amount, globalIndexes, database, outIndexes)) {
    logger(Logging::Error) << "extractKeyOtputIndexes failed: failed to read database";
    return ExtractOutputKeysResult::INVALID_GLOBAL_INDEX;
  }

  return ExtractOutputKeysResult::SUCCESS;
}

ExtractOutputKeysResult DatabaseBlockchainCache::extractKeyOtputReferences(
    uint64_t amount, Common::ArrayView<uint32_t> globalIndexes,
    std::vector<std::pair<Crypto::Hash, size_t>>& outputReferences) const {
  return extractKeyOutputs(
      amount, getTopBlockIndex(), globalIndexes,
      [&outputReferences](const CachedTransactionInfo& info, PackedOutIndex index, uint32_t globalIndex) {
        XI_UNUSED(globalIndex);
        outputReferences.push_back(std::make_pair(info.transactionHash, index.data.outputIndex));
        return ExtractOutputKeysResult::SUCCESS;
      });
}

std::map<DatabaseBlockchainCache::Amount, std::map<DatabaseBlockchainCache::GlobalOutputIndex, KeyOutputInfo>>
DatabaseBlockchainCache::extractKeyOutputs(
    const std::unordered_map<DatabaseBlockchainCache::Amount, GlobalOutputIndexSet>& references,
    uint32_t blockIndex) const {
  XI_RETURN_SC_IF(references.empty(), {});

  BlockchainReadBatch batch{};
  std::map<Amount, std::map<GlobalOutputIndex, KeyOutputInfo>> reval{};

  for (const auto& amountReferences : references) {
    const auto amount = amountReferences.first;
    const auto& globalIndices = amountReferences.second;

    for (const auto& globalIndex : globalIndices) {
      batch.requestKeyOutputInfo(amount, globalIndex);
    }
  }

  const auto read = readDatabase(batch);
  const auto& keyOutputInfos = read.getKeyOutputInfo();
  for (const auto& keyOutputInfo : keyOutputInfos) {
    if (keyOutputInfo.second.index.data.blockIndex > blockIndex) {
      // TODO: with an ordered map we could break here :)
      continue;
    }
    reval[keyOutputInfo.first.first][keyOutputInfo.first.second] = keyOutputInfo.second;
  }

  return reval;
}

uint32_t DatabaseBlockchainCache::getTopBlockIndex() const {
  if (!topBlockIndex) {
    auto batch = BlockchainReadBatch().requestLastBlockIndex();
    auto result = database.read(batch);

    if (result) {
      logger(Logging::Error) << "Failed to read top block index from database";
      throw std::system_error(result);
    }

    auto readResult = batch.extractResult();
    if (!readResult.getLastBlockIndex().second) {
      logger(Logging::Trace) << "Top block index does not exist in database";
      topBlockIndex = 0;
    }

    topBlockIndex = readResult.getLastBlockIndex().first;
  }

  return *topBlockIndex;
}

BlockVersion DatabaseBlockchainCache::getBlockVersionForHeight(uint32_t height) const {
  return currency.upgradeManager().getBlockVersion(height);
}

uint64_t DatabaseBlockchainCache::getCachedTransactionsCount() const {
  if (!transactionsCount) {
    auto batch = BlockchainReadBatch().requestTransactionsCount();
    auto result = database.read(batch);

    if (result) {
      logger(Logging::Error) << "Failed to read transactions count from database";
      throw std::system_error(result);
    }

    auto readResult = batch.extractResult();
    if (!readResult.getTransactionsCount().second) {
      logger(Logging::Trace) << "Transactions count does not exist in database";
      transactionsCount = 0;
    } else {
      transactionsCount = readResult.getTransactionsCount().first;
    }
  }

  return *transactionsCount;
}

const Crypto::Hash& DatabaseBlockchainCache::getTopBlockHash() const {
  if (!topBlockHash) {
    auto batch = BlockchainReadBatch().requestCachedBlock(getTopBlockIndex());
    auto result = readDatabase(batch);
    topBlockHash = result.getCachedBlocks().at(getTopBlockIndex()).blockHash;
  }
  return *topBlockHash;
}

BlockVersion DatabaseBlockchainCache::getTopBlockVersion() const {
  if (!topBlockVersion) {
    auto batch = BlockchainReadBatch().requestCachedBlock(getTopBlockIndex());
    auto result = readDatabase(batch);
    topBlockVersion = result.getCachedBlocks().at(getTopBlockIndex()).version;
  }
  return *topBlockVersion;
}
uint32_t DatabaseBlockchainCache::getBlockCount() const {
  return getTopBlockIndex() + 1;
}

bool DatabaseBlockchainCache::hasBlock(const Crypto::Hash& blockHash) const {
  auto batch = BlockchainReadBatch().requestBlockIndexByBlockHash(blockHash);
  auto result = database.read(batch);
  return !result && batch.extractResult().getBlockIndexesByBlockHashes().count(blockHash);
}

uint32_t DatabaseBlockchainCache::getBlockIndex(const Crypto::Hash& blockHash) const {
  if (blockHash == getTopBlockHash()) {
    return getTopBlockIndex();
  }

  auto batch = BlockchainReadBatch().requestBlockIndexByBlockHash(blockHash);
  auto result = readDatabase(batch);
  return result.getBlockIndexesByBlockHashes().at(blockHash);
}

BlockHeightVector DatabaseBlockchainCache::getBlockHeights(ConstBlockHashSpan hashes) const {
  BlockHeightVector reval{};
  reval.resize(hashes.size(), BlockHeight::Null);

  BlockchainReadBatch batch{};
  for (const auto& hash : hashes) {
    batch.requestBlockIndexByBlockHash(hash);
  }
  auto result = readDatabase(batch);

  size_t i = 0;
  const auto& indices = result.getBlockIndexesByBlockHashes();
  for (const auto& hash : hashes) {
    auto search = indices.find(hash);
    if (search == indices.end()) {
      i += 1;
      continue;
    } else {
      reval[i++] = BlockHeight::fromIndex(search->second);
    }
  }

  return reval;
}

bool DatabaseBlockchainCache::hasTransaction(const Crypto::Hash& transactionHash) const {
  auto batch = BlockchainReadBatch().requestCachedTransaction(transactionHash);
  auto result = database.read(batch);
  return !result && batch.extractResult().getCachedTransactions().count(transactionHash);
}

CachedBlockInfo DatabaseBlockchainCache::getCachedBlockInfo(uint32_t index) const {
  auto batch = BlockchainReadBatch().requestCachedBlock(index);
  auto result = readDatabase(batch);
  return result.getCachedBlocks().at(index);
}

std::vector<CachedBlockInfo> DatabaseBlockchainCache::getLastCachedUnits(uint32_t blockIndex, size_t count,
                                                                         UseGenesis useGenesis) const {
  assert(blockIndex <= getTopBlockIndex());

  std::vector<CachedBlockInfo> cachedResult;
  const uint32_t cacheStartIndex = (getTopBlockIndex() + 1) - static_cast<uint32_t>(unitsCache.size());

  count = std::min(unitsCache.size(), count);

  if (cacheStartIndex > blockIndex || count == 0) {
    return cachedResult;
  }

  count = std::min(blockIndex - cacheStartIndex + 1, static_cast<uint32_t>(count));
  uint32_t offset = static_cast<uint32_t>(blockIndex + 1 - count) - cacheStartIndex;

  assert(offset < unitsCache.size());

  if (!useGenesis && cacheStartIndex == 0 && offset == 0) {
    ++offset;
    --count;
  }

  if (offset >= unitsCache.size() || count == 0) {
    return cachedResult;
  }

  cachedResult.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    cachedResult.push_back(unitsCache[offset + i]);
  }

  return cachedResult;
}

std::vector<CachedBlockInfo> DatabaseBlockchainCache::getLastDbUnits(uint32_t blockIndex, size_t count,
                                                                     UseGenesis useGenesis) const {
  uint32_t readFrom = blockIndex + 1 - std::min(blockIndex + 1, static_cast<uint32_t>(count));
  if (readFrom == 0 && !useGenesis) {
    readFrom += 1;
  }

  uint32_t toRead = blockIndex - readFrom + 1;
  std::vector<CachedBlockInfo> units;
  units.reserve(toRead);

  const uint32_t step = 200;
  while (toRead > 0) {
    auto next = std::min(toRead, step);
    toRead -= next;

    BlockchainReadBatch batch;
    for (auto id = readFrom; id < readFrom + next; ++id) {
      batch.requestCachedBlock(id);
    }

    readFrom += next;

    auto res = readDatabase(batch);

    std::map<uint32_t, CachedBlockInfo> sortedResult(res.getCachedBlocks().begin(), res.getCachedBlocks().end());
    for (const auto& kv : sortedResult) {
      units.push_back(kv.second);
    }
    //    std::transform(sortedResult.begin(), sortedResult.end(), std::back_inserter(units),
    //                   [&](const std::pair<uint32_t, CachedBlockInfo>& cb) { return pred(cb.second); });
  }

  return units;
}

std::vector<uint64_t> DatabaseBlockchainCache::getLastUnits(
    size_t count, uint32_t blockIndex, UseGenesis useGenesis,
    std::function<uint64_t(const CachedBlockInfo&)> pred) const {
  XI_RETURN_EC_IF(blockIndex > getTopBlockIndex(), {});

  assert(count <= std::numeric_limits<uint32_t>::max());

  auto cachedUnits = getLastCachedUnits(blockIndex, count, useGenesis);

  uint32_t availableUnits = blockIndex;
  if (useGenesis) {
    availableUnits += 1;
  }

  assert(availableUnits >= cachedUnits.size());

  if (availableUnits - cachedUnits.size() == 0) {
    std::vector<uint64_t> result;
    result.reserve(cachedUnits.size());
    for (const auto& unit : cachedUnits) {
      result.push_back(pred(unit));
    }

    return result;
  }

  assert(blockIndex + 1 >= cachedUnits.size());
  uint32_t dbIndex = blockIndex - static_cast<uint32_t>(cachedUnits.size());

  assert(count >= cachedUnits.size());
  size_t leftCount = count - cachedUnits.size();

  auto dbUnits = getLastDbUnits(dbIndex, leftCount, useGenesis);
  std::vector<uint64_t> result;
  result.reserve(dbUnits.size() + cachedUnits.size());
  for (const auto& unit : dbUnits) {
    result.push_back(pred(unit));
  }

  for (const auto& unit : cachedUnits) {
    result.push_back(pred(unit));
  }

  return result;
}

Crypto::Hash DatabaseBlockchainCache::getBlockHash(uint32_t blockIndex) const {
  using namespace Xi;
  exceptional_if<NotFoundError>(blockIndex > getTopBlockIndex());

  if (blockIndex == getTopBlockIndex()) {
    return getTopBlockHash();
  }

  auto batch = BlockchainReadBatch().requestCachedBlock(blockIndex);
  auto result = readDatabase(batch);
  return result.getCachedBlocks().at(blockIndex).blockHash;
}

std::vector<Crypto::Hash> DatabaseBlockchainCache::getBlockHashes(uint32_t startIndex, size_t maxCount) const {
  assert(startIndex <= getTopBlockIndex());
  assert(maxCount <= std::numeric_limits<uint32_t>::max());

  uint32_t count = std::min(getTopBlockIndex() - startIndex + 1, static_cast<uint32_t>(maxCount));
  if (count == 0) {
    return {};
  }

  BlockchainReadBatch request;
  auto index = startIndex;
  while (index != startIndex + count) {
    request.requestCachedBlock(index++);
  }

  auto result = readDatabase(request);
  assert(result.getCachedBlocks().size() == count);

  std::vector<Crypto::Hash> hashes;
  hashes.reserve(count);

  std::map<uint32_t, CachedBlockInfo> sortedResult(result.getCachedBlocks().begin(), result.getCachedBlocks().end());

  std::transform(sortedResult.begin(), sortedResult.end(), std::back_inserter(hashes),
                 [](const std::pair<uint32_t, CachedBlockInfo>& cb) { return cb.second.blockHash; });
  return hashes;
}

IBlockchainCache* DatabaseBlockchainCache::getParent() const {
  return nullptr;
}

uint32_t DatabaseBlockchainCache::getStartBlockIndex() const {
  return 0;
}

size_t DatabaseBlockchainCache::getKeyOutputsCountForAmount(uint64_t amount, uint32_t blockIndex) const {
  uint32_t outputsCount = requestKeyOutputGlobalIndexesCountForAmount(amount, database);

  auto getOutput = std::bind(retrieveKeyOutput, std::placeholders::_1, std::placeholders::_2, std::ref(database));
  auto begin = DbOutputConstIterator(getOutput, amount, 0);
  auto end = DbOutputConstIterator(getOutput, amount, outputsCount);

  auto it = std::lower_bound(begin, end, blockIndex, [](const PackedOutIndex& output, uint32_t blockIndex) {
    return output.data.blockIndex < blockIndex;
  });

  size_t result = static_cast<size_t>(std::distance(begin, it));
  logger(Logging::Debugging) << "Key outputs count for amount " << amount << " is " << result << " by block index "
                             << blockIndex;

  return result;
}

uint32_t DatabaseBlockchainCache::getTimestampLowerBoundBlockIndex(uint64_t timestamp) const {
  auto midnight = roundToMidnight(timestamp);

  while (midnight > 0) {
    auto dbRes = requestClosestBlockIndexByTimestamp(midnight, database);
    if (!dbRes.second) {
      logger(Logging::Debugging) << "getTimestampLowerBoundBlockIndex failed: failed to read database";
      throw std::runtime_error("Couldn't get closest to timestamp block index");
    }

    if (!dbRes.first) {
      midnight -= 60 * 60 * 24;
      continue;
    }

    return *dbRes.first;
  }

  return 0;
}

bool DatabaseBlockchainCache::getTransactionGlobalIndexes(const Crypto::Hash& transactionHash,
                                                          std::vector<uint32_t>& globalIndexes) const {
  auto batch = BlockchainReadBatch().requestCachedTransaction(transactionHash);
  auto result = database.read(batch);
  if (result) {
    logger(Logging::Debugging) << "getTransactionGlobalIndexes failed: failed to read database";
    return false;
  }

  auto readResult = batch.extractResult();
  auto it = readResult.getCachedTransactions().find(transactionHash);
  if (it == readResult.getCachedTransactions().end()) {
    logger(Logging::Debugging) << "getTransactionGlobalIndexes failed: cached transaction for hash " << transactionHash
                               << " not present";
    return false;
  }

  globalIndexes = it->second.globalIndexes;
  return true;
}

size_t DatabaseBlockchainCache::getTransactionCount() const {
  return static_cast<size_t>(getCachedTransactionsCount());
}

uint32_t DatabaseBlockchainCache::getBlockIndexContainingTx(const Crypto::Hash& transactionHash) const {
  auto batch = BlockchainReadBatch().requestCachedTransaction(transactionHash);
  auto result = readDatabase(batch);
  return result.getCachedTransactions().at(transactionHash).blockIndex;
}

size_t DatabaseBlockchainCache::getChildCount() const {
  return children.size();
}

bool DatabaseBlockchainCache::save() {
  return true;
}

bool DatabaseBlockchainCache::load() {
  return true;
}

std::vector<BinaryArray> DatabaseBlockchainCache::getRawTransactions(
    const std::vector<Crypto::Hash>& transactions, std::vector<Crypto::Hash>& missedTransactions) const {
  std::vector<BinaryArray> found;
  getRawTransactions(transactions, found, missedTransactions);
  return found;
}

std::vector<BinaryArray> DatabaseBlockchainCache::getRawTransactions(
    const std::vector<Crypto::Hash>& transactions) const {
  std::vector<Crypto::Hash> missed;
  std::vector<BinaryArray> found;
  getRawTransactions(transactions, found, missed);
  return found;
}

void DatabaseBlockchainCache::getRawTransactions(const std::vector<Crypto::Hash>& transactions,
                                                 std::vector<BinaryArray>& foundTransactions,
                                                 std::vector<Crypto::Hash>& missedTransactions) const {
  BlockchainReadBatch batch;
  for (auto& hash : transactions) {
    batch.requestCachedTransaction(hash);
  }

  auto res = readDatabase(batch);
  for (auto& tx : res.getCachedTransactions()) {
    batch.requestRawBlock(tx.second.blockIndex);
  }

  auto blocks = readDatabase(batch);

  foundTransactions.reserve(foundTransactions.size() + transactions.size());
  auto& hashesMap = res.getCachedTransactions();
  auto& blocksMap = blocks.getRawBlocks();
  for (const auto& hash : transactions) {
    auto transactionIt = hashesMap.find(hash);
    if (transactionIt == hashesMap.end()) {
      logger(Logging::Debugging) << "detected missing transaction for hash " << hash << " in getRawTransaction";
      missedTransactions.push_back(hash);
      continue;
    }

    auto blockIt = blocksMap.find(transactionIt->second.blockIndex);
    if (blockIt == blocksMap.end()) {
      logger(Logging::Debugging) << "detected missing transaction for hash " << hash << " in getRawTransaction";
      missedTransactions.push_back(hash);
      continue;
    }

    auto block = fromBinaryArray<BlockTemplate>(blockIt->second.blockTemplate);
    const bool hasStaticReward = currency.isStaticRewardEnabledForBlockVersion(block.version);
    if (transactionIt->second.transactionIndex == 0) {
      foundTransactions.emplace_back(toBinaryArray(block.baseTransaction));
    } else if (hasStaticReward && transactionIt->second.transactionIndex == 1) {
      foundTransactions.emplace_back(toBinaryArray(*currency.constructStaticRewardTx(block).takeOrThrow()));
    } else {
      const size_t staticTransactionsOffset = hasStaticReward ? 2 : 1;
      assert(blockIt->second.transactions.size() >= transactionIt->second.transactionIndex - staticTransactionsOffset);
      foundTransactions.emplace_back(
          blockIt->second.transactions[transactionIt->second.transactionIndex - staticTransactionsOffset]);
    }
  }
}

RawBlock DatabaseBlockchainCache::getBlockByIndex(uint32_t index) const {
  auto batch = BlockchainReadBatch().requestRawBlock(index);
  auto res = readDatabase(batch);
  return std::move(res.getRawBlocks().at(index));
}

RawBlockVector DatabaseBlockchainCache::getBlocks(ConstBlockHeightSpan heights) const {
  using namespace Xi;

  RawBlockVector reval{};
  reval.reserve(heights.size());

  BlockchainReadBatch batch{};
  for (const auto& height : heights) {
    exceptional_if<InvalidArgumentError>(height.isNull(), "provided height is null");
    batch.requestRawBlock(height.toIndex());
  }

  auto result = readDatabase(batch);
  auto& rawBlocks = result.getRawBlocks();

  for (const auto& height : heights) {
    auto search = rawBlocks.find(height.toIndex());
    exceptional_if<NotFoundError>(search == rawBlocks.end(), "raw block not available for height");
    reval.emplace_back(std::move(search->second));
  }

  return reval;
}

CachedBlockInfoVector DatabaseBlockchainCache::getBlockInfos(ConstBlockHeightSpan heights) const {
  using namespace Xi;

  CachedBlockInfoVector reval{};
  reval.resize(heights.size());

  const auto topIndex = getTopBlockIndex();
  const uint32_t cacheStartIndex = (topIndex + 1) - static_cast<uint32_t>(unitsCache.size());

  std::map<uint32_t, size_t> cacheMisses{};
  BlockchainReadBatch batch{};

  size_t i = 0;
  for (const auto& height : heights) {
    exceptional_if<InvalidArgumentError>(height.isNull(), "provided height is null");
    const auto index = height.toIndex();
    exceptional_if<OutOfRangeError>(index > topIndex, "height exceeds cache boundary");

    if (index < cacheStartIndex || index + 1 >= cacheStartIndex + unitsCache.size()) {
      cacheMisses[index] = i++;
      batch.requestCachedBlock(index);
    } else {
      reval[i++] = unitsCache[index - cacheStartIndex];
    }
  }

  auto result = readDatabase(batch);
  auto& cachedBlocks = result.getCachedBlocks();

  for (const auto& miss : cacheMisses) {
    auto search = cachedBlocks.find(miss.first);
    exceptional_if<NotFoundError>(search == cachedBlocks.end(), "cached block info not found for given height");
    reval[miss.second] = std::move(search->second);
  }

  return reval;
}

CachedTransactionVector DatabaseBlockchainCache::getTransactions(ConstTransactionHashSpan ids) const {
  using namespace Xi;

  CachedTransactionVector reval{};
  reval.reserve(ids.size());

  auto cachedInfo = getTransactionInfos(ids);

  BlockchainReadBatch batch{};
  for (const auto& info : cachedInfo) {
    batch.requestRawBlock(info.blockIndex);
  }

  auto result = readDatabase(batch);
  auto& blocks = result.getRawBlocks();

  for (const auto& info : cachedInfo) {
    auto search = blocks.find(info.blockIndex);
    exceptional_if<NotFoundError>(search == blocks.end(), "block not contained for transaction cache info");

    CachedRawBlock rawBlock{std::move(search->second)};

    const auto transactionIndex = info.transactionIndex;
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

CachedTransactionInfoVector DatabaseBlockchainCache::getTransactionInfos(ConstTransactionHashSpan ids) const {
  using namespace Xi;

  CachedTransactionInfoVector reval{};
  reval.resize(ids.size());

  BlockchainReadBatch batch{};
  for (const auto& id : ids) {
    batch.requestCachedTransaction(id);
  }

  auto result = readDatabase(batch);
  auto& transactions = result.getCachedTransactions();

  size_t i = 0;
  for (const auto& id : ids) {
    auto search = transactions.find(id);
    exceptional_if<NotFoundError>(search == transactions.end(), "cached transaction not found");
    reval[i++] = std::move(search->second);
  }

  return reval;
}

BinaryArray DatabaseBlockchainCache::getRawTransaction(uint32_t blockIndex, uint32_t transactionIndex) const {
  const auto block = getBlockByIndex(blockIndex);
  const auto blockTemplate = fromBinaryArray<BlockTemplate>(block.blockTemplate);
  uint64_t txOffset = 1;
  if (transactionIndex == 0) {
    return toBinaryArray(blockTemplate.baseTransaction);
  }
  if (blockTemplate.staticRewardHash) {
    txOffset += 1;
    if (transactionIndex == 1) {
      return toBinaryArray(*currency.constructStaticRewardTx(blockTemplate).takeOrThrow());
    }
  }
  const auto txIndex = transactionIndex - txOffset;
  Xi::exceptional_if_not<Xi::NotFoundError>(txIndex < block.transactions.size());
  return block.transactions[txIndex];
}

std::vector<Crypto::Hash> DatabaseBlockchainCache::getTransactionHashes() const {
  assert(false);
  // TODO (njamnjam)
  return {};
}

uint64_t DatabaseBlockchainCache::getAvailableMixinsCount(DatabaseBlockchainCache::Amount amount, uint32_t blockIndex,
                                                          uint64_t threshold) const {
  auto batch = BlockchainReadBatch().requestKeyOutputGlobalIndexesCountForAmount(amount);
  auto result = readDatabase(batch);

  // std::unordered_map<IBlockchainCache::Amount, uint32_t>
  auto outputsCount = result.getKeyOutputGlobalIndexesCountForAmounts();
  auto outputsToCount = outputsCount[amount];

  XI_RETURN_SC_IF(outputsToCount == 0, 0);
  XI_RETURN_SC_IF(blockIndex >= getTopBlockIndex(), outputsToCount);

  uint32_t count =
      static_cast<uint32_t>(availableMixinsCountSearch(amount, blockIndex, 0, outputsToCount - 1, threshold));

#if !defined(NDEBUG)
  if (count < outputsToCount) {
    uint32_t nextIndex = count;
    std::vector<PackedOutIndex> nextPackedIndices{};
    if (!requestPackedOutputs(amount, Common::ArrayView<uint32_t>{std::addressof(nextIndex), 1}, database,
                              nextPackedIndices)) {
      throw std::runtime_error{"key output extraction failed"};
    }
    assert(nextPackedIndices.size() == 1);
    assert(nextPackedIndices.front().data.blockIndex > blockIndex);
  }
#endif

  return count;
}

uint64_t DatabaseBlockchainCache::availableMixinsCountSearch(DatabaseBlockchainCache::Amount amount,
                                                             uint32_t blockIndex, uint32_t leftIndex,
                                                             uint32_t rightIndex, uint64_t threshold) const {
  XI_RETURN_SC_IF(leftIndex == rightIndex, true);
  const uint32_t midIndex = leftIndex + (rightIndex - leftIndex) / 2;
  std::array<uint32_t, 3> boundaryIndices{{leftIndex, midIndex, rightIndex}};
  std::vector<PackedOutIndex> boundaryPackedIndices{};
  if (!requestPackedOutputs(amount, Common::ArrayView<uint32_t>{boundaryIndices.data(), boundaryIndices.size()},
                            database, boundaryPackedIndices)) {
    throw std::runtime_error{"key output extraction failed"};
  }
  assert(boundaryIndices.size() == 3);
  XI_RETURN_SC_IF(boundaryPackedIndices.front().data.blockIndex > blockIndex, leftIndex);
  XI_RETURN_SC_IF(boundaryPackedIndices.back().data.blockIndex <= blockIndex, rightIndex + 1);
  const auto mid = boundaryPackedIndices.at(1);

  if (mid.data.blockIndex > blockIndex) {
    XI_RETURN_SC_IF(midIndex == 0, 0);
    return availableMixinsCountSearch(amount, blockIndex, leftIndex, midIndex - 1, threshold);
  } else if (midIndex + 1 >= threshold) {
    return midIndex + 1;
  } else {
    return availableMixinsCountSearch(amount, blockIndex, midIndex + 1, rightIndex, threshold);
  }
}

std::vector<uint32_t> DatabaseBlockchainCache::getRandomOutsByAmount(uint64_t amount, size_t count,
                                                                     uint32_t blockIndex) const {
  auto batch = BlockchainReadBatch().requestKeyOutputGlobalIndexesCountForAmount(amount);
  auto result = readDatabase(batch);
  auto outputsCount = result.getKeyOutputGlobalIndexesCountForAmounts();
  auto outputsToPick = std::min(static_cast<uint32_t>(count), outputsCount[amount]);

  std::vector<uint32_t> resultOuts;
  resultOuts.reserve(outputsToPick);

  ShuffleGenerator<uint32_t, Xi::Crypto::Random::Engine<uint32_t>> generator(outputsCount[amount]);

  uint64_t cumulativeTries = 0;
  while (outputsToPick && cumulativeTries < outputsCount[amount]) {
    std::vector<uint32_t> globalIndexes;
    globalIndexes.reserve(outputsToPick);

    try {
      for (uint32_t i = 0; i < outputsToPick; ++i, globalIndexes.push_back(generator()))
        /* */;
    } catch (const SequenceEnded&) {
      logger(Logging::Trace) << "getRandomOutsByAmount: generator reached sequence end";
      return resultOuts;
    }

    std::vector<PackedOutIndex> outputs;
    if (extractKeyOtputIndexes(amount, Common::ArrayView<uint32_t>(globalIndexes.data(), globalIndexes.size()),
                               outputs) != ExtractOutputKeysResult::SUCCESS) {
      logger(Logging::Debugging) << "getRandomOutsByAmount: failed to extract key output indexes";
      throw std::runtime_error("Invalid output index");  // TODO: make error code
    }

    std::vector<ExtendedTransactionInfo> transactions;
    if (!requestExtendedTransactionInfos(outputs, database, transactions)) {
      logger(Logging::Trace) << "getRandomOutsByAmount: requestExtendedTransactionInfos failed";
      throw std::runtime_error("Error while requesting transactions");  // TODO: make error code
    }

    assert(globalIndexes.size() == transactions.size());

    for (size_t i = 0; i < transactions.size(); ++i) {
      if (!isTransactionSpendTimeUnlocked(transactions[i].unlockTime, blockIndex)) {
        continue;
      }

      resultOuts.push_back(globalIndexes[i]);
      --outputsToPick;
    }
    cumulativeTries += transactions.size();
  }

  return resultOuts;
}

ExtractOutputKeysResult DatabaseBlockchainCache::extractKeyOutputs(
    uint64_t amount, uint32_t blockIndex, Common::ArrayView<uint32_t> globalIndexes,
    std::function<ExtractOutputKeysResult(const CachedTransactionInfo& info, PackedOutIndex index,
                                          uint32_t globalIndex)>
        callback) const {
  XI_UNUSED(blockIndex);

  BlockchainReadBatch batch;
  for (auto it = globalIndexes.begin(); it != globalIndexes.end(); ++it) {
    batch.requestKeyOutputInfo(amount, *it);
  }

  auto result = readDatabase(batch).getKeyOutputInfo();
  std::map<std::pair<IBlockchainCache::Amount, IBlockchainCache::GlobalOutputIndex>, KeyOutputInfo> sortedResult(
      result.begin(), result.end());
  for (const auto& kv : sortedResult) {
    ExtendedTransactionInfo tx;
    tx.unlockTime = kv.second.unlockTime;
    tx.transactionHash = kv.second.transactionHash;
    tx.outputs.resize(kv.second.index.data.outputIndex + 1);
    TransactionAmountOutput output{};
    output.amount = amount;
    output.target = TransactionOutputTarget{KeyOutput{kv.second.publicKey}};
    tx.outputs[kv.second.index.data.outputIndex] = TransactionOutput{output};

    // TODO: change the interface of extractKeyOutputs to return vector of structures instead of passing callback as
    // predicate
    auto ret = callback(tx, kv.second.index, kv.first.second);
    if (ret != ExtractOutputKeysResult::SUCCESS) {
      logger(Logging::Debugging) << "extractKeyOutputs failed : callback returned error";
      return ret;
    }
  }

  return ExtractOutputKeysResult::SUCCESS;
}

std::vector<Crypto::Hash> DatabaseBlockchainCache::getTransactionHashesByPaymentId(const PaymentId& paymentId) const {
  auto countBatch = BlockchainReadBatch().requestTransactionCountByPaymentId(paymentId);
  uint32_t transactionsCountByPaymentId = readDatabase(countBatch).getTransactionCountByPaymentIds().at(paymentId);

  BlockchainReadBatch transactionBatch;
  for (uint32_t i = 0; i < transactionsCountByPaymentId; ++i) {
    transactionBatch.requestTransactionHashByPaymentId(paymentId, i);
  }

  auto result = readDatabase(transactionBatch);
  std::vector<Crypto::Hash> transactionHashes;
  transactionHashes.reserve(result.getTransactionHashesByPaymentIds().size());
  for (const auto& kv : result.getTransactionHashesByPaymentIds()) {
    transactionHashes.emplace_back(kv.second);
  }

  return transactionHashes;
}

std::vector<Crypto::Hash> DatabaseBlockchainCache::getBlockHashesByTimestamps(uint64_t timestampBegin,
                                                                              size_t secondsCount) const {
  std::vector<Crypto::Hash> blockHashes;
  if (secondsCount == 0) {
    return blockHashes;
  }

  BlockchainReadBatch batch;
  for (uint64_t timestamp = timestampBegin; timestamp < timestampBegin + static_cast<uint64_t>(secondsCount);
       ++timestamp) {
    batch.requestBlockHashesByTimestamp(timestamp);
  }

  auto result = readDatabase(batch);
  for (uint64_t timestamp = timestampBegin; timestamp < timestampBegin + static_cast<uint64_t>(secondsCount);
       ++timestamp) {
    if (result.getBlockHashesByTimestamp().count(timestamp) == 0) {
      continue;
    }

    const auto& hashes = result.getBlockHashesByTimestamp().at(timestamp);
    blockHashes.insert(blockHashes.end(), hashes.begin(), hashes.end());
  }

  return blockHashes;
}

DatabaseBlockchainCache::ExtendedPushedBlockInfo DatabaseBlockchainCache::getExtendedPushedBlockInfo(
    uint32_t blockIndex) const {
  assert(blockIndex <= getTopBlockIndex());

  auto batch = BlockchainReadBatch()
                   .requestRawBlock(blockIndex)
                   .requestCachedBlock(blockIndex)
                   .requestSpentKeyImagesByBlock(blockIndex);

  if (blockIndex > 0) {
    batch.requestCachedBlock(blockIndex - 1);
  }

  auto dbResult = readDatabase(batch);
  const CachedBlockInfo& blockInfo = dbResult.getCachedBlocks().at(blockIndex);
  const CachedBlockInfo& previousBlockInfo =
      blockIndex > 0 ? dbResult.getCachedBlocks().at(blockIndex - 1) : NULL_CACHED_BLOCK_INFO;

  ExtendedPushedBlockInfo extendedInfo;

  extendedInfo.pushedBlockInfo.rawBlock = dbResult.getRawBlocks().at(blockIndex);
  extendedInfo.pushedBlockInfo.blockSize = blockInfo.blobSize;
  extendedInfo.pushedBlockInfo.blockDifficulty =
      blockInfo.cumulativeDifficulty - previousBlockInfo.cumulativeDifficulty;
  extendedInfo.pushedBlockInfo.generatedCoins =
      blockInfo.alreadyGeneratedCoins - previousBlockInfo.alreadyGeneratedCoins;
  extendedInfo.pushedBlockInfo.timestamp = blockInfo.timestamp;

  const auto& spentKeyImages = dbResult.getSpentKeyImagesByBlock().at(blockIndex);

  extendedInfo.pushedBlockInfo.validatorState.spentKeyImages.insert(spentKeyImages.begin(), spentKeyImages.end());

  extendedInfo.timestamp = blockInfo.timestamp;

  return extendedInfo;
}

void DatabaseBlockchainCache::setParent(IBlockchainCache* ptr) {
  XI_UNUSED(ptr);
  throw std::runtime_error{"Parent caches are not supported for database based implmentations"};
}

void DatabaseBlockchainCache::addChild(IBlockchainCache* ptr) {
  if (std::find(children.begin(), children.end(), ptr) == children.end()) {
    children.push_back(ptr);
  }
}

bool DatabaseBlockchainCache::deleteChild(IBlockchainCache* ptr) {
  auto it = std::remove(children.begin(), children.end(), ptr);
  auto res = it != children.end();
  children.erase(it, children.end());
  return res;
}

BlockchainReadResult DatabaseBlockchainCache::readDatabase(BlockchainReadBatch& batch) const {
  auto result = database.read(batch);
  if (result) {
    logger(Logging::Error) << "failed to read database, error is " << result.message();
    throw std::runtime_error(result.message());
  }

  return batch.extractResult();
}

void DatabaseBlockchainCache::addGenesisBlock(CachedBlock&& genesisBlock) {
  uint64_t genesisGeneratedCoins = 0;
  for (const TransactionOutput& output : genesisBlock.getBlock().baseTransaction.outputs) {
    if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(output))) {
      genesisGeneratedCoins += amountOutput->amount;
    }
  }
  const auto staticReward = currency.constructStaticRewardTx(genesisBlock).takeOrThrow();
  if (staticReward.has_value()) {
    for (const TransactionOutput& output : staticReward->outputs) {
      if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(output))) {
        genesisGeneratedCoins += amountOutput->amount;
      }
    }
  }

  uint64_t genesisTransactionsSize = genesisBlock.getBlock().baseTransaction.binarySize();
  assert(genesisTransactionsSize < std::numeric_limits<uint32_t>::max());
  assert(genesisBlock.getBlock().transactionHashes.empty());

  BlockchainWriteBatch batch;

  CachedBlockInfo blockInfo{};
  blockInfo.blockHash = genesisBlock.getBlockHash();
  blockInfo.version = genesisBlock.getBlock().version;
  blockInfo.features = genesisBlock.getBlock().features;
  blockInfo.timestamp = genesisBlock.getBlock().timestamp;
  blockInfo.cumulativeDifficulty = 1;
  blockInfo.blobSize = genesisTransactionsSize;
  blockInfo.alreadyGeneratedCoins = genesisGeneratedCoins;
  blockInfo.alreadyGeneratedTransactions = staticReward.has_value() ? 2 : 1;

  auto baseTransaction = genesisBlock.getBlock().baseTransaction;
  auto cachedBaseTransaction = CachedTransaction{std::move(baseTransaction)};
  std::vector<Crypto::Hash> transactionHashes{cachedBaseTransaction.getTransactionHash()};

  pushTransaction(cachedBaseTransaction, 0, 0, batch, false);
  if (staticReward.has_value()) {
    CachedTransaction cachedStaticReward{*staticReward};
    transactionHashes.push_back(cachedStaticReward.getTransactionHash());
    pushTransaction(std::move(cachedStaticReward), 0, 1, batch, true);
  }

  batch.insertCachedBlock(blockInfo, 0, transactionHashes);
  batch.insertRawBlock(0, {toBinaryArray(genesisBlock.getBlock()), {}});
  batch.insertClosestTimestampBlockIndex(roundToMidnight(genesisBlock.getBlock().timestamp), 0);

  auto res = database.writeSync(batch);
  if (res) {
    logger(Logging::Error) << "addGenesisBlock failed: failed to write to database, " << res.message();
    throw std::runtime_error(res.message());
  }

  topBlockHash = genesisBlock.getBlockHash();
  topBlockIndex = genesisBlock.height().toIndex();
  topBlockVersion = genesisBlock.getBlock().version;

  unitsCache.push_back(blockInfo);
}

}  // namespace CryptoNote
