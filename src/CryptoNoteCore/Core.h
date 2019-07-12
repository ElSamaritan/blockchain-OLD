// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <ctime>
#include <vector>
#include <unordered_map>
#include <string>

#include <Xi/Result.h>
#include <Xi/Concurrent/RecursiveLock.h>

#include <Common/ObserverManager.h>
#include <Logging/LoggerMessage.h>
#include <System/ContextGroup.h>

#include "BlockchainCache.h"
#include "BlockchainMessages.h"
#include "CachedBlock.h"
#include "Transactions/CachedTransaction.h"
#include "Currency.h"
#include "Checkpoints.h"
#include "IBlockchainCache.h"
#include "IBlockchainCacheFactory.h"
#include "ICore.h"
#include "ICoreInformation.h"
#include "IMainChainStorage.h"
#include "IUpgradeManager.h"
#include "MessageQueue.h"
#include "SwappedVector.h"
#include "CryptoNoteCore/Blockchain/IBlockchain.h"
#include "CryptoNoteCore/Transactions/ITransactionPool.h"
#include "CryptoNoteCore/Transactions/ITransactionPoolObserver.h"
#include "CryptoNoteCore/Transactions/ITransactionPoolCleaner.h"
#include "CryptoNoteCore/Transactions/TransactionValidatiorState.h"
#include "Xi/Blockchain/Explorer/Data/Data.hpp"

namespace CryptoNote {

class Core : public ICore, public ICoreInformation, ITransactionPoolObserver {
 public:
  Core(const Currency& currency, Logging::ILogger& logger, Checkpoints& checkpoints, System::Dispatcher& dispatcher,
       bool isLightNode, std::unique_ptr<IBlockchainCacheFactory>&& blockchainCacheFactory,
       std::unique_ptr<IMainChainStorage>&& mainChainStorage);
  virtual ~Core() override;

  virtual Xi::Concurrent::RecursiveLock::lock_t lock() const override;
  virtual bool addMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) override;
  virtual bool removeMessageQueue(MessageQueue<BlockchainMessage>& messageQueue) override;

  // --------------------------------------------- IBlockchain BEGIN ---------------------------------------------------
 public:
  void addObserver(IBlockchainObserver* observer) override;
  void removeObserver(IBlockchainObserver* observer) override;

  const Currency& currency() const override;
  const IBlockchainCache* mainChain() const override;
  const IUpgradeManager& upgradeManager() const override;
  const ITimeProvider& timeProvider() const override;
  bool isInitialized() const override;
  // ---------------------------------------------- IBlockchain END ----------------------------------------------------

  // ------------------------------------- ITransactionPoolObserver BEGIN ----------------------------------------------
 private:
  void transactionDeletedFromPool(const Crypto::Hash& hash, ITransactionPoolObserver::DeletionReason reason) override;
  void transactionAddedToPool(const Crypto::Hash& hash, ITransactionPoolObserver::AdditionReason reason) override;
  // -------------------------------------- ITransactionPoolObserver END -----------------------------------------------

 public:
  virtual uint32_t getTopBlockIndex() const override;
  virtual Crypto::Hash getTopBlockHash() const override;
  virtual BlockVersion getTopBlockVersion() const override;
  virtual Crypto::Hash getBlockHashByIndex(uint32_t blockIndex) const override;
  virtual uint64_t getBlockTimestampByIndex(uint32_t blockIndex) const override;

  virtual std::optional<BlockSource> hasBlock(const Crypto::Hash& blockHash) const override;
  virtual std::optional<CoreBlockInfo> getBlockByIndex(uint32_t index) const override;
  virtual std::optional<CoreBlockInfo> getBlockByHash(const Crypto::Hash& blockHash) const override;

  virtual std::vector<Crypto::Hash> buildSparseChain() const override;
  virtual Xi::Result<std::vector<Crypto::Hash>> findBlockchainSupplement(
      const std::vector<Crypto::Hash>& remoteBlockIds, size_t maxCount, uint32_t& totalBlockCount,
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
                               std::vector<BinaryArray>& transactions, std::vector<Crypto::Hash>& missedHashes,
                               bool poolOnly = false) const override;

  virtual uint64_t getBlockDifficulty(uint32_t blockIndex) const override;
  virtual uint64_t getDifficultyForNextBlock() const override;

  virtual std::error_code addBlock(const CachedBlock& cachedBlock, RawBlock&& rawBlock) override;
  virtual std::error_code addBlock(RawBlock&& rawBlock) override;
  virtual Xi::Result<std::vector<Crypto::Hash>> addBlock(LiteBlock rawLiteBlock,
                                                         std::vector<CachedTransaction> txs) override;

  virtual std::error_code submitBlock(BinaryArray&& rawBlockTemplate) override;

  virtual bool getTransactionGlobalIndexes(const Crypto::Hash& transactionHash,
                                           std::vector<uint32_t>& globalIndexes) const override;
  virtual bool getRandomOutputs(uint64_t amount, uint16_t count, std::vector<uint32_t>& globalIndexes,
                                std::vector<Crypto::PublicKey>& publicKeys) const override;

  virtual std::vector<Crypto::Hash> getPoolTransactionHashes() const override;
  virtual bool getPoolChanges(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                              std::vector<Transaction>& addedTransactions,
                              std::vector<Crypto::Hash>& deletedTransactions) const override;
  virtual bool getPoolChanges(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                              std::vector<BinaryArray>& addedTransactions,
                              std::vector<Crypto::Hash>& deletedTransactions) const override;
  virtual bool getPoolChangesLite(const Crypto::Hash& lastBlockHash, const std::vector<Crypto::Hash>& knownHashes,
                                  std::vector<TransactionPrefixInfo>& addedTransactions,
                                  std::vector<Crypto::Hash>& deletedTransactions) const override;

  virtual bool getBlockTemplate(BlockTemplate& b, const AccountPublicAddress& adr, const BinaryArray& extraNonce,
                                uint64_t& difficulty, uint32_t& index) const override;

  virtual CoreStatistics getCoreStatistics() const override;
  virtual bool isPruned() const override;

  virtual std::time_t getStartTime() const;

  // ICoreInformation
  const ITransactionPool& transactionPool() const override;
  ITransactionPool& transactionPool() override;

  virtual size_t getBlockchainTransactionCount() const override;
  virtual size_t getAlternativeBlockCount() const override;
  virtual uint64_t getTotalGeneratedAmount() const override;
  virtual std::vector<BlockTemplate> getAlternativeBlocks() const override;
  virtual std::vector<Transaction> getPoolTransactions() const override;

  const Currency& getCurrency() const;

  [[nodiscard]] virtual bool save() override;
  [[nodiscard]] virtual bool load() override;

  virtual std::optional<BlockDetails> getBlockDetails(const Crypto::Hash& blockHash) const override;
  std::optional<BlockDetails> getBlockDetails(const uint32_t blockHeight) const;
  virtual TransactionDetails getTransactionDetails(const Crypto::Hash& transactionHash) const override;
  virtual std::vector<Crypto::Hash> getAlternativeBlockHashesByIndex(uint32_t blockIndex) const override;
  virtual std::vector<Crypto::Hash> getBlockHashesByTimestamps(uint64_t timestampBegin,
                                                               size_t secondsCount) const override;
  virtual std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const PaymentId& paymentId) const override;

  virtual SegmentReferenceVector<BlockHash> queryBlockSegments(ConstBlockHashSpan hashes) const override;
  virtual SegmentReferenceVector<BlockHeight> queryBlockSegments(ConstBlockHeightSpan heights) const override;
  virtual SegmentReferenceVector<BlockHash> queryTransactionSegments(ConstBlockHashSpan hashes) const override;

  virtual BlockHeight get_current_blockchain_height() const;

 private:
  const Currency& m_currency;
  System::Dispatcher& dispatcher;
  System::ContextGroup contextGroup;
  Logging::LoggerRef logger;
  Checkpoints& checkpoints;
  std::unique_ptr<IUpgradeManager> m_upgradeManager;
  std::vector<std::shared_ptr<IBlockchainCache>> chainsStorage;
  std::vector<IBlockchainCache*> chainsLeaves;
  std::unique_ptr<ITransactionPool> m_transactionPool;
  std::unordered_set<IBlockchainCache*> mainChainSet;

  std::string dataFolder;
  bool m_isLightNode = false;
  Tools::ObserverManager<IBlockchainObserver> m_blockchainObservers;
  std::unique_ptr<ITimeProvider> m_timeProvider;

  IntrusiveLinkedList<MessageQueue<BlockchainMessage>> queueList;
  std::unique_ptr<IBlockchainCacheFactory> blockchainCacheFactory;
  std::unique_ptr<IMainChainStorage> mainChainStorage;
  bool initialized;

  time_t start_time;
  Xi::Concurrent::RecursiveLock m_access;
  size_t blockMedianSize;

  void throwIfNotInitialized() const;

  bool extractTransactions(const std::vector<BinaryArray>& rawTransactions,
                           std::vector<CachedTransaction>& transactions, uint64_t& cumulativeSize,
                           BlockVersion blockVersion);

  Xi::Result<uint32_t> findBlockchainSupplement(const std::vector<Crypto::Hash>& remoteBlockIds) const;
  std::vector<Crypto::Hash> getBlockHashes(uint32_t startBlockIndex, uint32_t maxCount) const;

  std::error_code validateBlock(const CachedBlock& block, IBlockchainCache* cache, uint64_t& minerReward);

  uint64_t getAdjustedTime() const;
  void updateMainChainSet();
  IBlockchainCache* findSegmentContainingBlock(const Crypto::Hash& blockHash, bool* isMainChain = nullptr) const;
  IBlockchainCache* findSegmentContainingBlock(uint32_t blockHeight, bool* isMainChain = nullptr) const;
  IBlockchainCache* findMainChainSegmentContainingBlock(const Crypto::Hash& blockHash) const;
  IBlockchainCache* findAlternativeSegmentContainingBlock(const Crypto::Hash& blockHash) const;

  IBlockchainCache* findMainChainSegmentContainingBlock(uint32_t blockIndex) const;
  IBlockchainCache* findAlternativeSegmentContainingBlock(uint32_t blockIndex) const;

  IBlockchainCache* findSegmentContainingTransaction(const Crypto::Hash& transactionHash,
                                                     bool* isMainChain = nullptr) const;

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
  bool fillQueryBlockDetails(uint32_t fullOffset, uint32_t currentIndex, size_t maxItemsCount,
                             std::vector<BlockDetails>& entries) const;

  void getTransactionPoolDifference(const std::vector<Crypto::Hash>& knownHashes,
                                    std::vector<Crypto::Hash>& newTransactions,
                                    std::vector<Crypto::Hash>& deletedTransactions) const;

  BlockVersion getBlockVersionForIndex(uint32_t height) const;
  size_t calculateCumulativeBlocksizeLimit(uint32_t height) const;

  /*!
   * \brief fillBlockTemplate Fills a block template with transactions, trying to achieve the maximum reward
   * \param block The block currently generated, copy transaction hashes in here
   * \param height The index this block is mined for
   * \param fullRewardZone The maximum block before penalties are applied
   * \param maxCumulativeSize The maximum size of a block, exceeding this limit yields invalid blocks
   * \param transactionsSize Is set to the cumulative size of transactions added
   * \param fee Is set to the cumulative fees of transaction blobs included
   *
   * The implementation uses a greedy maximum knapsack approximation,
   *    see https://pubsonline.informs.org/doi/abs/10.1287/opre.5.2.266
   * .
   */
  void fillBlockTemplate(BlockTemplate& block, uint32_t index, size_t fullRewardZone, size_t maxCumulativeSize,
                         size_t& transactionsSize, uint64_t& fee) const;
  void deleteAlternativeChains();
  void deleteLeaf(size_t leafIndex);
  void mergeMainChainSegments();
  void mergeSegments(IBlockchainCache* acceptingSegment, IBlockchainCache* segment);
  TransactionDetails getTransactionDetails(const Crypto::Hash& transactionHash, IBlockchainCache* segment,
                                           bool foundInPool) const;
  void notifyOnSuccess(error::AddBlockErrorCode opResult, uint32_t previousBlockIndex, const CachedBlock& cachedBlock,
                       const IBlockchainCache& cache);

  void transactionPoolCleaningProcedure();
  void updateBlockMedianSize();
  bool isTransactionValidForPool(const CachedTransaction& cachedTransaction, TransactionValidatorState& validatorState);

  [[nodiscard]] bool initRootSegment();
  [[nodiscard]] bool importBlocksFromStorage();

  [[nodiscard]] bool importTransactionPool();
  [[nodiscard]] bool exportTransactionPool();

  void cutSegment(IBlockchainCache& segment, uint32_t startIndex);

  void switchMainChainStorage(uint32_t splitBlockIndex, IBlockchainCache& newChain);
};

}  // namespace CryptoNote
