﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <ctime>
#include <vector>
#include <unordered_map>
#include <string>

#include "BlockchainCache.h"
#include "BlockchainMessages.h"
#include "CachedBlock.h"
#include "CachedTransaction.h"
#include "Currency.h"
#include "Checkpoints.h"
#include "IBlockchainCache.h"
#include "IBlockchainCacheFactory.h"
#include "ICore.h"
#include "ICoreInformation.h"
#include "IMainChainStorage.h"
#include "ITransactionPool.h"
#include "ITransactionPoolCleaner.h"
#include "IUpgradeManager.h"
#include <Logging/LoggerMessage.h>
#include "MessageQueue.h"
#include "TransactionValidatiorState.h"
#include "SwappedVector.h"

#include <System/ContextGroup.h>

namespace CryptoNote {

class Core : public ICore, public ICoreInformation {
 public:
  Core(const Currency& currency, Logging::ILogger& logger, Checkpoints&& checkpoints, System::Dispatcher& dispatcher,
       std::unique_ptr<IBlockchainCacheFactory>&& blockchainCacheFactory,
       std::unique_ptr<IMainChainStorage>&& mainChainStorage);
  virtual ~Core();

  virtual bool addMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) override;
  virtual bool removeMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) override;

  virtual uint32_t getTopBlockIndex() const override;
  virtual Crypto::Hash getTopBlockHash() const override;
  virtual Crypto::Hash getBlockHashByIndex(uint32_t blockIndex) const override;
  virtual uint64_t getBlockTimestampByIndex(uint32_t blockIndex) const override;

  virtual bool hasBlock(const Crypto::Hash& blockHash) const override;
  virtual BlockTemplate getBlockByIndex(uint32_t index) const override;
  virtual BlockTemplate getBlockByHash(const Crypto::Hash& blockHash) const override;

  virtual std::vector<Crypto::Hash> buildSparseChain() const override;
  virtual std::vector<Crypto::Hash> findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds,
                                                             size_t maxCount, uint32_t& totalBlockCount,
                                                             uint32_t& startBlockIndex) const override;

  virtual std::vector<RawBlock> getBlocks(uint32_t minIndex, uint32_t count) const override;
  virtual void getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<RawBlock>& blocks,
                         std::vector<Crypto::Hash>& missedHashes) const override;
  virtual bool queryBlocks(const std::vector<Crypto::Hash>& blockHashes, uint64_t timestamp, uint32_t& startIndex,
                           uint32_t& currentIndex, uint32_t& fullOffset,
                           std::vector<BlockFullInfo>& entries) const override;
  virtual bool queryBlocksLite(const std::vector<Crypto::Hash>& knownBlockHashes, uint64_t timestamp,
                               uint32_t& startIndex, uint32_t& currentIndex, uint32_t& fullOffset,
                               std::vector<BlockShortInfo>& entries) const override;
  virtual bool queryBlocksDetailed(const std::vector<Crypto::Hash>& knownBlockHashes, uint64_t timestamp,
                                   uint32_t& startIndex, uint32_t& currentIndex, uint32_t& fullOffset,
                                   std::vector<BlockDetails>& entries) const override;

  virtual bool hasTransaction(const Crypto::Hash& transactionHash) const override;
  virtual void getTransactions(const std::vector<Crypto::Hash>& transactionHashes,
                               std::vector<BinaryArray>& transactions,
                               std::vector<Crypto::Hash>& missedHashes) const override;

  virtual uint64_t getBlockDifficulty(uint32_t blockIndex) const override;
  virtual uint64_t getDifficultyForNextBlock() const override;

  virtual std::error_code addBlock(const CachedBlock& cachedBlock, RawBlock&& rawBlock) override;
  virtual std::error_code addBlock(RawBlock&& rawBlock) override;

  virtual std::error_code submitBlock(BinaryArray&& rawBlockTemplate) override;

  virtual bool getTransactionGlobalIndexes(const Crypto::Hash& transactionHash,
                                           std::vector<uint32_t>& globalIndexes) const override;
  virtual bool getRandomOutputs(uint64_t amount, uint16_t count, std::vector<uint32_t>& globalIndexes,
                                std::vector<Crypto::PublicKey>& publicKeys) const override;

  virtual bool addTransactionToPool(const BinaryArray& transactionBinaryArray) override;

  virtual std::vector<Crypto::Hash> getPoolTransactionHashes() const override;
  virtual bool getPoolChanges(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                              std::vector<BinaryArray>& addedTransactions,
                              std::vector<Crypto::Hash>& deletedTransactions) const override;
  virtual bool getPoolChangesLite(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                                  std::vector<TransactionPrefixInfo>& addedTransactions,
                                  std::vector<Crypto::Hash>& deletedTransactions) const override;

  virtual bool getBlockTemplate(BlockTemplate& b, const AccountPublicAddress& adr, const BinaryArray& extraNonce,
                                uint64_t& difficulty, uint32_t& height) const override;

  virtual CoreStatistics getCoreStatistics() const override;

  virtual std::time_t getStartTime() const;

  // ICoreInformation
  virtual size_t getPoolTransactionCount() const override;
  virtual size_t getBlockchainTransactionCount() const override;
  virtual size_t getAlternativeBlockCount() const override;
  virtual uint64_t getTotalGeneratedAmount() const override;
  virtual std::vector<BlockTemplate> getAlternativeBlocks() const override;
  virtual std::vector<Transaction> getPoolTransactions() const override;

  const Currency& getCurrency() const;

  virtual void save() override;
  virtual void load() override;

  virtual BlockDetails getBlockDetails(const Crypto::Hash& blockHash) const override;
  BlockDetails getBlockDetails(const uint32_t blockHeight) const;
  virtual TransactionDetails getTransactionDetails(const Crypto::Hash& transactionHash) const override;
  virtual std::vector<Crypto::Hash> getAlternativeBlockHashesByIndex(uint32_t blockIndex) const override;
  virtual std::vector<Crypto::Hash> getBlockHashesByTimestamps(uint64_t timestampBegin,
                                                               size_t secondsCount) const override;
  virtual std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const Crypto::Hash& paymentId) const override;

  virtual uint64_t get_current_blockchain_height() const;

 private:
  const Currency& currency;
  System::Dispatcher& dispatcher;
  System::ContextGroup contextGroup;
  Logging::LoggerRef logger;
  Checkpoints checkpoints;
  std::unique_ptr<IUpgradeManager> upgradeManager;
  std::vector<std::unique_ptr<IBlockchainCache>> chainsStorage;
  std::vector<IBlockchainCache*> chainsLeaves;
  std::unique_ptr<ITransactionPoolCleanWrapper> transactionPool;
  std::unordered_set<IBlockchainCache*> mainChainSet;

  std::string dataFolder;

  IntrusiveLinkedList<MessageQueue<BlockchainMessage>> queueList;
  std::unique_ptr<IBlockchainCacheFactory> blockchainCacheFactory;
  std::unique_ptr<IMainChainStorage> mainChainStorage;
  bool initialized;

  time_t start_time;

  size_t blockMedianSize;

  void throwIfNotInitialized() const;
  bool extractTransactions(const std::vector<BinaryArray>& rawTransactions,
                           std::vector<CachedTransaction>& transactions, uint64_t& cumulativeSize);

  std::error_code validateSemantic(const Transaction& transaction, uint64_t& fee, uint32_t blockIndex);
  std::error_code validateTransaction(const CachedTransaction& transaction, TransactionValidatorState& state,
                                      IBlockchainCache* cache, uint64_t& fee, uint32_t blockIndex);

  uint32_t findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds) const;
  std::vector<Crypto::Hash> getBlockHashes(uint32_t startBlockIndex, uint32_t maxCount) const;

  std::error_code validateBlock(const CachedBlock& block, IBlockchainCache* cache, uint64_t& minerReward);

  uint64_t getAdjustedTime() const;
  void updateMainChainSet();
  IBlockchainCache* findSegmentContainingBlock(const Crypto::Hash& blockHash) const;
  IBlockchainCache* findSegmentContainingBlock(uint32_t blockHeight) const;
  IBlockchainCache* findMainChainSegmentContainingBlock(const Crypto::Hash& blockHash) const;
  IBlockchainCache* findAlternativeSegmentContainingBlock(const Crypto::Hash& blockHash) const;

  IBlockchainCache* findMainChainSegmentContainingBlock(uint32_t blockIndex) const;
  IBlockchainCache* findAlternativeSegmentContainingBlock(uint32_t blockIndex) const;

  IBlockchainCache* findSegmentContainingTransaction(const Crypto::Hash& transactionHash) const;

  BlockTemplate restoreBlockTemplate(IBlockchainCache* blockchainCache, uint32_t blockIndex) const;
  std::vector<Crypto::Hash> doBuildSparseChain(const Crypto::Hash& blockHash) const;

  RawBlock getRawBlock(IBlockchainCache* segment, uint32_t blockIndex) const;

  size_t pushBlockHashes(uint32_t startIndex, uint32_t fullOffset, size_t maxItemsCount,
                         std::vector<BlockShortInfo>& entries) const;
  size_t pushBlockHashes(uint32_t startIndex, uint32_t fullOffset, size_t maxItemsCount,
                         std::vector<BlockFullInfo>& entries) const;
  size_t pushBlockHashes(uint32_t startIndex, uint32_t fullOffset, size_t maxItemsCount,
                         std::vector<BlockDetails>& entries) const;
  bool notifyObservers(BlockchainMessage&& msg);
  void fillQueryBlockFullInfo(uint32_t fullOffset, uint32_t currentIndex, size_t maxItemsCount,
                              std::vector<BlockFullInfo>& entries) const;
  void fillQueryBlockShortInfo(uint32_t fullOffset, uint32_t currentIndex, size_t maxItemsCount,
                               std::vector<BlockShortInfo>& entries) const;
  void fillQueryBlockDetails(uint32_t fullOffset, uint32_t currentIndex, size_t maxItemsCount,
                             std::vector<BlockDetails>& entries) const;

  void getTransactionPoolDifference(const std::vector<Crypto::Hash>& knownHashes,
                                    std::vector<Crypto::Hash>& newTransactions,
                                    std::vector<Crypto::Hash>& deletedTransactions) const;

  uint8_t getBlockMajorVersionForHeight(uint32_t height) const;
  size_t calculateCumulativeBlocksizeLimit(uint32_t height) const;
  void fillBlockTemplate(BlockTemplate& block, size_t medianSize, size_t maxCumulativeSize, size_t& transactionsSize,
                         uint64_t& fee) const;
  void deleteAlternativeChains();
  void deleteLeaf(size_t leafIndex);
  void mergeMainChainSegments();
  void mergeSegments(IBlockchainCache* acceptingSegment, IBlockchainCache* segment);
  TransactionDetails getTransactionDetails(const Crypto::Hash& transactionHash, IBlockchainCache* segment,
                                           bool foundInPool) const;
  void notifyOnSuccess(error::AddBlockErrorCode opResult, uint32_t previousBlockIndex, const CachedBlock& cachedBlock,
                       const IBlockchainCache& cache);
  void copyTransactionsToPool(IBlockchainCache* alt);

  void actualizePoolTransactions();
  void actualizePoolTransactionsLite(
      const TransactionValidatorState& validatorState);  // Checks pool txs only for double spend.

  void transactionPoolCleaningProcedure();
  void updateBlockMedianSize();
  bool addTransactionToPool(CachedTransaction&& cachedTransaction);
  bool isTransactionValidForPool(const CachedTransaction& cachedTransaction, TransactionValidatorState& validatorState);

  void initRootSegment();
  void importBlocksFromStorage();
  void cutSegment(IBlockchainCache& segment, uint32_t startIndex);

  void switchMainChainStorage(uint32_t splitBlockIndex, IBlockchainCache& newChain);
};

}  // namespace CryptoNote
