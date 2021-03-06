﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <vector>
#include <optional>

#include <Xi/Result.h>
#include <Xi/Concurrent/RecursiveLock.h>

#include "CryptoNoteCore/CryptoNote.h"

#include "AddBlockErrors.h"
#include "AddBlockErrorCondition.h"
#include "BlockchainExplorer/BlockchainExplorerData.h"
#include "BlockchainMessages.h"
#include "CachedBlock.h"
#include "Transactions/CachedTransaction.h"
#include "CoreStatistics.h"
#include "ICoreObserver.h"
#include "ICoreDefinitions.h"
#include "MessageQueue.h"

#include "CryptoNoteCore/Blockchain/IBlockchain.h"
#include "CryptoNoteCore/Blockchain/RawBlock.h"

namespace CryptoNote {

enum class CoreEvent { POOL_UPDATED, BLOCKHAIN_UPDATED };

enum struct BlockSource {
  AlternativeChain,
  MainChain,
};

struct CoreBlockInfo {
  BlockSource source;
  BlockHeight height;
  CachedRawBlock block;

  CoreBlockInfo(BlockSource _source, uint32_t index, RawBlock _block)
      : source{_source}, height{BlockHeight::fromIndex(index)}, block{std::move(_block)} {
    /* */
  }
};

template <typename _IdentifierT>
struct SegmentReference {
  std::shared_ptr<const IBlockchainCache> segment;
  BlockSource source;
  std::vector<_IdentifierT> content;

  explicit SegmentReference(std::shared_ptr<const IBlockchainCache> _segment, BlockSource _source)
      : segment{_segment}, source{_source}, content{} {
    /* */
  }
};

template <typename _IdentifierT>
using SegmentReferenceVector = std::vector<SegmentReference<_IdentifierT>>;

class ICore : public IBlockchain {
 public:
  virtual ~ICore() {
  }

  virtual Xi::Concurrent::RecursiveLock::lock_t lock() const = 0;

  virtual bool addMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) = 0;
  virtual bool removeMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) = 0;

  virtual uint32_t getTopBlockIndex() const = 0;
  virtual Crypto::Hash getTopBlockHash() const = 0;
  virtual BlockVersion getTopBlockVersion() const = 0;
  virtual Crypto::Hash getBlockHashByIndex(uint32_t blockIndex) const = 0;
  virtual uint32_t getBlockIndexByHash(const Crypto::Hash hash) const = 0;
  virtual uint64_t getBlockTimestampByIndex(uint32_t blockIndex) const = 0;

  virtual std::optional<BlockSource> hasBlock(const Crypto::Hash& blockHash) const = 0;
  virtual std::optional<CoreBlockInfo> getBlockByIndex(uint32_t index) const = 0;
  virtual std::optional<CoreBlockInfo> getBlockByHash(const Crypto::Hash& blockHash) const = 0;

  virtual std::vector<Crypto::Hash> buildSparseChain() const = 0;
  virtual Xi::Result<std::vector<Crypto::Hash>> findBlockchainSupplement(
      const std::vector<Crypto::Hash>& remoteBlockIds, size_t maxCount, uint32_t& totalBlockCount,
      uint32_t& startBlockIndex) const = 0;

  virtual std::vector<RawBlock> getBlocks(uint32_t startIndex, uint32_t count) const = 0;
  virtual void getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<RawBlock>& blocks,
                         std::vector<Crypto::Hash>& missedHashes) const = 0;
  virtual void getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<RawBlock>& blocks,
                         std::vector<Crypto::Hash>& missedHashes, uint64_t maxBinarySize) const = 0;

  virtual SegmentReferenceVector<BlockHash> queryBlockSegments(ConstBlockHashSpan hashes) const = 0;
  virtual SegmentReferenceVector<BlockHeight> queryBlockSegments(ConstBlockHeightSpan hashes) const = 0;
  virtual SegmentReferenceVector<BlockHash> queryTransactionSegments(ConstBlockHashSpan hashes) const = 0;

  virtual bool queryBlocks(const std::vector<Crypto::Hash>& blockHashes, uint64_t timestamp, uint32_t& startIndex,
                           uint32_t& currentIndex, uint32_t& fullOffset, std::vector<BlockFullInfo>& entries) const = 0;
  virtual bool queryBlocksLite(const std::vector<Crypto::Hash>& knownBlockHashes, uint64_t timestamp,
                               uint32_t& startIndex, uint32_t& currentIndex, uint32_t& fullOffset,
                               std::vector<BlockShortInfo>& entries) const = 0;

  virtual bool queryBlocksDetailed(const std::vector<Crypto::Hash>& knownBlockHashes, uint64_t timestamp,
                                   uint32_t& startIndex, uint32_t& currentIndex, uint32_t& fullOffset,
                                   std::vector<BlockDetails>& entries) const = 0;

  virtual bool hasTransaction(const Crypto::Hash& transactionHash) const = 0;
  virtual void getTransactions(const std::vector<Crypto::Hash>& transactionHashes,
                               std::vector<BinaryArray>& transactions, std::vector<Crypto::Hash>& missedHashes,
                               bool poolOnly = false) const = 0;

  virtual uint64_t getBlockDifficulty(uint32_t blockIndex) const = 0;
  virtual uint64_t getDifficultyForNextBlock() const = 0;

  virtual std::error_code addBlock(const CachedBlock& cachedBlock, RawBlock&& rawBlock) = 0;
  virtual std::error_code addBlock(RawBlock&& rawBlock) = 0;

  /*!
   * \brief addBlock tries to add block with already known transaction details
   * \param block A lite block missing transaction transaction details
   * \param txs Lite blocks may require additional transaction information from the publisher, if known they will be
   * placed here
   * \return A result of missing hashes if partial data was not found, otherwise an error that may contain a
   * success error code.
   */
  virtual Xi::Result<std::vector<Crypto::Hash>> addBlock(LiteBlock block, std::vector<CachedTransaction> txs = {}) = 0;

  virtual std::error_code submitBlock(BinaryArray&& rawBlockTemplate) = 0;

  virtual bool getTransactionGlobalIndexes(const Crypto::Hash& transactionHash,
                                           std::vector<uint32_t>& globalIndexes) const = 0;
  virtual bool getRandomOutputs(uint64_t amount, uint16_t count, std::vector<uint32_t>& globalIndexes,
                                std::vector<Crypto::PublicKey>& publicKeys) const = 0;
  virtual uint64_t getCurrentRequiredMixin(uint64_t amount) const = 0;

  virtual const class ITransactionPool& transactionPool() const = 0;
  virtual class ITransactionPool& transactionPool() = 0;

  virtual std::vector<Crypto::Hash> getPoolTransactionHashes() const = 0;
  virtual bool getPoolChanges(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                              std::vector<Transaction>& addedTransactions,
                              std::vector<Crypto::Hash>& deletedTransactions) const = 0;
  virtual bool getPoolChanges(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                              std::vector<BinaryArray>& addedTransactions,
                              std::vector<Crypto::Hash>& deletedTransactions) const = 0;
  virtual bool getPoolChangesLite(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                                  std::vector<TransactionPrefixInfo>& addedTransactions,
                                  std::vector<Crypto::Hash>& deletedTransactions) const = 0;

  virtual bool getBlockTemplate(BlockTemplate& b, const AccountPublicAddress& adr, uint64_t& difficulty,
                                uint32_t& height) const = 0;

  virtual CoreStatistics getCoreStatistics() const = 0;
  virtual bool isPruned() const = 0;

  [[nodiscard]] virtual bool save() = 0;
  [[nodiscard]] virtual bool load() = 0;

  virtual std::optional<BlockDetails> getBlockDetails(const Crypto::Hash& blockHash) const = 0;
  virtual TransactionDetails getTransactionDetails(const Crypto::Hash& transactionHash) const = 0;
  virtual std::vector<Crypto::Hash> getAlternativeBlockHashesByIndex(uint32_t blockIndex) const = 0;
  virtual std::vector<Crypto::Hash> getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount) const = 0;
  virtual std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const PaymentId& paymentId) const = 0;
};
}  // namespace CryptoNote
