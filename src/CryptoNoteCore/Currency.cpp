// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "Currency.h"

#include <cctype>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <Xi/Global.h>

#include "../Common/Base58.h"
#include "../Common/int-util.h"
#include "../Common/StringTools.h"

#include "Account.h"
#include "CheckDifficulty.h"
#include "CryptoNoteBasicImpl.h"
#include "CryptoNoteFormatUtils.h"
#include "CryptoNoteTools.h"
#include "Difficulty.h"
#include "Transactions/TransactionExtra.h"
#include "UpgradeDetector.h"

#undef ERROR

using namespace Logging;
using namespace Common;

namespace CryptoNote {

// clang-format off
const std::vector<uint64_t> Currency::PRETTY_AMOUNTS = {
  1, 2, 3, 4, 5, 6, 7, 8, 9,
  10, 20, 30, 40, 50, 60, 70, 80, 90,
  100, 200, 300, 400, 500, 600, 700, 800, 900,
  1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000,
  10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000,
  100000, 200000, 300000, 400000, 500000, 600000, 700000, 800000, 900000,
  1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000,
  10000000, 20000000, 30000000, 40000000, 50000000, 60000000, 70000000, 80000000, 90000000,
  100000000, 200000000, 300000000, 400000000, 500000000, 600000000, 700000000, 800000000, 900000000,
  1000000000, 2000000000, 3000000000, 4000000000, 5000000000, 6000000000, 7000000000, 8000000000, 9000000000,
  10000000000, 20000000000, 30000000000, 40000000000, 50000000000, 60000000000, 70000000000, 80000000000, 90000000000,
  100000000000, 200000000000, 300000000000, 400000000000, 500000000000, 600000000000, 700000000000, 800000000000, 900000000000,
  1000000000000, 2000000000000, 3000000000000, 4000000000000, 5000000000000, 6000000000000, 7000000000000, 8000000000000, 9000000000000,
  10000000000000, 20000000000000, 30000000000000, 40000000000000, 50000000000000, 60000000000000, 70000000000000, 80000000000000, 90000000000000,
  100000000000000, 200000000000000, 300000000000000, 400000000000000, 500000000000000, 600000000000000, 700000000000000, 800000000000000, 900000000000000,
  1000000000000000, 2000000000000000, 3000000000000000, 4000000000000000, 5000000000000000, 6000000000000000, 7000000000000000, 8000000000000000, 9000000000000000,
  10000000000000000, 20000000000000000, 30000000000000000, 40000000000000000, 50000000000000000, 60000000000000000, 70000000000000000, 80000000000000000, 90000000000000000,
  100000000000000000, 200000000000000000, 300000000000000000, 400000000000000000, 500000000000000000, 600000000000000000, 700000000000000000, 800000000000000000, 900000000000000000,
  1000000000000000000, 2000000000000000000, 3000000000000000000, 4000000000000000000, 5000000000000000000, 6000000000000000000, 7000000000000000000, 8000000000000000000, 9000000000000000000,
  10000000000000000000ull
};
// clang-format on

bool Currency::init() {
  if (!generateGenesisBlock()) {
    logger(ERROR) << "Failed to generate genesis block";
    return false;
  }

  try {
    m_cachedGenesisBlock->getBlockHash();
  } catch (std::exception& e) {
    logger(ERROR) << "Failed to get genesis block hash: " << e.what();
    return false;
  }

  return true;
}

bool Currency::generateGenesisBlock() {
  if (m_cachedGenesisBlock.get() != nullptr) {
    return true;
  }
  m_genesisBlockTemplate = boost::value_initialized<BlockTemplate>();

  std::string genesisCoinbaseTxHex = Xi::Config::Coin::genesisTransactionBlob(network());
  BinaryArray minerTxBlob;

  bool r = fromHex(genesisCoinbaseTxHex, minerTxBlob) &&
           fromBinaryArray(m_genesisBlockTemplate.baseTransaction, minerTxBlob);

  if (!r) {
    logger(ERROR) << "failed to parse coinbase tx from hard coded blob";
    return false;
  }

  m_genesisBlockTemplate.majorVersion = Xi::Config::BlockVersion::BlockVersionCheckpoint<0>::version();
  m_genesisBlockTemplate.minorVersion = Xi::Config::BlockVersion::expectedMinorVersion();
  m_genesisBlockTemplate.timestamp = Xi::Config::Coin::genesisTimestamp(network());
  m_genesisBlockTemplate.nonce = 0;
  m_genesisBlockTemplate.previousBlockHash.nullify();
  if (!isMainNet()) {
    m_genesisBlockTemplate.nonce += static_cast<uint8_t>(network()) * 0xFFFF;
  }

  if (!fromBinaryArray(m_genesisBlockTemplate.baseTransaction, minerTxBlob)) {
    Xi::exceptional<TransactionParseError>("Unable to parse hex encoded genesis coinbase transaction.");
  }

  m_genesisBlockTemplate.transactionHashes = {};

  do {
    m_cachedGenesisBlock.reset(new CachedBlock(m_genesisBlockTemplate));
    m_genesisBlockTemplate.nonce++;
  } while (!checkProofOfWork(*m_cachedGenesisBlock, 1));
  m_genesisBlockTemplate.nonce--;

  return true;
}

size_t Currency::difficultyBlocksCountByVersion(uint8_t version) const {
  return Xi::Config::Difficulty::windowSize(version) + 1;
}

size_t Currency::fusionTxMaxSize(uint8_t blockMajorVersion) const {
  const auto rewardZone = blockGrantedFullRewardZoneByBlockVersion(blockMajorVersion);
  const auto maxSize = std::min(rewardZone, m_fusionTxMaxSize);
  const auto reservedSize = minerTxBlobReservedSize();
  assert(maxSize > reservedSize);
  return maxSize - reservedSize;
}

size_t Currency::maxTxSize(uint8_t blockMajorVersion) const {
  const auto rewardZone = blockGrantedFullRewardZoneByBlockVersion(blockMajorVersion);
  const auto maxSize = std::min(rewardZone, m_maxTxSize);
  const auto reservedSize = minerTxBlobReservedSize();
  assert(maxSize > reservedSize);
  return maxSize - reservedSize;
}

uint8_t Currency::maxTxVersion() const { return Xi::Config::Transaction::maximumVersion(); }

uint8_t Currency::minTxVersion() const { return Xi::Config::Transaction::minimumVersion(); }

uint8_t Currency::majorBlockVersionForHeight(uint32_t blockHeight) const {
  return Xi::Config::BlockVersion::version(blockHeight);
}

uint32_t Currency::timestampCheckWindow(uint32_t blockHeight, uint8_t majorVersion) const {
  XI_UNUSED(blockHeight);
  return static_cast<uint32_t>(std::chrono::seconds{Xi::Config::Time::pastWindowSize(majorVersion)}.count());
}

uint64_t Currency::blockFutureTimeLimit(uint32_t blockHeight, uint8_t majorVersion) const {
  XI_UNUSED(blockHeight);
  return static_cast<uint64_t>(std::chrono::seconds{Xi::Config::Time::futureTimeLimit(majorVersion)}.count());
}

size_t Currency::rewardBlocksWindowByBlockVersion(uint8_t blockMajorVersion) const {
  return Xi::Config::MinerReward::window(blockMajorVersion);
}

uint8_t Currency::minimumMixin(uint8_t blockMajorVersion) const {
  return Xi::Config::Mixin::minimum(blockMajorVersion);
}

uint8_t Currency::maximumMixin(uint8_t blockMajorVersion) const {
  return Xi::Config::Mixin::maximum(blockMajorVersion);
}

size_t Currency::blockGrantedFullRewardZoneByBlockVersion(uint8_t blockMajorVersion) const {
  return Xi::Config::MinerReward::fullRewardZone(blockMajorVersion);
}

uint64_t Currency::defaultDustThresholdForMajorVersion(uint8_t blockMajorVersion) const {
  return Xi::Config::Dust::dust(blockMajorVersion);
}

uint64_t Currency::defaultDustThreshold(uint32_t height) const {
  return defaultDustThresholdForMajorVersion(Xi::Config::BlockVersion::version(height));
}

uint64_t Currency::defaultFusionDustThreshold(uint32_t height) const {
  return Xi::Config::Dust::fusionDust(Xi::Config::BlockVersion::version(height));
}

uint32_t Currency::upgradeHeight(uint8_t majorVersion) const {
  return Xi::Config::BlockVersion::upgradeHeight(majorVersion);
}

std::string Currency::blocksFileName() const { return m_blocksFileName + "." + Xi::to_lower(Xi::to_string(network())); }

std::string Currency::blockIndexesFileName() const {
  return m_blockIndexesFileName + "." + Xi::to_lower(Xi::to_string(network()));
}

std::string Currency::txPoolFileName() const { return m_txPoolFileName + "." + Xi::to_lower(Xi::to_string(network())); }

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

uint64_t Currency::genesisTimestamp() const { return Xi::Config::Coin::genesisTimestamp(network()); }

bool Currency::getBlockReward(uint8_t blockMajorVersion, size_t medianSize, size_t currentBlockSize,
                              uint64_t alreadyGeneratedCoins, uint64_t fee, uint64_t& reward,
                              int64_t& emissionChange) const {
  assert(alreadyGeneratedCoins <= m_moneySupply);
  assert(m_emissionSpeedFactor > 0 && m_emissionSpeedFactor <= 8 * sizeof(uint64_t));

  uint64_t baseReward = (m_moneySupply - alreadyGeneratedCoins) >> m_emissionSpeedFactor;
  if (alreadyGeneratedCoins == 0 && m_genesisBlockReward != 0) {
    baseReward = m_genesisBlockReward;
    logger(TRACE) << "Genesis block reward: " << baseReward;
  }

  size_t blockGrantedFullRewardZone = blockGrantedFullRewardZoneByBlockVersion(blockMajorVersion);
  medianSize = std::max(medianSize, blockGrantedFullRewardZone);
  if (currentBlockSize > UINT64_C(2) * medianSize) {
    logger(TRACE) << "Block cumulative size is too big: " << currentBlockSize << ", expected less than "
                  << 2 * medianSize;
    return false;
  }

  const uint64_t penalizedBaseReward = getPenalizedAmount(baseReward, medianSize, currentBlockSize);
  const uint64_t penalizedFee = blockMajorVersion >= Xi::Config::BlockVersion::BlockVersionCheckpoint<1>::version()
                                    ? getPenalizedAmount(fee, medianSize, currentBlockSize)
                                    : fee;
  const uint64_t staticReward = staticRewardAmountForBlockVersion(blockMajorVersion);

  emissionChange = penalizedBaseReward + staticReward - (fee - penalizedFee);
  reward = penalizedBaseReward + penalizedFee;

  return true;
}

size_t Currency::maxBlockCumulativeSize(uint64_t height) const {
  assert(height <= std::numeric_limits<uint64_t>::max() / m_maxBlockSizeGrowthSpeedNumerator);
  size_t maxSize = static_cast<size_t>(m_maxBlockSizeInitial + (height * m_maxBlockSizeGrowthSpeedNumerator) /
                                                                   m_maxBlockSizeGrowthSpeedDenominator);
  assert(maxSize >= m_maxBlockSizeInitial);
  return maxSize;
}

bool Currency::constructMinerTx(uint8_t blockMajorVersion, uint32_t height, size_t medianSize,
                                uint64_t alreadyGeneratedCoins, size_t currentBlockSize, uint64_t fee,
                                const AccountPublicAddress& minerAddress, Transaction& tx,
                                const BinaryArray& extraNonce /* = BinaryArray()*/, size_t maxOuts /* = 1*/) const {
  tx.inputs.clear();
  tx.outputs.clear();
  tx.extra.clear();

  KeyPair txkey = generateKeyPair();
  addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);
  if (!extraNonce.empty()) {
    if (!addExtraNonceToTransactionExtra(tx.extra, extraNonce)) {
      return false;
    }
  }

  BaseInput in;
  in.blockIndex = height;

  uint64_t blockReward;
  int64_t emissionChange;
  if (!getBlockReward(blockMajorVersion, medianSize, currentBlockSize, alreadyGeneratedCoins, fee, blockReward,
                      emissionChange)) {
    logger(INFO) << "Block is too big";
    return false;
  }

  std::vector<uint64_t> outAmounts;
  decompose_amount_into_digits(
      blockReward, defaultDustThreshold(height), [&outAmounts](uint64_t a_chunk) { outAmounts.push_back(a_chunk); },
      [&outAmounts](uint64_t a_dust) { outAmounts.push_back(a_dust); });

  if (!(1 <= maxOuts)) {
    logger(ERROR) << "max_out must be non-zero";
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
      logger(ERROR) << "while creating outs: failed to generate_key_derivation(" << minerAddress.viewPublicKey << ", "
                    << txkey.secretKey << ")";
      return false;
    }

    r = Crypto::derive_public_key(derivation, no, minerAddress.spendPublicKey, outEphemeralPubKey);

    if (!(r)) {
      logger(ERROR) << "while creating outs: failed to derive_public_key(" << derivation << ", " << no << ", "
                    << minerAddress.spendPublicKey << ")";
      return false;
    }

    KeyOutput tk;
    tk.key = outEphemeralPubKey;

    TransactionOutput out;
    summaryAmounts += out.amount = outAmounts[no];
    out.target = tk;
    tx.outputs.push_back(out);
  }

  if (!(summaryAmounts == blockReward)) {
    logger(ERROR) << "Failed to construct miner tx, summaryAmounts = " << summaryAmounts
                  << " not equal blockReward = " << blockReward;
    return false;
  }

  tx.version = Xi::Config::Transaction::version();
  // lock
  tx.unlockTime = height + m_minedMoneyUnlockWindow;
  tx.inputs.push_back(in);
  return true;
}

bool Currency::isStaticRewardEnabledForBlockVersion(uint8_t blockMajorVersion) const {
  return Xi::Config::StaticReward::isEnabled(blockMajorVersion);
}

uint64_t Currency::staticRewardAmountForBlockVersion(uint8_t blockMajorVersion) const {
  return Xi::Config::StaticReward::amount(blockMajorVersion);
}

std::string Currency::staticRewardAddressForBlockVersion(uint8_t blockMajorVersion) const {
  return Xi::Config::StaticReward::address(blockMajorVersion);
}

Xi::Result<boost::optional<Transaction>> Currency::constructStaticRewardTx(uint8_t blockMajorVersion,
                                                                           uint32_t blockIndex) const {
  XI_ERROR_TRY();
  const auto rewardAmount = staticRewardAmountForBlockVersion(blockMajorVersion);
  const auto rewardAddress = staticRewardAddressForBlockVersion(blockMajorVersion);
  if (rewardAddress.empty() || rewardAmount == 0) {
    if (!rewardAddress.empty()) {
      logger(ERROR) << "Static reward address set but amount is zero, consider deleting the static reward address.";
    } else if (rewardAmount > 0) {
      logger(ERROR)
          << "Static reward amount set but no address given, consider setting the static reward amount to zero.";
    } else {
      logger(TRACE) << "Skipping static reward.";
    }
    return Xi::make_result<boost::optional<Transaction>>(boost::none);
  }
  logger(TRACE) << "Generating static reward: (" << rewardAddress << ", " << rewardAmount << ")";

  AccountPublicAddress parsedRewardAddress = boost::value_initialized<AccountPublicAddress>();
  if (!parseAccountAddressString(rewardAddress, parsedRewardAddress)) {
    Xi::exceptional<AccountPublicAddressParseError>();
  }

  Transaction tx{};
  tx.nullify();

  const KeyPair txkey = generateKeyPair(blockIndex);
  addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);

  BaseInput in;
  in.blockIndex = blockIndex;

  std::vector<uint64_t> outAmounts;
  const auto dustThreshold = defaultDustThresholdForMajorVersion(blockMajorVersion);
  decompose_amount_into_digits(
      rewardAmount, dustThreshold, [&outAmounts](uint64_t a_chunk) { outAmounts.push_back(a_chunk); },
      [&outAmounts](uint64_t a_dust) { outAmounts.push_back(a_dust); });

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

    TransactionOutput out;
    summaryAmounts += out.amount = outAmounts[no];
    out.target = tk;
    tx.outputs.push_back(out);
  }

  if (!(summaryAmounts == rewardAmount)) {
    Xi::exceptional<RewardMissmatchError>();
  }

  tx.version = Xi::Config::Transaction::version();
  // lock
  tx.unlockTime = blockIndex + m_minedMoneyUnlockWindow;
  tx.inputs.push_back(in);

  return Xi::make_result<boost::optional<Transaction>>(std::move(tx));
  XI_ERROR_CATCH();
}

bool Currency::isFusionTransaction(const std::vector<uint64_t>& inputsAmounts,
                                   const std::vector<uint64_t>& outputsAmounts, size_t size, uint32_t height) const {
  if (size > fusionTxMaxSize(majorBlockVersionForHeight(height))) {
    return false;
  }

  if (inputsAmounts.size() < fusionTxMinInputCount()) {
    return false;
  }

  if (inputsAmounts.size() < outputsAmounts.size() * fusionTxMinInOutCountRatio()) {
    return false;
  }

  uint64_t inputAmount = 0;
  for (auto amount : inputsAmounts) {
    if (amount < defaultFusionDustThreshold(height)) {
      return false;
    }

    inputAmount += amount;
  }

  std::vector<uint64_t> expectedOutputsAmounts;
  expectedOutputsAmounts.reserve(outputsAmounts.size());
  decomposeAmount(inputAmount, defaultFusionDustThreshold(height), expectedOutputsAmounts);
  std::sort(expectedOutputsAmounts.begin(), expectedOutputsAmounts.end());

  return expectedOutputsAmounts == outputsAmounts;
}

bool Currency::isFusionTransaction(const Transaction& transaction, size_t size, uint32_t height) const {
  assert(getObjectBinarySize(transaction) == size);

  std::vector<uint64_t> outputsAmounts;
  outputsAmounts.reserve(transaction.outputs.size());
  for (const TransactionOutput& output : transaction.outputs) {
    outputsAmounts.push_back(output.amount);
  }

  return isFusionTransaction(getInputsAmounts(transaction), outputsAmounts, size, height);
}

bool Currency::isFusionTransaction(const Transaction& transaction, uint32_t height) const {
  return isFusionTransaction(transaction, getObjectBinarySize(transaction), height);
}

bool Currency::isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold, uint32_t height) const {
  uint8_t ignore;
  return isAmountApplicableInFusionTransactionInput(amount, threshold, ignore, height);
}

bool Currency::isAmountApplicableInFusionTransactionInput(uint64_t amount, uint64_t threshold,
                                                          uint8_t& amountPowerOfTen, uint32_t height) const {
  if (amount >= threshold) {
    return false;
  }

  if (amount < defaultFusionDustThreshold(height)) {
    return false;
  }

  auto it = std::lower_bound(PRETTY_AMOUNTS.begin(), PRETTY_AMOUNTS.end(), amount);
  if (it == PRETTY_AMOUNTS.end() || amount != *it) {
    return false;
  }

  amountPowerOfTen = static_cast<uint8_t>(std::distance(PRETTY_AMOUNTS.begin(), it) / 9);
  return true;
}

std::string Currency::accountAddressAsString(const AccountBase& account) const {
  return getAccountAddressAsStr(m_publicAddressBase58Prefix, account.getAccountKeys().address);
}

std::string Currency::accountAddressAsString(const AccountPublicAddress& accountPublicAddress) const {
  return getAccountAddressAsStr(m_publicAddressBase58Prefix, accountPublicAddress);
}

bool Currency::parseAccountAddressString(const std::string& str, AccountPublicAddress& addr) const {
  uint64_t prefix;
  if (!CryptoNote::parseAccountAddressString(prefix, addr, str)) {
    return false;
  }

  if (prefix != m_publicAddressBase58Prefix) {
    logger(DEBUGGING) << "Wrong address prefix: " << prefix << ", expected " << m_publicAddressBase58Prefix;
    return false;
  }

  return true;
}

std::string Currency::formatAmount(uint64_t amount) const {
  std::string s = std::to_string(amount);
  if (s.size() < m_numberOfDecimalPlaces + 1) {
    s.insert(0, m_numberOfDecimalPlaces + 1 - s.size(), '0');
  }
  s.insert(s.size() - m_numberOfDecimalPlaces, ".");
  return s;
}

std::string Currency::formatAmount(int64_t amount) const {
  std::string s = formatAmount(static_cast<uint64_t>(std::abs(amount)));

  if (amount < 0) {
    s.insert(0, "-");
  }

  return s;
}

bool Currency::parseAmount(const std::string& str, uint64_t& amount) const {
  std::string strAmount = str;
  boost::algorithm::trim(strAmount);

  size_t pointIndex = strAmount.find_first_of('.');
  size_t fractionSize;
  if (std::string::npos != pointIndex) {
    fractionSize = strAmount.size() - pointIndex - 1;
    while (m_numberOfDecimalPlaces < fractionSize && '0' == strAmount.back()) {
      strAmount.erase(strAmount.size() - 1, 1);
      --fractionSize;
    }
    if (m_numberOfDecimalPlaces < fractionSize) {
      return false;
    }
    strAmount.erase(pointIndex, 1);
  } else {
    fractionSize = 0;
  }

  if (strAmount.empty()) {
    return false;
  }

  if (!std::all_of(strAmount.begin(), strAmount.end(), ::isdigit)) {
    return false;
  }

  if (fractionSize < m_numberOfDecimalPlaces) {
    strAmount.append(m_numberOfDecimalPlaces - fractionSize, '0');
  }

  return Common::fromString(strAmount, amount);
}

uint64_t Currency::nextDifficulty(uint8_t version, uint32_t blockIndex, std::vector<uint64_t> timestamps,
                                  std::vector<uint64_t> cumulativeDifficulties) const {
  XI_UNUSED(blockIndex);
  return Xi::Config::Difficulty::nextDifficulty(version, timestamps, cumulativeDifficulties);
}

bool Currency::checkProofOfWork(const CachedBlock& block, uint64_t currentDiffic) const {
  return check_hash(block.getBlockLongHash(), currentDiffic);
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

Currency::Currency(ILogger& log) : logger(log, "currency") {}

Currency::Currency(Currency&& currency)
    : m_maxBlockHeight(currency.m_maxBlockHeight),
      m_maxBlockBlobSize(currency.m_maxBlockBlobSize),
      m_maxTxSize(currency.m_maxTxSize),
      m_publicAddressBase58Prefix(currency.m_publicAddressBase58Prefix),
      m_minedMoneyUnlockWindow(currency.m_minedMoneyUnlockWindow),
      m_timestampCheckWindow(currency.m_timestampCheckWindow),
      m_blockFutureTimeLimit(currency.m_blockFutureTimeLimit),
      m_moneySupply(currency.m_moneySupply),
      m_emissionSpeedFactor(currency.m_emissionSpeedFactor),
      m_minerTxBlobReservedSize(currency.m_minerTxBlobReservedSize),
      m_numberOfDecimalPlaces(currency.m_numberOfDecimalPlaces),
      m_coin(currency.m_coin),
      m_mininumFee(currency.m_mininumFee),
      m_difficultyTarget(currency.m_difficultyTarget),
      m_maxBlockSizeInitial(currency.m_maxBlockSizeInitial),
      m_maxBlockSizeGrowthSpeedNumerator(currency.m_maxBlockSizeGrowthSpeedNumerator),
      m_maxBlockSizeGrowthSpeedDenominator(currency.m_maxBlockSizeGrowthSpeedDenominator),
      m_lockedTxAllowedDeltaSeconds(currency.m_lockedTxAllowedDeltaSeconds),
      m_lockedTxAllowedDeltaBlocks(currency.m_lockedTxAllowedDeltaBlocks),
      m_mempoolTxLiveTime(currency.m_mempoolTxLiveTime),
      m_numberOfPeriodsToForgetTxDeletedFromPool(currency.m_numberOfPeriodsToForgetTxDeletedFromPool),
      m_fusionTxMaxSize(currency.m_fusionTxMaxSize),
      m_fusionTxMinInputCount(currency.m_fusionTxMinInputCount),
      m_fusionTxMinInOutCountRatio(currency.m_fusionTxMinInOutCountRatio),
      m_blocksFileName(currency.m_blocksFileName),
      m_blockIndexesFileName(currency.m_blockIndexesFileName),
      m_txPoolFileName(currency.m_txPoolFileName),
      m_genesisBlockReward(currency.m_genesisBlockReward),
      m_network(currency.m_network),
      m_genesisBlockTemplate(currency.m_genesisBlockTemplate),
      m_cachedGenesisBlock(new CachedBlock(m_genesisBlockTemplate)),
      logger(currency.logger) {}

CurrencyBuilder::CurrencyBuilder(Logging::ILogger& log) : m_currency(log) {
  maxBlockNumber(Xi::Config::Limits::maximumHeight());
  maxBlockBlobSize(Xi::Config::Limits::maximumBlockBlobSize());
  maxTxSize(Xi::Config::Limits::maximumTransactionSize());
  publicAddressBase58Prefix(Xi::Config::Coin::addressBas58Prefix());

  moneySupply(Xi::Config::Coin::totalSupply());
  emissionSpeedFactor(Xi::Config::Coin::emissionSpeed());
  genesisBlockReward(Xi::Config::Coin::amountOfPremine());

  minerTxBlobReservedSize(Xi::Config::Limits::blockBlobCoinbaseReservedSize());
  minedMoneyUnlockWindow(Xi::Config::Time::minerRewardUnlockBlocksCount());

  numberOfDecimalPlaces(Xi::Config::Coin::numberOfDecimalPoints());

  mininumFee(Xi::Config::Coin::minimumFee());

  difficultyTarget(Xi::Config::Time::blockTimeSeconds());

  maxBlockSizeInitial(Xi::Config::Limits::initialBlockBlobSizeLimit());
  maxBlockSizeGrowthSpeedNumerator(Xi::Config::Limits::blockBlobSizeGrowthNumerator());
  maxBlockSizeGrowthSpeedDenominator(Xi::Config::Limits::blockBlobSizeGrowthDenominator());

  lockedTxAllowedDeltaSeconds(
      static_cast<uint64_t>(Xi::Config::Limits::maximumTimeWindowForLockedTransation().count()));
  lockedTxAllowedDeltaBlocks(Xi::Config::Limits::maximumBlockWindowForLockedTransation());

  mempoolTxLiveTime(static_cast<uint64_t>(Xi::Config::Limits::maximumTransactionLivetimeSpan().count()));
  mempoolTxFromAltBlockLiveTime(
      static_cast<uint64_t>(Xi::Config::Limits::maximumTransactionLivetimeSpanFromAltBlocks().count()));
  numberOfPeriodsToForgetTxDeletedFromPool(Xi::Config::Limits::minimumTransactionLivetimeSpansUntilDeletion());

  fusionTxMaxSize(Xi::Config::Limits::maximumFusionTransactionSize());
  fusionTxMinInputCount(Xi::Config::Limits::minimumFusionTransactionInputCount());
  fusionTxMinInOutCountRatio(Xi::Config::Limits::minimumFusionTransactionInputOutputRatio());

  blocksFileName(Xi::Config::Database::blocksFilename());
  blockIndexesFileName(Xi::Config::Database::blockIndicesFilename());
  txPoolFileName(Xi::Config::Database::pooldataFilename());

  network(::Xi::Config::Network::defaultNetworkType());
}

Transaction CurrencyBuilder::generateGenesisTransaction() {
  CryptoNote::Transaction tx;
  CryptoNote::AccountPublicAddress ac = boost::value_initialized<CryptoNote::AccountPublicAddress>();
  m_currency.constructMinerTx(1, 0, 0, 0, 0, 0, ac, tx, BinaryArray{}, 20);  // zero fee in genesis
  return tx;
}
Transaction CurrencyBuilder::generateGenesisTransaction(const std::vector<AccountPublicAddress>& targets) {
  assert(!targets.empty());

  CryptoNote::Transaction tx;
  tx.inputs.clear();
  tx.outputs.clear();
  tx.extra.clear();
  tx.version = Xi::Config::Transaction::version();
  tx.unlockTime = m_currency.m_minedMoneyUnlockWindow;
  KeyPair txkey = generateKeyPair();
  addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);
  BaseInput in;
  in.blockIndex = 0;
  tx.inputs.push_back(in);
  uint64_t block_reward = m_currency.m_genesisBlockReward;
  uint64_t target_amount = block_reward / targets.size();
  uint64_t first_target_amount = target_amount + block_reward % targets.size();
  for (size_t i = 0; i < targets.size(); ++i) {
    Crypto::KeyDerivation derivation = boost::value_initialized<Crypto::KeyDerivation>();
    Crypto::PublicKey outEphemeralPubKey = boost::value_initialized<Crypto::PublicKey>();
    if (!Crypto::generate_key_derivation(targets[i].viewPublicKey, txkey.secretKey, derivation)) {
      Xi::exceptional<Crypto::KeyDerivationError>();
    }
    if (!Crypto::derive_public_key(derivation, i, targets[i].spendPublicKey, outEphemeralPubKey)) {
      Xi::exceptional<Crypto::PublicKeyDerivationError>();
    }
    KeyOutput tk;
    tk.key = outEphemeralPubKey;
    TransactionOutput out;
    out.amount = (i == 0) ? first_target_amount : target_amount;
    std::cout << "outs: " << std::to_string(out.amount) << std::endl;
    out.target = tk;
    tx.outputs.push_back(out);
  }
  return tx;
}
CurrencyBuilder& CurrencyBuilder::emissionSpeedFactor(unsigned int val) {
  if (val <= 0 || val > 8 * sizeof(uint64_t)) {
    throw std::invalid_argument("val at emissionSpeedFactor()");
  }

  m_currency.m_emissionSpeedFactor = val;
  return *this;
}

CurrencyBuilder& CurrencyBuilder::numberOfDecimalPlaces(size_t val) {
  m_currency.m_numberOfDecimalPlaces = val;
  m_currency.m_coin = 1;
  for (size_t i = 0; i < m_currency.m_numberOfDecimalPlaces; ++i) {
    m_currency.m_coin *= 10;
  }

  return *this;
}

}  // namespace CryptoNote
