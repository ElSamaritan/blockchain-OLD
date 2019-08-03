// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <boost/utility.hpp>
#include <Xi/Global.hh>
#include <Xi/Exceptional.hpp>
#include <Xi/Result.h>
#include <Xi/Config.h>
#include <Xi/ProofOfWork/ProofOfWork.hpp>
#include "Logging/LoggerRef.h"

#include "CryptoNoteCore/CryptoNoteBasic.h"
#include "CryptoNoteCore/CachedBlock.h"
#include "CryptoNoteCore/Checkpoints.h"
#include "CryptoNoteCore/IAmountFormatter.hpp"
#include "CryptoNoteCore/Difficulty.h"
#include "CryptoNoteCore/UpgradeManager.h"

namespace CryptoNote {

XI_DECLARE_EXCEPTIONAL_CATEGORY(Parse)
XI_DECLARE_EXCEPTIONAL_INSTANCE(AccountPublicAddressParse, "Unable to parse account public address", Parse)
XI_DECLARE_EXCEPTIONAL_INSTANCE(TransactionParse, "Unable to parse a given transaction blob", Parse)

XI_DECLARE_EXCEPTIONAL_CATEGORY(Reward)
XI_DECLARE_EXCEPTIONAL_INSTANCE(RewardMissmatch, "actual and expected reward missmatch", Reward)

class AccountBase;

class Currency {
 public:
  XI_DELETE_COPY(Currency);

  const Xi::Config::General::Configuration& general() const;
  const Xi::Config::Coin::Configuration& coin() const;
  const Xi::Config::Network::Configuration& network() const;
  const Xi::Config::Transaction::Configuration& transaction(BlockVersion version) const;
  const Xi::Config::MinerReward::Configuration& minerReward(BlockVersion version) const;
  const Xi::Config::StaticReward::Configuration* staticReward(BlockVersion version) const;
  const Xi::Config::Time::Configuration& time(BlockVersion version) const;
  const Xi::Config::Limits::Configuration& limit(BlockVersion version) const;
  const Xi::Config::Difficulty::Configuration& difficulty(BlockVersion version) const;
  const Xi::Config::MergeMining::Configuration* mergeMining(BlockVersion version) const;
  const IUpgradeManager& upgradeManager() const;

  const Difficulty::IDifficultyAlgorithm& difficultyAlgorithm(BlockVersion version) const;
  const Xi::ProofOfWork::IAlgorithm& proofOfWorkAlgorithm(BlockVersion version) const;

  std::string networkUniqueName() const;

  size_t maxTxSize(BlockVersion blockVersion) const;
  uint32_t minedMoneyUnlockWindow() const;

  const IAmountFormatter& amountFormatter() const;

  unsigned int emissionSpeedFactor() const;

  size_t rewardBlocksWindowByBlockVersion(BlockVersion blockVersion) const;
  uint64_t rewardCutOffByBlockVersion(BlockVersion blockVersion) const;
  size_t blockGrantedFullRewardZoneByBlockVersion(BlockVersion blockVersion) const;
  size_t minerTxBlobReservedSize(BlockVersion blockVersion) const;

  uint8_t mixinLowerBound(BlockVersion blockVersion) const;
  uint8_t mixinUpperBound(BlockVersion blockVersion) const;

  [[nodiscard]] bool isTransactionVersionSupported(BlockVersion blockVersion, uint16_t transferVersion) const;

  uint64_t minimumFee(BlockVersion version) const;
  uint64_t minimumFee(BlockVersion version, uint64_t canonicialBuckets) const;

  size_t difficultyBlocksCountByVersion(BlockVersion version) const;

  uint64_t maximumMergeMiningSize(BlockVersion version) const;
  uint64_t maximumCoinbaseSize(BlockVersion version) const;

  uint64_t maxBlockSize(BlockVersion version, uint32_t index) const;
  uint64_t maxBlockSizeInitial(BlockVersion version) const;
  uint64_t maxBlockSizeGrowthSpeedNumerator(BlockVersion version) const;
  uint64_t maxBlockSizeGrowthSpeedDenominator(BlockVersion version) const;

  uint64_t mempoolTxLiveTime() const {
    return m_mempoolTxLiveTime;
  }
  uint64_t mempoolTxFromAltBlockLiveTime() const {
    return m_mempoolTxFromAltBlockLiveTime;
  }
  uint64_t numberOfPeriodsToForgetTxDeletedFromPool() const {
    return m_numberOfPeriodsToForgetTxDeletedFromPool;
  }

  size_t fusionTxMaxSize(BlockVersion blockVersion) const;
  size_t fusionTxMinInputCount(BlockVersion blockVersion) const;
  size_t fusionTxMinInOutCountRatio(BlockVersion blockVersion) const;

  std::string blocksFileName() const;
  std::string blockIndexesFileName() const;
  std::string txPoolFileName() const;

  const std::vector<CheckpointData>& integratedCheckpoints() const;

  const BlockTemplate& genesisBlock() const;
  const Crypto::Hash& genesisBlockHash() const;

  bool getBlockBaseReward(uint32_t blockIndex, BlockVersion blockVersion, uint64_t circulating, uint64_t& reward) const;

  bool getBlockReward(uint32_t blockIndex, BlockVersion blockVersion, size_t medianSize, size_t currentBlockSize,
                      uint64_t alreadyGeneratedCoins, uint64_t fee, uint64_t& reward, int64_t& emissionChange) const;

  bool constructMinerTx(BlockVersion blockVersion, uint32_t height, size_t medianSize, uint64_t alreadyGeneratedCoins,
                        size_t currentBlockSize, uint64_t fee, const AccountPublicAddress& minerAddress,
                        Transaction& tx, size_t maxOuts) const;

  // -------------------------------------------- Static Reward -------------------------------------------------------
  bool isStaticRewardEnabledForBlockVersion(BlockVersion blockVersion) const;
  uint64_t staticRewardAmountForBlockVersion(BlockVersion blockVersion) const;
  std::string staticRewardAddressForBlockVersion(BlockVersion blockVersion) const;
  Xi::Result<boost::optional<Transaction>> constructStaticRewardTx(const Crypto::Hash& previousBlockHash,
                                                                   BlockVersion blockVersion,
                                                                   uint32_t blockIndex) const;
  Xi::Result<boost::optional<Transaction>> constructStaticRewardTx(const CachedBlock& block) const;
  Xi::Result<boost::optional<Transaction>> constructStaticRewardTx(const BlockTemplate& block) const;
  // -------------------------------------------- Static Reward -------------------------------------------------------

  // ----------------------------------------------- Locking ----------------------------------------------------------
  bool isLockedBasedOnTimestamp(uint64_t unlock) const;
  bool isLockedBasedOnBlockIndex(uint64_t unlock) const;
  bool isUnlockSatisfied(uint64_t unlock, uint32_t blockIndex, uint64_t timestamp) const;
  uint32_t estimateUnlockIndex(uint64_t unlock) const;
  uint64_t unlockLimit(BlockVersion version) const;
  // ----------------------------------------------- Locking ----------------------------------------------------------

  bool isFusionTransaction(const Transaction& transaction, BlockVersion version) const;
  bool isFusionTransaction(const Transaction& transaction, size_t size, BlockVersion version) const;
  bool isFusionTransaction(const std::vector<uint64_t>& inputsAmounts, const std::vector<uint64_t>& outputsAmounts,
                           size_t size, BlockVersion version) const;
  bool isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold) const;
  bool isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold, uint8_t& amountPowerOfTen) const;

  std::string accountAddressAsString(const AccountBase& account) const;
  std::string accountAddressAsString(const AccountPublicAddress& accountPublicAddress) const;
  [[nodiscard]] bool parseAccountAddressString(const std::string& str, AccountPublicAddress& addr) const;

  uint64_t nextDifficulty(BlockVersion version, uint32_t blockIndex, std::vector<uint64_t> timestamps,
                          std::vector<uint64_t> cumulativeDifficulties) const;

  bool checkProofOfWork(const CachedBlock& block, uint64_t currentDifficulty) const;

  Currency(Currency&& currency);

  size_t getApproximateMaximumInputCount(size_t transactionSize, size_t outputCount, size_t mixinCount) const;

 private:
  Currency(Logging::ILogger& log);

  bool init();
  bool generateGenesisBlock();

 private:
  uint64_t m_mempoolTxLiveTime;
  uint64_t m_mempoolTxFromAltBlockLiveTime;
  uint64_t m_numberOfPeriodsToForgetTxDeletedFromPool;

  std::string m_blocksFileName;
  std::string m_blockIndexesFileName;
  std::string m_txPoolFileName;

  BlockTemplate m_genesisBlockTemplate;
  std::unique_ptr<CachedBlock> m_cachedGenesisBlock;

  Xi::ByteVector m_staticRewardSalt{};

  Logging::LoggerRef logger;

  std::shared_ptr<IAmountFormatter> m_amountFormatter;
  std::vector<CheckpointData> m_integratedCheckpointData;
  Xi::Config::General::Configuration m_general;
  Xi::Config::Coin::Configuration m_coin;
  Xi::Config::StaticReward::Container m_staticReward;
  Xi::Config::Network::Configuration m_network;
  Xi::Config::Transaction::Container m_transaction;
  Xi::Config::Time::Container m_time;
  Xi::Config::Limits::Container m_limit;
  Xi::Config::MinerReward::Container m_minerReward;
  Xi::Config::Difficulty::Container m_difficulty;
  Xi::Config::MergeMining::Container m_mergeMining;
  std::shared_ptr<IUpgradeManager> m_upgradeManager;

  std::vector<std::shared_ptr<Difficulty::IDifficultyAlgorithm>> m_difficultyAlgorithms;
  std::vector<std::shared_ptr<Xi::ProofOfWork::IAlgorithm>> m_proofOfWorkAlgorithms;

  friend class CurrencyBuilder;
};

class CurrencyBuilder {
 public:
  CurrencyBuilder(Logging::ILogger& log);
  XI_DELETE_COPY(CurrencyBuilder);
  XI_DEFAULT_MOVE(CurrencyBuilder);

  Currency currency();

  Transaction generateGenesisTransaction() const;
  Transaction generateGenesisTransaction(const std::vector<AccountPublicAddress>& targets) const;

  CurrencyBuilder& mempoolTxLiveTime(uint64_t val) {
    m_currency.m_mempoolTxLiveTime = val;
    return *this;
  }
  CurrencyBuilder& mempoolTxFromAltBlockLiveTime(uint64_t val) {
    m_currency.m_mempoolTxFromAltBlockLiveTime = val;
    return *this;
  }
  CurrencyBuilder& numberOfPeriodsToForgetTxDeletedFromPool(uint64_t val) {
    m_currency.m_numberOfPeriodsToForgetTxDeletedFromPool = val;
    return *this;
  }

  CurrencyBuilder& blocksFileName(const std::string& val) {
    m_currency.m_blocksFileName = val;
    return *this;
  }
  CurrencyBuilder& blockIndexesFileName(const std::string& val) {
    m_currency.m_blockIndexesFileName = val;
    return *this;
  }
  CurrencyBuilder& txPoolFileName(const std::string& val) {
    m_currency.m_txPoolFileName = val;
    return *this;
  }

  CurrencyBuilder& configuration(const Xi::Config::Configuration& config);
  CurrencyBuilder& network(std::string netId);
  CurrencyBuilder& networkDir(std::string dir);

  const Currency& immediateState() const;

 private:
  Currency m_currency;
  std::string m_networkDir;
};

}  // namespace CryptoNote
