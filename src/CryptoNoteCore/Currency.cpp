// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "Currency.h"

#include <cctype>
#include <iterator>

#include <Xi/ExternalIncludePush.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>

#include "../Common/Base58.h"
#include "../Common/int-util.h"
#include "../Common/StringTools.h"

#include "Serialization/SerializationTools.h"
#include "Account.h"
#include "CheckDifficulty.h"
#include "CryptoNoteBasicImpl.h"
#include "CryptoNoteFormatUtils.h"
#include "CryptoNoteTools.h"
#include "Difficulty.h"
#include "Transactions/TransactionExtra.h"
#include "Transactions/CachedTransaction.h"
#include "UpgradeDetector.h"

#include "CryptoNoteCore/AmountFormatter.hpp"

#undef ERROR

using namespace Logging;
using namespace Common;

namespace CryptoNote {

bool Currency::init() {
  m_amountFormatter = std::make_shared<AmountFormatter>(coin().decimals(), coin().ticker());

  if (m_staticRewardSalt.empty()) {
    std::string saltPrefix{};
    {
      std::stringstream saltBuilder{};
      saltBuilder << coin().name() << "-StaticReward-";
      saltPrefix = saltBuilder.str();
    }
    saltPrefix += toString(network().type());
    std::transform(saltPrefix.begin(), saltPrefix.end(), std::back_inserter(m_staticRewardSalt),
                   [](const char i) { return static_cast<Xi::Byte>(i); });
  }

  m_difficultyAlgorithms.resize(upgradeManager().maximumVersion().native());
  m_proofOfWorkAlgorithms.resize(upgradeManager().maximumVersion().native());

  for (BlockVersion::value_type i = BlockVersion::Genesis.native(); i <= upgradeManager().maximumVersion().native();
       ++i) {
    const BlockVersion current{i};
    m_difficultyAlgorithms[i - 1] = Difficulty::makeDifficultyAlgorithm(difficulty(current).algorithm());
    if (m_difficultyAlgorithms[i - 1].get() == nullptr) {
      logger(Fatal) << "Difficulty algorithm unknown: " << difficulty(current).algorithm();
      XI_RETURN_EC(false);
    }
    m_proofOfWorkAlgorithms[i - 1] = Xi::ProofOfWork::makeAlgorithm(difficulty(current).proofOfWorkAlgorithm());
    if (m_proofOfWorkAlgorithms[i - 1].get() == nullptr) {
      logger(Fatal) << "Proof of Work algorithm unknown: " << difficulty(current).proofOfWorkAlgorithm();
      XI_RETURN_EC(false);
    }
  }

  if (!generateGenesisBlock()) {
    logger(Error) << "Failed to generate genesis block";
    return false;
  }

  try {
    m_cachedGenesisBlock->getBlockHash();
  } catch (std::exception& e) {
    logger(Error) << "Failed to get genesis block hash: " << e.what();
    return false;
  }

  return true;
}

bool Currency::generateGenesisBlock() {
  if (m_cachedGenesisBlock.get() != nullptr) {
    return true;
  }
  m_genesisBlockTemplate = boost::value_initialized<BlockTemplate>();

  std::string genesisCoinbaseTxHex = coin().genesisTransaction();
  BinaryArray minerTxBlob;

  try {
    minerTxBlob = fromHex(genesisCoinbaseTxHex);
    m_genesisBlockTemplate.baseTransaction = fromBinaryArray<Transaction>(minerTxBlob);
  } catch (std::exception& e) {
    logger(Error) << "failed to parse coinbase tx from hard coded blob: " << e.what();
    return false;
  }

  m_genesisBlockTemplate.version = BlockVersion::Genesis;
  m_genesisBlockTemplate.timestamp = coin().startTimestamp();
  m_genesisBlockTemplate.nonce.fill(0);
  m_genesisBlockTemplate.previousBlockHash.nullify();
  if (!network().isMainNet()) {
    m_genesisBlockTemplate.nonce.setAsInteger(static_cast<BlockNonce::integer_type>(network().type()) * 0xFF);
  }

  if (!fromBinaryArray(m_genesisBlockTemplate.baseTransaction, minerTxBlob)) {
    logger(Error) << "Unable to parse hex encoded genesis coinbase transaction.";
    return false;
  }

  if (isStaticRewardEnabledForBlockVersion(m_genesisBlockTemplate.version)) {
    auto staticReward =
        constructStaticRewardTx(m_genesisBlockTemplate.previousBlockHash, m_genesisBlockTemplate.version, 0);
    if (staticReward.isError()) {
      logger(Error) << "Static reward construction failed: " << staticReward.error().message();
      return false;
    } else if (!staticReward.value().has_value()) {
      logger(Error) << "Expected static reward but none given.";
      return false;
    }
    const CachedTransaction cStaticReward{*staticReward.takeOrThrow()};
    const auto txHash = cStaticReward.getTransactionHash();
    m_genesisBlockTemplate.staticRewardHash = Xi::Crypto::Hash::Crc::Hash16::Null;
    compute(txHash.span(), *m_genesisBlockTemplate.staticRewardHash);
  } else {
    m_genesisBlockTemplate.staticRewardHash = std::nullopt;
  }

  m_genesisBlockTemplate.transactionHashes = {};

  do {
    m_genesisBlockTemplate.nonce.advance(1);
    m_cachedGenesisBlock.reset(new CachedBlock(m_genesisBlockTemplate));
  } while (!checkProofOfWork(*m_cachedGenesisBlock, 1));

  return true;
}

size_t Currency::difficultyBlocksCountByVersion(BlockVersion version) const {
  return difficulty(version).windowSize() + 1;
}

uint64_t Currency::maximumMergeMiningSize(BlockVersion version) const {
  const auto mm = mergeMining(version);
  if (mm == nullptr) {
    return 0;
  } else {
    return mm->maximum();
  }
}

uint64_t Currency::maximumCoinbaseSize(BlockVersion version) const {
  return minerReward(version).reservedSize();
}

uint64_t Currency::maxBlockSize(BlockVersion version, uint32_t index) const {
  auto scaled = maxBlockSizeInitial(version) +
                (index * maxBlockSizeGrowthSpeedNumerator(version)) / maxBlockSizeGrowthSpeedDenominator(version);
  return std::min(scaled, limit(version).maximum());
}

uint64_t Currency::maxBlockSizeInitial(BlockVersion version) const {
  return limit(version).initial();
}

uint64_t Currency::maxBlockSizeGrowthSpeedNumerator(BlockVersion version) const {
  return limit(version).increaseRate();
}

uint64_t Currency::maxBlockSizeGrowthSpeedDenominator(BlockVersion version) const {
  return limit(version).windowSize();
}

size_t Currency::fusionTxMaxSize(BlockVersion blockVersion) const {
  const uint64_t rewardZone = blockGrantedFullRewardZoneByBlockVersion(blockVersion);
  const auto maxSize = std::min(rewardZone, m_transaction(blockVersion)->fusion().maximumSize());
  const auto reservedSize = minerTxBlobReservedSize(blockVersion);
  assert(maxSize > reservedSize);
  return maxSize - reservedSize;
}

size_t Currency::fusionTxMinInputCount(BlockVersion blockVersion) const {
  return m_transaction(blockVersion)->fusion().minimumInputCount();
}

size_t Currency::fusionTxMinInOutCountRatio(BlockVersion blockVersion) const {
  return m_transaction(blockVersion)->fusion().ratioLimit();
}

const Xi::Config::General::Configuration& Currency::general() const {
  return m_general;
}

const Xi::Config::Coin::Configuration& Currency::coin() const {
  return m_coin;
}

const Xi::Config::Network::Configuration& Currency::network() const {
  return m_network;
}

const Xi::Config::Transaction::Configuration& Currency::transaction(BlockVersion version) const {
  return *m_transaction(version);
}

const Xi::Config::MinerReward::Configuration& Currency::minerReward(BlockVersion version) const {
  return *m_minerReward(version);
}

const Xi::Config::StaticReward::Configuration* Currency::staticReward(BlockVersion version) const {
  return m_staticReward(version);
}

const Xi::Config::Time::Configuration& Currency::time(BlockVersion version) const {
  return *m_time(version);
}

const Xi::Config::Limits::Configuration& Currency::limit(BlockVersion version) const {
  return *m_limit(version);
}

const Xi::Config::Difficulty::Configuration& Currency::difficulty(BlockVersion version) const {
  return *m_difficulty(version);
}

const Xi::Config::MergeMining::Configuration* Currency::mergeMining(BlockVersion version) const {
  return m_mergeMining(version);
}

const IUpgradeManager& Currency::upgradeManager() const {
  return *m_upgradeManager;
}

const Difficulty::IDifficultyAlgorithm& Currency::difficultyAlgorithm(BlockVersion version) const {
  return *m_difficultyAlgorithms[version.native() - 1];
}

const Xi::ProofOfWork::IAlgorithm& Currency::proofOfWorkAlgorithm(BlockVersion version) const {
  return *m_proofOfWorkAlgorithms[version.native() - 1];
}

std::string Currency::networkUniqueName() const {
  if (!network().isMainNet()) {
    return coin().name() + "." + toString(network().type());
  } else {
    return coin().name();
  }
}

size_t Currency::maxTxSize(BlockVersion blockVersion) const {
  const uint64_t rewardZone = blockGrantedFullRewardZoneByBlockVersion(blockVersion);
  const auto maxSize = std::min(rewardZone, m_transaction(blockVersion)->transfer().maximumSize());
  const auto reservedSize = minerTxBlobReservedSize(blockVersion);
  assert(maxSize > reservedSize);
  return maxSize - reservedSize;
}

uint32_t Currency::minedMoneyUnlockWindow() const {
  return coin().rewardUnlockTime() / coin().blockTime();
}

const IAmountFormatter& Currency::amountFormatter() const {
  return *m_amountFormatter;
}

unsigned int Currency::emissionSpeedFactor() const {
  return coin().emissionSpeed();
}

size_t Currency::rewardBlocksWindowByBlockVersion(BlockVersion blockVersion) const {
  return minerReward(blockVersion).windowSize();
}

uint64_t Currency::rewardCutOffByBlockVersion(BlockVersion blockVersion) const {
  return minerReward(blockVersion).cuttOff();
}

uint8_t Currency::mixinLowerBound(BlockVersion blockVersion) const {
  return m_transaction(blockVersion)->mixin().minimum();
}

uint8_t Currency::mixinUpperBound(BlockVersion blockVersion) const {
  return m_transaction(blockVersion)->mixin().maximum();
}

bool Currency::isTransactionVersionSupported(BlockVersion blockVersion, uint16_t transferVersion) const {
  const auto& supportedVersions = m_transaction(blockVersion)->supportedVersions();
  return std::find(supportedVersions.begin(), supportedVersions.end(), transferVersion) != supportedVersions.end();
}

uint64_t Currency::minimumFee(BlockVersion version) const {
  return m_transaction(version)->transfer().minimumFee();
}

uint64_t Currency::minimumFee(BlockVersion version, uint64_t canonicialBuckets) const {
  const auto& transfer = m_transaction(version)->transfer();
  const auto minFee = transfer.minimumFee();
  XI_RETURN_SC_IF(canonicialBuckets <= transfer.freeBuckets(), minFee);
  canonicialBuckets -= transfer.freeBuckets();
  return minFee + ((canonicialBuckets * minFee * transfer.rateNominator()) / transfer.rateDenominator());
}

size_t Currency::blockGrantedFullRewardZoneByBlockVersion(BlockVersion blockVersion) const {
  return minerReward(blockVersion).zone();
}

size_t Currency::minerTxBlobReservedSize(BlockVersion blockVersion) const {
  return minerReward(blockVersion).reservedSize();
}

std::string Currency::blocksFileName() const {
  return m_blocksFileName;
}

std::string Currency::blockIndexesFileName() const {
  return m_blockIndexesFileName;
}

std::string Currency::txPoolFileName() const {
  return m_txPoolFileName;
}

const std::vector<CheckpointData>& Currency::integratedCheckpoints() const {
  return m_integratedCheckpointData;
}

const BlockTemplate& Currency::genesisBlock() const {
  if (m_cachedGenesisBlock.get() == nullptr) {
    const_cast<Currency*>(this)->generateGenesisBlock();
  }
  return m_cachedGenesisBlock->getBlock();
}

const Crypto::Hash& Currency::genesisBlockHash() const {
  if (m_cachedGenesisBlock.get() == nullptr) {
    const_cast<Currency*>(this)->generateGenesisBlock();
  }
  return m_cachedGenesisBlock->getBlockHash();
}

bool Currency::getBlockReward(BlockVersion blockVersion, size_t medianSize, size_t currentBlockSize,
                              uint64_t alreadyGeneratedCoins, uint64_t fee, uint64_t& reward,
                              int64_t& emissionChange) const {
  assert(alreadyGeneratedCoins <= coin().totalSupply());
  assert(coin().emissionSpeed() > 0 && coin().emissionSpeed() <= 8 * sizeof(uint64_t));

  uint64_t baseReward = (coin().totalSupply() - alreadyGeneratedCoins) >> coin().emissionSpeed();
  if (alreadyGeneratedCoins == 0 && coin().premine() != 0) {
    XI_RETURN_EC_IF_NOT(blockVersion == BlockVersion::Genesis, false);
    baseReward = coin().premine();
    logger(Trace) << "Genesis block reward: " << baseReward;
  }

  size_t blockGrantedFullRewardZone = blockGrantedFullRewardZoneByBlockVersion(blockVersion);
  medianSize = std::max(medianSize, blockGrantedFullRewardZone);
  if (currentBlockSize > UINT64_C(2) * medianSize) {
    logger(Trace) << "Block cumulative size is too big: " << currentBlockSize << ", expected less than "
                  << 2 * medianSize;
    return false;
  }

  const uint64_t penalizedBaseReward = getPenalizedAmount(baseReward, medianSize, currentBlockSize);
  const uint64_t penalizedFee = getPenalizedAmount(fee, medianSize, currentBlockSize);
  const uint64_t staticReward = staticRewardAmountForBlockVersion(blockVersion);
  const uint64_t cuttedReward = cutDigitsFromAmount(penalizedBaseReward, rewardCutOffByBlockVersion(blockVersion));

  emissionChange = cuttedReward + staticReward - (fee - penalizedFee);
  reward = cuttedReward + penalizedFee;

  return true;
}

bool Currency::constructMinerTx(BlockVersion blockVersion, uint32_t index, size_t medianSize,
                                uint64_t alreadyGeneratedCoins, size_t currentBlockSize, uint64_t fee,
                                const AccountPublicAddress& minerAddress, Transaction& tx,
                                size_t maxOuts /* = 1*/) const {
  tx.nullify();

  KeyPair txkey = generateKeyPair();
  addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);

  BaseInput in;
  in.height = BlockHeight::fromIndex(index);

  uint64_t blockReward;
  int64_t emissionChange;
  if (!getBlockReward(blockVersion, medianSize, currentBlockSize, alreadyGeneratedCoins, fee, blockReward,
                      emissionChange)) {
    logger(Info) << "Block is too big";
    return false;
  }

  std::vector<uint64_t> outAmounts;
  decompose_amount_into_digits(blockReward, [&outAmounts](uint64_t a_chunk) { outAmounts.push_back(a_chunk); });

  if (!(1 <= maxOuts)) {
    logger(Error) << "max_out must be non-zero";
    return false;
  }
  while (maxOuts < outAmounts.size()) {
    outAmounts[outAmounts.size() - 2] += outAmounts.back();
    outAmounts.resize(outAmounts.size() - 1);
  }

  uint64_t summaryAmounts = 0;
  for (size_t no = 0; no < outAmounts.size(); no++) {
    Crypto::KeyDerivation derivation = boost::value_initialized<Crypto::KeyDerivation>();
    Crypto::PublicKey outEphemeralPubKey = boost::value_initialized<Crypto::PublicKey>();

    bool r = Crypto::generate_key_derivation(minerAddress.viewPublicKey, txkey.secretKey, derivation);

    if (!(r)) {
      logger(Error) << "while creating outs: failed to generate_key_derivation(" << minerAddress.viewPublicKey << ", "
                    << txkey.secretKey << ")";
      return false;
    }

    r = Crypto::derive_public_key(derivation, no, minerAddress.spendPublicKey, outEphemeralPubKey);

    if (!(r)) {
      logger(Error) << "while creating outs: failed to derive_public_key(" << derivation << ", " << no << ", "
                    << minerAddress.spendPublicKey << ")";
      return false;
    }

    KeyOutput tk;
    tk.key = outEphemeralPubKey;

    TransactionAmountOutput out;
    summaryAmounts += out.amount = outAmounts[no];
    out.target = tk;
    tx.outputs.push_back(out);
  }

  if (!(summaryAmounts == blockReward)) {
    logger(Error) << "Failed to construct miner tx, summaryAmounts = " << summaryAmounts
                  << " not equal blockReward = " << blockReward;
    return false;
  }

  tx.version = 1;
  tx.type = TransactionType::Reward;
  tx.features |= TransactionFeature::UniformUnlock;
  tx.unlockTime = index + minedMoneyUnlockWindow();
  tx.inputs.push_back(in);
  return true;
}

bool Currency::isStaticRewardEnabledForBlockVersion(BlockVersion blockVersion) const {
  const auto staticReward = m_staticReward(blockVersion);
  if (staticReward) {
    return staticReward->isEnabled();
  } else {
    return false;
  }
}

uint64_t Currency::staticRewardAmountForBlockVersion(BlockVersion blockVersion) const {
  const auto staticReward = m_staticReward(blockVersion);
  if (staticReward && staticReward->isEnabled()) {
    return staticReward->amount();
  } else {
    return 0;
  }
}

std::string Currency::staticRewardAddressForBlockVersion(BlockVersion blockVersion) const {
  const auto staticReward = m_staticReward(blockVersion);
  if (staticReward && staticReward->isEnabled()) {
    return staticReward->address();
  } else {
    return "";
  }
}

Xi::Result<boost::optional<Transaction>> Currency::constructStaticRewardTx(const Crypto::Hash& previousBlockHash,
                                                                           BlockVersion blockVersion,
                                                                           uint32_t blockIndex) const {
  XI_ERROR_TRY();
  const auto rewardAmount = staticRewardAmountForBlockVersion(blockVersion);
  const auto rewardAddress = staticRewardAddressForBlockVersion(blockVersion);
  if (rewardAddress.empty() || rewardAmount == 0) {
    if (!rewardAddress.empty()) {
      logger(Error) << "Static reward address set but amount is zero, consider deleting the static reward address.";
    } else if (rewardAmount > 0) {
      logger(Error)
          << "Static reward amount set but no address given, consider setting the static reward amount to zero.";
    } else {
      logger(Trace) << "Skipping static reward.";
    }
    return success<boost::optional<Transaction>>(boost::none);
  }

  logger(Trace) << "Generating static reward " << amountFormatter()(rewardAmount) << " for " << rewardAddress;

  AccountPublicAddress parsedRewardAddress = boost::value_initialized<AccountPublicAddress>();
  Xi::exceptional_if_not<AccountPublicAddressParseError>(parseAccountAddressString(rewardAddress, parsedRewardAddress));

  Transaction tx{};
  tx.nullify();

  auto saltedBuffer = m_staticRewardSalt;
  saltedBuffer.reserve(saltedBuffer.size() + Crypto::Hash::bytes());
  std::copy(previousBlockHash.begin(), previousBlockHash.end(), std::back_inserter(saltedBuffer));

  const KeyPair txkey = generateDeterministicKeyPair(saltedBuffer);
  addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);

  BaseInput in;
  in.height = BlockHeight::fromIndex(blockIndex);

  std::vector<uint64_t> outAmounts;
  decompose_amount_into_digits(rewardAmount, [&outAmounts](uint64_t a_chunk) { outAmounts.push_back(a_chunk); });

  const size_t maxOuts = 10;
  while (maxOuts < outAmounts.size()) {
    outAmounts[outAmounts.size() - 2] += outAmounts.back();
    outAmounts.resize(outAmounts.size() - 1);
  }

  uint64_t summaryAmounts = 0;
  for (size_t no = 0; no < outAmounts.size(); no++) {
    Crypto::KeyDerivation derivation = boost::value_initialized<Crypto::KeyDerivation>();
    Crypto::PublicKey outEphemeralPubKey = boost::value_initialized<Crypto::PublicKey>();

    if (!Crypto::generate_key_derivation(parsedRewardAddress.viewPublicKey, txkey.secretKey, derivation)) {
      Xi::exceptional<Crypto::KeyDerivationError>();
    }
    if (!Crypto::derive_public_key(derivation, no, parsedRewardAddress.spendPublicKey, outEphemeralPubKey)) {
      Xi::exceptional<Crypto::PublicKeyDerivationError>();
    }

    KeyOutput tk;
    tk.key = outEphemeralPubKey;

    TransactionAmountOutput out;
    summaryAmounts += out.amount = outAmounts[no];
    out.target = tk;
    tx.outputs.push_back(out);
  }

  if (!(summaryAmounts == rewardAmount)) {
    Xi::exceptional<RewardMissmatchError>();
  }

  tx.version = 1;
  tx.type = TransactionType::Reward;
  tx.unlockTime = 0;
  tx.inputs.push_back(in);

  return Xi::success<boost::optional<Transaction>>(std::move(tx));
  XI_ERROR_CATCH();
}

Xi::Result<boost::optional<Transaction>> Currency::constructStaticRewardTx(const CachedBlock& block) const {
  return constructStaticRewardTx(block.getBlock().previousBlockHash, block.getBlock().version, block.getBlockIndex());
}

Xi::Result<boost::optional<Transaction>> Currency::constructStaticRewardTx(const BlockTemplate& block) const {
  CachedBlock cblock{block};
  return constructStaticRewardTx(cblock);
}

bool Currency::isLockedBasedOnTimestamp(uint64_t unlock) const {
  return !isLockedBasedOnBlockIndex(unlock);
}
bool Currency::isLockedBasedOnBlockIndex(uint64_t unlock) const {
  return unlock <= BlockHeight::max().toIndex();
}

bool Currency::isUnlockSatisfied(uint64_t unlock, uint32_t blockIndex, uint64_t timestamp) const {
  if (isLockedBasedOnBlockIndex(unlock)) {
    return unlock <= blockIndex;
  } else {
    assert(isLockedBasedOnTimestamp(unlock));
    return unlock <= timestamp;
  }
}

uint32_t Currency::estimateUnlockIndex(uint64_t unlock) const {
  if (isLockedBasedOnBlockIndex(unlock)) {
    return static_cast<uint32_t>(unlock);
  } else {
    assert(isLockedBasedOnTimestamp(unlock));
    if (unlock < coin().startTimestamp()) {
      return 0;
    } else {
      auto estimate = (coin().startTimestamp() - unlock) / coin().blockTime() + 1;
      if (estimate <= BlockHeight::max().toIndex()) {
        return static_cast<uint32_t>(estimate);
      } else {
        return BlockHeight::max().toIndex();
      }
    }
  }
}

uint64_t Currency::unlockLimit(BlockVersion version) const {
  (void)version;
  return 500ULL * 365ULL * 24ULL * 60ULL * 60ULL;  // 500 years in seconds.
}

bool Currency::isFusionTransaction(const std::vector<uint64_t>& inputsAmounts,
                                   const std::vector<uint64_t>& outputsAmounts, size_t size,
                                   BlockVersion version) const {
  if (size > fusionTxMaxSize(version)) {
    return false;
  }

  if (inputsAmounts.size() < fusionTxMinInputCount(version)) {
    return false;
  }

  if (inputsAmounts.size() < outputsAmounts.size() * fusionTxMinInOutCountRatio(version)) {
    return false;
  }

  uint64_t inputAmount = 0;
  for (auto amount : inputsAmounts) {
    inputAmount += amount;
  }

  std::vector<uint64_t> expectedOutputsAmounts;
  expectedOutputsAmounts.reserve(outputsAmounts.size());
  decomposeAmount(inputAmount, expectedOutputsAmounts);
  std::sort(expectedOutputsAmounts.begin(), expectedOutputsAmounts.end());

  return expectedOutputsAmounts == outputsAmounts;
}

bool Currency::isFusionTransaction(const Transaction& transaction, size_t size, BlockVersion version) const {
  assert(transaction.binarySize() == size);

  std::vector<uint64_t> outputsAmounts;
  outputsAmounts.reserve(transaction.outputs.size());
  for (const TransactionOutput& output : transaction.outputs) {
    XI_RETURN_EC_IF_NOT(std::holds_alternative<TransactionAmountOutput>(output), false);
    const auto& amountOutput = std::get<TransactionAmountOutput>(output);
    outputsAmounts.push_back(amountOutput.amount);
  }

  return isFusionTransaction(getInputsAmounts(transaction), outputsAmounts, size, version);
}

bool Currency::isFusionTransaction(const Transaction& transaction, BlockVersion version) const {
  return isFusionTransaction(transaction, transaction.binarySize(), version);
}

bool Currency::isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold) const {
  uint8_t ignore;
  return isAmountApplicableInFusionTransactionInput(amount, threshold, ignore);
}

bool Currency::isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold,
                                                          uint8_t& amountPowerOfTen) const {
  if (amount >= threshold) {
    return false;
  }

  const auto decadeIndex = getCanoncialAmountDecimalPlace(amount);
  XI_RETURN_EC_IF(decadeIndex == std::numeric_limits<uint8_t>::max(), false);
  amountPowerOfTen = decadeIndex;
  return true;
}

std::string Currency::accountAddressAsString(const AccountBase& account) const {
  return getAccountAddressAsStr(coin().prefix().base58(), account.getAccountKeys().address);
}

std::string Currency::accountAddressAsString(const AccountPublicAddress& accountPublicAddress) const {
  return getAccountAddressAsStr(coin().prefix().base58(), accountPublicAddress);
}

bool Currency::parseAccountAddressString(const std::string& str, AccountPublicAddress& addr) const {
  uint64_t prefix;
  if (!CryptoNote::parseAccountAddressString(prefix, addr, str)) {
    return false;
  }

  if (prefix != coin().prefix().base58()) {
    logger(Debugging) << "Wrong address prefix: " << prefix << ", expected " << coin().prefix().base58();
    return false;
  }

  return true;
}

uint64_t Currency::nextDifficulty(BlockVersion version, uint32_t blockIndex, std::vector<uint64_t> timestamps,
                                  std::vector<uint64_t> cumulativeDifficulties) const {
  XI_UNUSED(blockIndex);
  const auto& diff = difficulty(version);
  if (timestamps.size() < diff.windowSize() + 1 || cumulativeDifficulties.size() < diff.windowSize() + 1) {
    return diff.initialDifficulty();
  } else {
    return difficultyAlgorithm(version)(std::move(timestamps), std::move(cumulativeDifficulties), diff.windowSize(),
                                        coin().blockTime());
  }
}

bool Currency::checkProofOfWork(const CachedBlock& block, uint64_t currentDiffic) const {
  const auto& powBlob = block.getProofOfWorkBlob();
  Crypto::Hash hash{};
  proofOfWorkAlgorithm(block.getBlock().version)(powBlob.span(), hash);
  return check_hash(hash, currentDiffic);
}

size_t Currency::getApproximateMaximumInputCount(size_t transactionSize, size_t outputCount, size_t mixinCount) const {
  const size_t KEY_IMAGE_SIZE = sizeof(Crypto::KeyImage);
  const size_t OUTPUT_KEY_SIZE = sizeof(decltype(KeyOutput::key));
  const size_t AMOUNT_SIZE = sizeof(uint64_t) + 2;                    // varint
  const size_t GLOBAL_INDEXES_VECTOR_SIZE_SIZE = sizeof(uint8_t);     // varint
  const size_t GLOBAL_INDEXES_INITIAL_VALUE_SIZE = sizeof(uint32_t);  // varint
  const size_t GLOBAL_INDEXES_DIFFERENCE_SIZE = sizeof(uint32_t);     // varint
  const size_t SIGNATURE_SIZE = sizeof(Crypto::Signature);
  const size_t EXTRA_TAG_SIZE = sizeof(uint8_t);
  const size_t INPUT_TAG_SIZE = sizeof(uint8_t);
  const size_t OUTPUT_TAG_SIZE = sizeof(uint8_t);
  const size_t PUBLIC_KEY_SIZE = sizeof(Crypto::PublicKey);
  const size_t TRANSACTION_VERSION_SIZE = sizeof(uint8_t);
  const size_t TRANSACTION_UNLOCK_TIME_SIZE = sizeof(uint64_t);

  const size_t outputsSize = outputCount * (OUTPUT_TAG_SIZE + OUTPUT_KEY_SIZE + AMOUNT_SIZE);
  const size_t headerSize = TRANSACTION_VERSION_SIZE + TRANSACTION_UNLOCK_TIME_SIZE + EXTRA_TAG_SIZE + PUBLIC_KEY_SIZE;
  const size_t inputSize = INPUT_TAG_SIZE + AMOUNT_SIZE + KEY_IMAGE_SIZE + SIGNATURE_SIZE +
                           GLOBAL_INDEXES_VECTOR_SIZE_SIZE + GLOBAL_INDEXES_INITIAL_VALUE_SIZE +
                           mixinCount * (GLOBAL_INDEXES_DIFFERENCE_SIZE + SIGNATURE_SIZE);

  return (transactionSize - headerSize - outputsSize) / inputSize;
}

Currency::Currency(ILogger& log) : logger(log, "currency"), m_upgradeManager{std::make_shared<UpgradeManager>()} {
}

Currency::Currency(Currency&& currency)
    : m_mempoolTxLiveTime(currency.m_mempoolTxLiveTime),
      m_mempoolTxFromAltBlockLiveTime(currency.m_mempoolTxFromAltBlockLiveTime),
      m_numberOfPeriodsToForgetTxDeletedFromPool(currency.m_numberOfPeriodsToForgetTxDeletedFromPool),
      m_blocksFileName(currency.m_blocksFileName),
      m_blockIndexesFileName(currency.m_blockIndexesFileName),
      m_txPoolFileName(currency.m_txPoolFileName),
      m_genesisBlockTemplate(currency.m_genesisBlockTemplate),
      m_cachedGenesisBlock(std::move(currency.m_cachedGenesisBlock)),
      m_staticRewardSalt{currency.m_staticRewardSalt},
      logger(currency.logger),

      m_amountFormatter{std::move(currency.m_amountFormatter)},
      m_integratedCheckpointData{std::move(currency.m_integratedCheckpointData)},
      m_general{std::move(currency.m_general)},
      m_coin{std::move(currency.m_coin)},
      m_staticReward{std::move(currency.m_staticReward)},
      m_network{std::move(currency.m_network)},
      m_transaction{std::move(currency.m_transaction)},
      m_time{std::move(currency.m_time)},
      m_limit{std::move(currency.m_limit)},
      m_minerReward{std::move(currency.m_minerReward)},
      m_difficulty{std::move(currency.m_difficulty)},
      m_mergeMining{std::move(currency.m_mergeMining)},
      m_upgradeManager{std::move(currency.m_upgradeManager)},

      m_difficultyAlgorithms{std::move(currency.m_difficultyAlgorithms)},
      m_proofOfWorkAlgorithms{std::move(currency.m_proofOfWorkAlgorithms)} {
}

CurrencyBuilder::CurrencyBuilder(Logging::ILogger& log) : m_currency(log), m_networkDir{"./config"} {
  mempoolTxLiveTime(static_cast<uint64_t>(Xi::Config::Limits::maximumTransactionLivetimeSpan().count()));
  mempoolTxFromAltBlockLiveTime(
      static_cast<uint64_t>(Xi::Config::Limits::maximumTransactionLivetimeSpanFromAltBlocks().count()));
  numberOfPeriodsToForgetTxDeletedFromPool(Xi::Config::Limits::minimumTransactionLivetimeSpansUntilDeletion());

  blocksFileName(Xi::Config::Database::blocksFilename());
  blockIndexesFileName(Xi::Config::Database::blockIndicesFilename());
  txPoolFileName(Xi::Config::Database::pooldataFilename());
}

Currency CurrencyBuilder::currency() {
  if (!m_currency.init()) {
    throw std::runtime_error("Failed to initialize currency object");
  }

  return std::move(m_currency);
}

Transaction CurrencyBuilder::generateGenesisTransaction() const {
  CryptoNote::Transaction tx;
  CryptoNote::AccountPublicAddress ac = boost::value_initialized<CryptoNote::AccountPublicAddress>();
  m_currency.constructMinerTx(BlockVersion::Genesis, 0, 0, 0, 0, 0, ac, tx, 20);  // zero fee in genesis
  return tx;
}
Transaction CurrencyBuilder::generateGenesisTransaction(const std::vector<AccountPublicAddress>& targets) const {
  assert(!targets.empty());

  CryptoNote::Transaction tx;
  tx.nullify();
  tx.version = 1;
  tx.type = TransactionType::Reward;
  tx.features |= TransactionFeature::UniformUnlock;
  tx.unlockTime = m_currency.minedMoneyUnlockWindow();
  KeyPair txkey = generateKeyPair();
  addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);
  BaseInput in;
  in.height = BlockHeight::Genesis;
  tx.inputs.push_back(in);
  uint64_t block_reward = m_currency.coin().premine();
  uint64_t target_amount = block_reward / targets.size();
  uint64_t first_target_amount = target_amount + block_reward % targets.size();
  size_t iOutputIndex = 0;
  for (size_t i = 0; i < targets.size(); ++i) {
    const auto amount = (i == 0) ? first_target_amount : target_amount;
    AmountVector canoncialDeomposition{};
    decomposeAmount(amount, canoncialDeomposition);

    for (const auto iAmount : canoncialDeomposition) {
      Crypto::KeyDerivation derivation = boost::value_initialized<Crypto::KeyDerivation>();
      Crypto::PublicKey outEphemeralPubKey = boost::value_initialized<Crypto::PublicKey>();
      if (!Crypto::generate_key_derivation(targets[i].viewPublicKey, txkey.secretKey, derivation)) {
        Xi::exceptional<Crypto::KeyDerivationError>();
      }
      if (!Crypto::derive_public_key(derivation, iOutputIndex++, targets[i].spendPublicKey, outEphemeralPubKey)) {
        Xi::exceptional<Crypto::PublicKeyDerivationError>();
      }
      KeyOutput tk;
      tk.key = outEphemeralPubKey;
      TransactionAmountOutput out;
      out.amount = iAmount;
      out.target = tk;
      tx.outputs.push_back(out);
    }
  }
  return tx;
}

CurrencyBuilder& CurrencyBuilder::configuration(const Xi::Config::Configuration& config) {
  m_currency.m_general = config.general();
  m_currency.m_coin = config.coin();
  m_currency.m_network = config.network();

  uint8_t i = 1;
  for (const auto& upgrade : config.upgrades()) {
    BlockVersion currentVersion{i++};
    if (upgrade.minerReward()) {
      m_currency.m_minerReward.insert(currentVersion, *upgrade.minerReward());
    }
    if (upgrade.staticReward()) {
      m_currency.m_staticReward.insert(currentVersion, *upgrade.staticReward());
    }
    if (upgrade.time()) {
      m_currency.m_time.insert(currentVersion, *upgrade.time());
    }
    if (upgrade.limit()) {
      m_currency.m_limit.insert(currentVersion, *upgrade.limit());
    }
    if (upgrade.transaction()) {
      m_currency.m_transaction.insert(currentVersion, *upgrade.transaction());
    }
    if (upgrade.difficulty()) {
      m_currency.m_difficulty.insert(currentVersion, *upgrade.difficulty());
    }
    if (upgrade.mergeMining()) {
      m_currency.m_mergeMining.insert(currentVersion, *upgrade.mergeMining());
    }

    m_currency.m_upgradeManager->addBlockVersion(currentVersion, upgrade.height().toIndex(), upgrade.fork());
  }
  return *this;
}

CurrencyBuilder& CurrencyBuilder::network(std::string netId) {
  if (const auto config = Xi::Config::Registry::searchByName(netId); config != nullptr) {
    m_currency.logger(Debugging) << "Using built in configuration for: " << netId;
    return configuration(*config);
  } else {
    auto rootDir = boost::filesystem::path{m_networkDir};
    auto filePath = boost::filesystem::path{netId};
    if (filePath.is_relative()) {
      if (!boost::filesystem::exists(rootDir / filePath)) {
        m_currency.logger(Info) << "Configuration not found in network directory: " << (rootDir / filePath).string();
      } else {
        filePath = rootDir / filePath;
        std::string sfilepath = filePath.string();
        if (!Xi::Config::Registry::addConfigJsonFile(netId, sfilepath)) {
          m_currency.logger(Fatal) << "Unable to load " << sfilepath;
          Xi::exceptional<Xi::NotFoundError>();
        }
        return configuration(*Xi::Config::Registry::searchByName(netId));
      }

      if (!boost::filesystem::exists(filePath)) {
        m_currency.logger(Error) << "Configuration not found in working directory: " << filePath.string();
        Xi::exceptional<Xi::NotFoundError>();
      } else {
        std::string sfilepath = filePath.string();
        if (!Xi::Config::Registry::addConfigJsonFile(netId, sfilepath)) {
          m_currency.logger(Fatal) << "Unable to load " << sfilepath;
          Xi::exceptional<Xi::NotFoundError>();
        }
        return configuration(*Xi::Config::Registry::searchByName(netId));
      }
    } else {
      if (!boost::filesystem::exists(filePath)) {
        m_currency.logger(Error) << "Configuration file not found: " << filePath.string();
        Xi::exceptional<Xi::NotFoundError>();
      } else {
        std::string sfilepath = filePath.string();
        if (!Xi::Config::Registry::addConfigJsonFile(netId, sfilepath)) {
          m_currency.logger(Fatal) << "Unable to load " << sfilepath;
          Xi::exceptional<Xi::NotFoundError>();
        }
        return configuration(*Xi::Config::Registry::searchByName(netId));
      }
    }
  }
}

CurrencyBuilder& CurrencyBuilder::networkDir(std::string dir) {
  m_networkDir = dir;
  return *this;
}

const Currency& CurrencyBuilder::immediateState() const {
  return this->m_currency;
}

}  // namespace CryptoNote
