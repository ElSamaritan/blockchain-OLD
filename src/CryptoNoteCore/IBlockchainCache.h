// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#pragma once

#include <vector>
#include <limits>
#include <utility>

#include <Common/ArrayView.h>

#include "CryptoNoteCore/CryptoNote.h"
#include "CryptoNoteCore/CachedBlock.h"
#include "CryptoNoteCore/Blockchain/RawBlock.h"
#include "CryptoNoteCore/DatabaseCacheData.h"
#include "CryptoNoteCore/Transactions/CachedTransaction.h"
#include "CryptoNoteCore/Transactions/TransactionValidatiorState.h"

namespace CryptoNote {

class ISerializer;
struct TransactionValidatorState;

enum class ExtractOutputKeysResult { SUCCESS, INVALID_GLOBAL_INDEX, OUTPUT_LOCKED, TIME_PROVIDER_FAILED, INVALID_TYPE };

const uint32_t INVALID_BLOCK_INDEX = std::numeric_limits<uint32_t>::max();

struct PushedBlockInfo {
  RawBlock rawBlock;
  TransactionValidatorState validatorState;
  size_t blockSize;
  uint64_t generatedCoins;
  uint64_t blockDifficulty;
  uint64_t timestamp;
};

class ISerializer;

struct SpentKeyImage {
  uint32_t blockIndex;
  Crypto::KeyImage keyImage;

  [[nodiscard]] bool serialize(ISerializer& s);
};

struct CachedTransactionInfo {
  uint32_t blockIndex;
  uint32_t transactionIndex;
  Crypto::Hash transactionHash;
  uint64_t unlockTime;
  std::vector<TransactionOutput> outputs;
  // needed for getTransactionGlobalIndexes query
  std::vector<uint32_t> globalIndexes;
  bool isDeterministicallyGenerated;

  [[nodiscard]] bool serialize(ISerializer& s);
};
using CachedTransactionInfoVector = std::vector<CachedTransactionInfo>;

// inherit here to avoid breaking IBlockchainCache interface
struct ExtendedTransactionInfo : CachedTransactionInfo {
  // CachedTransactionInfo tx;
  std::map<uint64_t, std::vector<uint32_t>>
      amountToKeyIndexes;  // global key output indexes spawned in this transaction
  [[nodiscard]] bool serialize(ISerializer& s);
};

struct CachedBlockInfo {
  Crypto::Hash blockHash{Crypto::Hash::Null};
  BlockVersion version{BlockVersion::Null};
  BlockFeature features{BlockFeature::None};
  uint64_t timestamp;
  uint64_t blobSize;
  uint64_t cumulativeDifficulty;
  uint64_t alreadyGeneratedCoins;
  uint64_t alreadyGeneratedTransactions;

  [[nodiscard]] bool serialize(ISerializer& s);
};
using CachedBlockInfoVector = std::vector<CachedBlockInfo>;

struct OutputGlobalIndexesForAmount {
  uint32_t startIndex = 0;

  // 1. This container must be sorted by PackedOutIndex::blockIndex and PackedOutIndex::transactionIndex
  // 2. GlobalOutputIndex for particular output is calculated as following: startIndex + index in vector
  std::vector<PackedOutIndex> outputs;

  [[nodiscard]] bool serialize(ISerializer& s);
};

struct PaymentIdTransactionHashPair {
  PaymentId paymentId;
  Crypto::Hash transactionHash;

  [[nodiscard]] bool serialize(ISerializer& s);
};

[[nodiscard]] bool serialize(PackedOutIndex& value, Common::StringView name, CryptoNote::ISerializer& serializer);

class UseGenesis {
 public:
  explicit UseGenesis(bool u) : use(u) {
  }
  // emulate boolean flag
  operator bool() {
    return use;
  }

 private:
  bool use = false;
};

struct CachedBlockInfo;
struct CachedTransactionInfo;

class IBlockchainCache : public std::enable_shared_from_this<IBlockchainCache> {
 public:
  using BlockIndex = uint32_t;
  using GlobalOutputIndex = uint32_t;
  using Amount = uint64_t;

  virtual ~IBlockchainCache() = default;

  virtual RawBlock getBlockByIndex(uint32_t index) const = 0;
  virtual BinaryArray getRawTransaction(uint32_t blockIndex, uint32_t transactionIndex) const = 0;
  virtual std::shared_ptr<IBlockchainCache> split(uint32_t splitBlockIndex) = 0;
  virtual void pushBlock(const CachedBlock& cachedBlock, const std::vector<CachedTransaction>& cachedTransactions,
                         const TransactionValidatorState& validatorState, size_t blockSize, uint64_t generatedCoins,
                         uint64_t blockDifficulty, RawBlock&& rawBlock) = 0;
  virtual PushedBlockInfo getPushedBlockInfo(uint32_t index) const = 0;

  [[nodiscard]] virtual bool checkIfSpent(const Crypto::KeyImage& keyImage, uint32_t blockIndex) const = 0;
  [[nodiscard]] virtual bool checkIfSpent(const Crypto::KeyImage& keyImage) const = 0;
  [[nodiscard]] virtual bool checkIfAnySpent(const Crypto::KeyImageSet& keyImages, uint32_t blockIndex) const = 0;

  [[nodiscard]] virtual uint64_t getAvailableMixinsCount(Amount amount, uint32_t blockIndex,
                                                         uint64_t threshold) const = 0;

  /*!
   * \brief isTransactionSpendTimeUnlocked Validates if the output of a transaction is unlocked, eligible to
   * be used for a new transaction.
   * \param unlockTime The transaction unlock index/timestamp
   * \param blockIndex The block index this transaction is mined in
   *
   * This overload assumes the transaction belongs not to any block and is validated to be mined in
   * the future. For timestamp encoded unlocks, the current timestamp will be taken otherwise the index
   * of the next block that will be mined.
   *
   * \return True if the output can be used, otherwise false
   */
  [[nodiscard]] virtual bool isTransactionSpendTimeUnlocked(uint64_t unlockTime) const = 0;

  /*!
   * \brief isTransactionSpendTimeUnlocked Validates if the output of a transaction is unlocked, eligible to
   * be used for a new transaction.
   * \param unlockTime The transaction unlock index/timestamp
   * \param blockIndex The block index to validate against
   *
   * In this case the transaction output must haven been unlocked at the state of the blockchain the
   * block with the given index was mined. In case of a timestamp encoded unlock, the timestamp of the
   * indexed block is taken.
   *
   * \return True if the output can be used, otherwise false
   */
  [[nodiscard]] virtual bool isTransactionSpendTimeUnlocked(uint64_t unlockTime, uint32_t blockIndex) const = 0;

  /*!
   * \brief isTransactionSpendTimeUnlocked Validates if the output of a transaction is unlocked, eligible to
   * be used for a new transaction.
   * \param unlockTime The transaction unlock index/timestamp
   * \param blockIndex The block index to validate against
   * \param timestamp A specific timestamp to test against.
   *
   * This overload assuems you gatehered all information correctly already and simply test the unlock
   * against the block index or the timestamp, based on the unlock encoding. This is useful for pool
   * transaction validations as you are not able to know with which timestamp the next block will be mined.
   *
   * \return True if the output can be used, otherwise false
   */
  [[nodiscard]] virtual bool isTransactionSpendTimeUnlocked(uint64_t unlockTime, uint32_t blockIndex,
                                                            uint64_t timestamp) const = 0;

  virtual ExtractOutputKeysResult extractKeyOutputKeys(uint64_t amount, Common::ArrayView<uint32_t> globalIndexes,
                                                       std::vector<Crypto::PublicKey>& publicKeys) const = 0;
  virtual ExtractOutputKeysResult extractKeyOutputKeys(uint64_t amount, uint32_t blockIndex,
                                                       Common::ArrayView<uint32_t> globalIndexes,
                                                       std::vector<Crypto::PublicKey>& publicKeys) const = 0;
  virtual ExtractOutputKeysResult extractKeyOutputKeys(uint64_t amount, uint32_t blockIndex,
                                                       Common::ArrayView<uint32_t> globalIndexes,
                                                       std::vector<Crypto::PublicKey>& publicKeys,
                                                       uint64_t timestamp) const = 0;

  virtual ExtractOutputKeysResult extractKeyOtputIndexes(uint64_t amount, Common::ArrayView<uint32_t> globalIndexes,
                                                         std::vector<PackedOutIndex>& outIndexes) const = 0;
  virtual ExtractOutputKeysResult extractKeyOtputReferences(
      uint64_t amount, Common::ArrayView<uint32_t> globalIndexes,
      std::vector<std::pair<Crypto::Hash, size_t>>& outputReferences) const = 0;
  // TODO: get rid of pred in this method. return vector of KeyOutputInfo structures
  virtual ExtractOutputKeysResult extractKeyOutputs(
      uint64_t amount, uint32_t blockIndex, Common::ArrayView<uint32_t> globalIndexes,
      std::function<ExtractOutputKeysResult(const CachedTransactionInfo& info, PackedOutIndex index,
                                            uint32_t globalIndex)>
          pred) const = 0;

  virtual std::map<Amount, std::map<GlobalOutputIndex, KeyOutputInfo>> extractKeyOutputs(
      const std::unordered_map<Amount, GlobalOutputIndexSet>& references, uint32_t blockIndex) const = 0;

  virtual uint32_t getTopBlockIndex() const = 0;
  virtual const Crypto::Hash& getTopBlockHash() const = 0;
  virtual BlockVersion getTopBlockVersion() const = 0;
  virtual uint32_t getBlockCount() const = 0;
  virtual bool hasBlock(const Crypto::Hash& blockHash) const = 0;
  virtual uint32_t getBlockIndex(const Crypto::Hash& blockHash) const = 0;

  virtual BlockHeightVector getBlockHeights(ConstBlockHashSpan hashes) const = 0;
  virtual RawBlockVector getBlocks(ConstBlockHeightSpan heights) const = 0;
  virtual CachedBlockInfoVector getBlockInfos(ConstBlockHeightSpan height) const = 0;
  virtual CachedTransactionVector getTransactions(ConstTransactionHashSpan ids) const = 0;
  virtual CachedTransactionInfoVector getTransactionInfos(ConstTransactionHashSpan ids) const = 0;

  virtual bool hasTransaction(const Crypto::Hash& transactionHash) const = 0;

  virtual std::vector<uint64_t> getLastTimestamps(size_t count) const = 0;
  virtual std::vector<uint64_t> getLastTimestamps(size_t count, uint32_t blockIndex, UseGenesis) const = 0;

  virtual std::vector<uint64_t> getLastBlocksSizes(size_t count) const = 0;
  virtual std::vector<uint64_t> getLastBlocksSizes(size_t count, uint32_t blockIndex, UseGenesis) const = 0;

  virtual std::vector<uint64_t> getLastCumulativeDifficulties(size_t count, uint32_t blockIndex, UseGenesis) const = 0;
  virtual std::vector<uint64_t> getLastCumulativeDifficulties(size_t count) const = 0;

  [[nodiscard]] virtual uint64_t getCurrentBlockSize() const = 0;
  [[nodiscard]] virtual uint64_t getCurrentBlockSize(uint32_t blockIndex) const = 0;

  virtual uint64_t getDifficultyForNextBlock(BlockVersion version) const = 0;
  virtual uint64_t getDifficultyForNextBlock(BlockVersion version, uint32_t blockIndex) const = 0;

  virtual uint64_t getCurrentCumulativeDifficulty() const = 0;
  virtual uint64_t getCurrentCumulativeDifficulty(uint32_t blockIndex) const = 0;

  virtual uint64_t getAlreadyGeneratedCoins() const = 0;
  virtual uint64_t getAlreadyGeneratedCoins(uint32_t blockIndex) const = 0;

  virtual uint64_t getAlreadyGeneratedTransactions(uint32_t blockIndex) const = 0;

  virtual Crypto::Hash getBlockHash(uint32_t blockIndex) const = 0;
  virtual std::vector<Crypto::Hash> getBlockHashes(uint32_t startIndex, size_t maxCount) const = 0;

  virtual IBlockchainCache* getParent() const = 0;
  virtual void setParent(IBlockchainCache* parent) = 0;
  virtual uint32_t getStartBlockIndex() const = 0;

  virtual size_t getKeyOutputsCountForAmount(uint64_t amount, uint32_t blockIndex) const = 0;

  virtual uint32_t getTimestampLowerBoundBlockIndex(uint64_t timestamp) const = 0;

  // NOTE: shouldn't be recursive otherwise we'll get quadratic complexity
  virtual void getRawTransactions(const std::vector<Crypto::Hash>& transactions,
                                  std::vector<BinaryArray>& foundTransactions,
                                  std::vector<Crypto::Hash>& missedTransactions) const = 0;
  virtual std::vector<BinaryArray> getRawTransactions(const std::vector<Crypto::Hash>& transactions,
                                                      std::vector<Crypto::Hash>& missedTransactions) const = 0;
  virtual std::vector<BinaryArray> getRawTransactions(const std::vector<Crypto::Hash>& transactions) const = 0;

  // NOTE: not recursive!
  virtual bool getTransactionGlobalIndexes(const Crypto::Hash& transactionHash,
                                           std::vector<uint32_t>& globalIndexes) const = 0;

  virtual size_t getTransactionCount() const = 0;

  virtual uint32_t getBlockIndexContainingTx(const Crypto::Hash& transactionHash) const = 0;

  virtual size_t getChildCount() const = 0;
  virtual void addChild(IBlockchainCache*) = 0;
  virtual bool deleteChild(IBlockchainCache*) = 0;

  [[nodiscard]] virtual bool save() = 0;
  [[nodiscard]] virtual bool load() = 0;

  virtual std::vector<uint64_t> getLastUnits(size_t count, uint32_t blockIndex, UseGenesis use,
                                             std::function<uint64_t(const CachedBlockInfo&)> pred) const = 0;
  virtual std::vector<Crypto::Hash> getTransactionHashes() const = 0;

  virtual std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const PaymentId& paymentId) const = 0;
  virtual std::vector<Crypto::Hash> getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount) const = 0;

  virtual std::vector<uint32_t> getRandomOutsByAmount(uint64_t amount, size_t count, uint32_t blockIndex) const = 0;
};

using WeakIBlockchainCache = std::weak_ptr<IBlockchainCache>;
using SharedIBlockchainCache = std::shared_ptr<IBlockchainCache>;

}  // namespace CryptoNote
