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
#include "Logging/LoggerRef.h"
#include "CachedBlock.h"
#include "CryptoNoteBasic.h"

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

  std::string name() const;
  std::string ticker() const;
  std::string addressPrefix() const;

  std::string homepage() const;
  std::string description() const;
  std::string copyright() const;

  size_t maxBlockBlobSize() const { return m_maxBlockBlobSize; }
  size_t maxTxSize(BlockVersion blockVersion) const;
  uint8_t maxTxVersion() const;
  uint8_t minTxVersion() const;
  uint64_t publicAddressBase58Prefix() const { return m_publicAddressBase58Prefix; }
  uint32_t minedMoneyUnlockWindow() const { return m_minedMoneyUnlockWindow; }

  uint32_t timestampCheckWindow(BlockVersion version) const;
  uint64_t blockFutureTimeLimit(BlockVersion version) const;

  uint32_t blockTimeTarget() const;

  uint64_t moneySupply() const { return m_moneySupply; }
  unsigned int emissionSpeedFactor() const { return m_emissionSpeedFactor; }
  uint64_t genesisBlockReward() const { return m_genesisBlockReward; }

  size_t rewardBlocksWindowByBlockVersion(BlockVersion blockVersion) const;
  uint64_t rewardCutOffByBlockVersion(BlockVersion blockVersion) const;
  size_t blockGrantedFullRewardZoneByBlockVersion(BlockVersion blockVersion) const;
  size_t minerTxBlobReservedSize() const { return m_minerTxBlobReservedSize; }

  size_t numberOfDecimalPlaces() const { return m_numberOfDecimalPlaces; }
  uint64_t coin() const;

  uint8_t requiredMixin(BlockVersion blockVersion) const;
  uint64_t requiredMixinUpgradeWindow(BlockVersion blockVersion) const;
  uint64_t requiredMixinThreshold(BlockVersion blockVersion) const;

  uint64_t minimumFee(BlockVersion version) const;
  uint64_t minimumFee(BlockVersion version, uint64_t canonicialBuckets) const;

  uint32_t upgradeHeight(BlockVersion version) const;

  uint64_t difficultyTarget() const { return m_difficultyTarget; }
  size_t difficultyBlocksCountByVersion(BlockVersion version) const;

  uint64_t maximumMergeMiningSize(BlockVersion version) const;
  uint64_t maximumCoinbaseSize(BlockVersion version) const;

  size_t maxBlockSizeInitial() const { return m_maxBlockSizeInitial; }
  uint64_t maxBlockSizeGrowthSpeedNumerator() const { return m_maxBlockSizeGrowthSpeedNumerator; }
  uint64_t maxBlockSizeGrowthSpeedDenominator() const { return m_maxBlockSizeGrowthSpeedDenominator; }

  uint64_t mempoolTxLiveTime() const { return m_mempoolTxLiveTime; }
  uint64_t mempoolTxFromAltBlockLiveTime() const { return m_mempoolTxFromAltBlockLiveTime; }
  uint64_t numberOfPeriodsToForgetTxDeletedFromPool() const { return m_numberOfPeriodsToForgetTxDeletedFromPool; }

  size_t fusionTxMaxSize(BlockVersion blockVersion) const;
  size_t fusionTxMinInputCount() const { return m_fusionTxMinInputCount; }
  size_t fusionTxMinInOutCountRatio() const { return m_fusionTxMinInOutCountRatio; }

  std::string blocksFileName() const;
  std::string blockIndexesFileName() const;
  std::string txPoolFileName() const;

  ::Xi::Config::Network::Type network() const { return m_network; }
  bool isMainNet() const { return m_network == ::Xi::Config::Network::Type::MainNet; }
  bool isTestNet() const { return !isMainNet() && m_network != ::Xi::Config::Network::MainNet; }

  const BlockTemplate& genesisBlock() const;
  const Crypto::Hash& genesisBlockHash() const;
  uint64_t genesisTimestamp() const;

  bool getBlockReward(BlockVersion blockVersion, size_t medianSize, size_t currentBlockSize,
                      uint64_t alreadyGeneratedCoins, uint64_t fee, uint64_t& reward, int64_t& emissionChange) const;
  size_t maxBlockCumulativeSize(uint64_t height) const;

  bool constructMinerTx(BlockVersion blockVersion, uint32_t height, size_t medianSize, uint64_t alreadyGeneratedCoins,
                        size_t currentBlockSize, uint64_t fee, const AccountPublicAddress& minerAddress,
                        Transaction& tx, const BinaryArray& extraNonce, size_t maxOuts) const;

  // -------------------------------------------- Static Reward -------------------------------------------------------
  bool isStaticRewardEnabledForBlockVersion(BlockVersion blockVersion) const;
  uint64_t staticRewardAmountForBlockVersion(BlockVersion blockVersion) const;
  std::string staticRewardAddressForBlockVersion(BlockVersion blockVersion) const;
  Xi::Result<boost::optional<Transaction> > constructStaticRewardTx(const Crypto::Hash& previousBlockHash,
                                                                    BlockVersion blockVersion,
                                                                    uint32_t blockIndex) const;
  Xi::Result<boost::optional<Transaction> > constructStaticRewardTx(const CachedBlock& block) const;
  Xi::Result<boost::optional<Transaction> > constructStaticRewardTx(const BlockTemplate& block) const;
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

  std::string formatAmount(uint64_t amount) const;
  std::string formatAmount(int64_t amount) const;
  [[nodiscard]] bool parseAmount(const std::string& str, uint64_t& amount) const;

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
  size_t m_maxBlockBlobSize;
  size_t m_maxTxSize;
  uint64_t m_publicAddressBase58Prefix;
  uint32_t m_minedMoneyUnlockWindow;

  size_t m_timestampCheckWindow;
  uint64_t m_blockFutureTimeLimit;

  uint64_t m_moneySupply;
  unsigned int m_emissionSpeedFactor;
  uint64_t m_genesisBlockReward;

  size_t m_minerTxBlobReservedSize;

  size_t m_numberOfDecimalPlaces;
  uint64_t m_coin;

  uint64_t m_mininumFee;

  uint64_t m_difficultyTarget;

  size_t m_maxBlockSizeInitial;
  uint64_t m_maxBlockSizeGrowthSpeedNumerator;
  uint64_t m_maxBlockSizeGrowthSpeedDenominator;

  uint64_t m_mempoolTxLiveTime;
  uint64_t m_mempoolTxFromAltBlockLiveTime;
  uint64_t m_numberOfPeriodsToForgetTxDeletedFromPool;

  size_t m_fusionTxMaxSize;
  size_t m_fusionTxMinInputCount;
  size_t m_fusionTxMinInOutCountRatio;

  std::string m_blocksFileName;
  std::string m_blockIndexesFileName;
  std::string m_txPoolFileName;

  static const std::vector<uint64_t> PRETTY_AMOUNTS;

  Xi::Config::Network::Type m_network;

  BlockTemplate m_genesisBlockTemplate;
  std::unique_ptr<CachedBlock> m_cachedGenesisBlock;

  Xi::ByteVector m_staticRewardSalt{};

  Logging::LoggerRef logger;

  friend class CurrencyBuilder;
};

class CurrencyBuilder : boost::noncopyable {
 public:
  CurrencyBuilder(Logging::ILogger& log);

  Currency currency() {
    if (!m_currency.init()) {
      throw std::runtime_error("Failed to initialize currency object");
    }

    return std::move(m_currency);
  }

  Transaction generateGenesisTransaction();
  Transaction generateGenesisTransaction(const std::vector<AccountPublicAddress>& targets);
  CurrencyBuilder& maxBlockBlobSize(size_t val) {
    m_currency.m_maxBlockBlobSize = val;
    return *this;
  }
  CurrencyBuilder& maxTxSize(size_t val) {
    m_currency.m_maxTxSize = val;
    return *this;
  }
  CurrencyBuilder& publicAddressBase58Prefix(uint64_t val) {
    m_currency.m_publicAddressBase58Prefix = val;
    return *this;
  }
  CurrencyBuilder& minedMoneyUnlockWindow(uint32_t val) {
    m_currency.m_minedMoneyUnlockWindow = val;
    return *this;
  }

  CurrencyBuilder& timestampCheckWindow(size_t val) {
    m_currency.m_timestampCheckWindow = val;
    return *this;
  }
  CurrencyBuilder& blockFutureTimeLimit(uint64_t val) {
    m_currency.m_blockFutureTimeLimit = val;
    return *this;
  }

  CurrencyBuilder& moneySupply(uint64_t val) {
    m_currency.m_moneySupply = val;
    return *this;
  }
  CurrencyBuilder& emissionSpeedFactor(unsigned int val);
  CurrencyBuilder& genesisBlockReward(uint64_t val) {
    m_currency.m_genesisBlockReward = val;
    return *this;
  }
  CurrencyBuilder& minerTxBlobReservedSize(size_t val) {
    m_currency.m_minerTxBlobReservedSize = val;
    return *this;
  }

  CurrencyBuilder& numberOfDecimalPlaces(size_t val);

  CurrencyBuilder& mininumFee(uint64_t val) {
    m_currency.m_mininumFee = val;
    return *this;
  }

  CurrencyBuilder& difficultyTarget(uint64_t val) {
    m_currency.m_difficultyTarget = val;
    return *this;
  }

  CurrencyBuilder& maxBlockSizeInitial(size_t val) {
    m_currency.m_maxBlockSizeInitial = val;
    return *this;
  }
  CurrencyBuilder& maxBlockSizeGrowthSpeedNumerator(uint64_t val) {
    m_currency.m_maxBlockSizeGrowthSpeedNumerator = val;
    return *this;
  }
  CurrencyBuilder& maxBlockSizeGrowthSpeedDenominator(uint64_t val) {
    m_currency.m_maxBlockSizeGrowthSpeedDenominator = val;
    return *this;
  }

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

  CurrencyBuilder& fusionTxMaxSize(size_t val) {
    m_currency.m_fusionTxMaxSize = val;
    return *this;
  }
  CurrencyBuilder& fusionTxMinInputCount(size_t val) {
    m_currency.m_fusionTxMinInputCount = val;
    return *this;
  }
  CurrencyBuilder& fusionTxMinInOutCountRatio(size_t val) {
    m_currency.m_fusionTxMinInOutCountRatio = val;
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

  CurrencyBuilder& network(Xi::Config::Network::Type network) {
    m_currency.m_network = network;
    return *this;
  }

 private:
  Currency m_currency;
};

}  // namespace CryptoNote
