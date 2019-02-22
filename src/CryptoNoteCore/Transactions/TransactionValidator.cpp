/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include "CryptoNoteCore/Transactions/TransactionValidator.h"

#include <algorithm>
#include <functional>

#include <crypto/crypto.h>

#include "CryptoNoteCore/IBlockchainCache.h"
#include "CryptoNoteCore/Transactions/Mixins.h"
#include "CryptoNoteCore/Transactions/TransactionApi.h"
#include "CryptoNoteCore/Transactions/TransactionUtils.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"
#include "CryptoNoteCore/BlockchainCache.h"

using Error = CryptoNote::error::TransactionValidationError;

CryptoNote::TransactionValidator::TransactionValidator(uint8_t _blockVersion, const IBlockchainCache &chain,
                                                       const Currency &currency)
    : m_blockVersion{_blockVersion}, m_chain{chain}, m_currency{currency} {}

uint8_t CryptoNote::TransactionValidator::blockVersion() const { return m_blockVersion; }
const CryptoNote::IBlockchainCache &CryptoNote::TransactionValidator::chain() const { return m_chain; }
const CryptoNote::Currency &CryptoNote::TransactionValidator::currency() const { return m_currency; }

Xi::Result<CryptoNote::EligibleIndex> CryptoNote::TransactionValidator::doValidate(
    const CachedTransaction &transaction) const {
  const auto &tx = transaction.getTransaction();
  EligibleIndex eligibleIndex{};

  if (chain().hasTransaction(transaction.getTransactionHash())) return Xi::make_error(Error::EXISTS_IN_BLOCKCHAIN);
  if (transaction.getBlobSize() > currency().maxTxSize()) return Xi::make_error(Error::TOO_LARGE);
  if (isExtraTooLarge(tx)) return Xi::make_error(Error::EXTRA_NONCE_TOO_LARGE);
  if (hasUnsupportedVersion(tx.version)) return Xi::make_error(Error::INVALID_VERSION);
  if (tx.inputs.empty()) return Xi::make_error(Error::EMPTY_INPUTS);

  if (containsUnsupportedInputTypes(tx)) return Xi::make_error(Error::BASE_INPUT_UNEXPECTED_TYPE);
  if (containsUnsupportedOutputTypes(tx)) return Xi::make_error(Error::OUTPUT_UNEXPECTED_TYPE);
  if (containsEmptyOutput(tx)) return Xi::make_error(Error::OUTPUT_ZERO_AMOUNT);
  if (containsInvalidOutputKey(transaction.getOutputKeys())) return Xi::make_error(Error::OUTPUT_INVALID_KEY);
  if (hasInputOverflow(tx)) return Xi::make_error(Error::INPUTS_AMOUNT_OVERFLOW);
  if (hasOutputOverflow(tx)) return Xi::make_error(Error::OUTPUTS_AMOUNT_OVERFLOW);
  if (containsKeyImageDuplicates(transaction.getKeyImages())) return Xi::make_error(Error::INPUT_IDENTICAL_KEYIMAGES);
  if (containsSpendedKey(transaction.getKeyImagesSet())) return Xi::make_error(Error::INPUT_KEYIMAGE_ALREADY_SPENT);

  {
    const auto mixinValidationResult = validateMixin(transaction);
    if (mixinValidationResult != Error::VALIDATION_SUCCESS) return Xi::make_error(mixinValidationResult);
  }

  const uint64_t inputAmount = transaction.getInputAmount();
  const uint64_t outputAmount = transaction.getOutputAmount();

  if (inputAmount < outputAmount) return Xi::make_error(Error::INPUT_AMOUNT_INSUFFICIENT);

  if (isFeeInsufficient(transaction)) return Xi::make_error(Error::FEE_INSUFFICIENT);

  if (!isInCheckpointRange()) {
    const auto inputValidationResult = validateInputs(transaction);
    if (inputValidationResult.isError()) {
      return inputValidationResult.error();
    } else {
      eligibleIndex = inputValidationResult.value();
    }
    if (containsInvalidDomainKeyImage(transaction.getKeyImages()))
      return Xi::make_error(Error::INPUT_INVALID_DOMAIN_KEYIMAGES);
  }
  return eligibleIndex;
}

CryptoNote::error::TransactionValidationError CryptoNote::TransactionValidator::getErrorCode(
    ExtractOutputKeysResult e) const {
  // TODO Consider real error code for core routines to simply forward xi errors.
  if (e == ExtractOutputKeysResult::INVALID_GLOBAL_INDEX) {
    return Error::INPUT_INVALID_GLOBAL_INDEX;
  } else if (e == ExtractOutputKeysResult::OUTPUT_LOCKED) {
    return Error::INPUT_SPEND_LOCKED_OUT;
  } else {
    return Error::INPUT_INVALID_UNKNOWN;
  }
}

bool CryptoNote::TransactionValidator::hasUnsupportedVersion(const uint8_t version) const {
  return version < currency().minTxVersion() || version > currency().minTxVersion();
}

bool CryptoNote::TransactionValidator::containsUnsupportedInputTypes(const CryptoNote::Transaction &transaction) const {
  return std::any_of(transaction.inputs.begin(), transaction.inputs.end(),
                     [](const auto &input) { return input.type() != typeid(KeyInput); });
}

bool CryptoNote::TransactionValidator::containsUnsupportedOutputTypes(
    const CryptoNote::Transaction &transaction) const {
  return std::any_of(transaction.outputs.begin(), transaction.outputs.end(),
                     [](const auto &output) { return output.target.type() != typeid(KeyOutput); });
}

bool CryptoNote::TransactionValidator::containsEmptyOutput(const CryptoNote::Transaction &transaction) const {
  return std::any_of(transaction.outputs.begin(), transaction.outputs.end(),
                     [](const auto &output) { return getTransactionOutputAmount(output) == 0; });
}

bool CryptoNote::TransactionValidator::containsInvalidOutputKey(const std::vector<Crypto::PublicKey> &keys) const {
  return std::any_of(keys.begin(), keys.end(), [](const auto &key) { return !check_key(key); });
}

bool CryptoNote::TransactionValidator::hasInputOverflow(const CryptoNote::Transaction &transaction) const {
  uint64_t money = 0;
  for (const auto &input : transaction.inputs) {
    uint64_t amount = getTransactionInputAmount(input);
    if (money > amount + money) return true;
    money += amount;
  }
  return false;
}

bool CryptoNote::TransactionValidator::hasOutputOverflow(const CryptoNote::Transaction &transaction) const {
  uint64_t money = 0;
  for (const auto &output : transaction.outputs) {
    uint64_t amount = getTransactionOutputAmount(output);
    if (money > amount + money) return true;
    money += amount;
  }
  return false;
}

bool CryptoNote::TransactionValidator::containsKeyImageDuplicates(
    const std::vector<Crypto::KeyImage> &keyImages) const {
  Crypto::KeyImagesSet set;
  for (const auto &keyImage : keyImages) {
    if (!set.insert(keyImage).second) return true;
  }
  return set.size() != keyImages.size();
}

bool CryptoNote::TransactionValidator::isExtraTooLarge(const CryptoNote::Transaction &transaction) const {
  return transaction.extra.size() > TX_EXTRA_NONCE_MAX_COUNT;
}

bool CryptoNote::TransactionValidator::isInvalidDomainKeyImage(const Crypto::KeyImage &keyImage) {
  static const Crypto::KeyImage I = {{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
  static const Crypto::KeyImage L = {{0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7,
                                      0xa2, 0xde, 0xf9, 0xde, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10}};
  return scalarmultKey(keyImage, L) != I;
}

bool CryptoNote::TransactionValidator::containsInvalidDomainKeyImage(
    const std::vector<Crypto::KeyImage> &keyImages) const {
  return std::any_of(keyImages.begin(), keyImages.end(), &TransactionValidator::isInvalidDomainKeyImage);
}

bool CryptoNote::TransactionValidator::containsSpendedKey(const Crypto::KeyImagesSet &keyImages) const {
  if (isInCheckpointRange()) {
    return false;
  } else {
    return std::any_of(keyImages.begin(), keyImages.end(),
                       [&, this](const auto &keyImage) { return checkIfKeyImageIsAlreadySpent(keyImage); });
  }
}

Xi::Result<std::tuple<std::vector<Crypto::PublicKey>, CryptoNote::EligibleIndex>>
CryptoNote::TransactionValidator::extractOutputKeys(uint64_t amount, const std::vector<uint32_t> &indices) const {
  uint32_t minHeight = 0;
  uint64_t minTimestamp = 0;
  std::vector<Crypto::PublicKey> outputKeys;
  outputKeys.reserve(indices.size());
  auto queryResult = chain().extractKeyOutputs(
      amount, chain().getTopBlockIndex(), {indices.data(), indices.size()},
      [&, this](const CachedTransactionInfo &info, PackedOutIndex index, uint32_t globalIndex) {
        XI_UNUSED(globalIndex);
        if (info.unlockTime < currency().maxBlockHeight()) {
          minHeight = std::max<uint32_t>(minHeight, static_cast<uint32_t>(info.unlockTime));
        } else {
          minTimestamp = std::max(minTimestamp, info.unlockTime);
        }
        outputKeys.push_back(boost::get<KeyOutput>(info.outputs[index.data.outputIndex]).key);
        return ExtractOutputKeysResult::SUCCESS;
      });
  if (queryResult != ExtractOutputKeysResult::SUCCESS) {
    return Xi::make_error(getErrorCode(queryResult));
  } else {
    return Xi::make_result<std::tuple<std::vector<Crypto::PublicKey>, EligibleIndex>>(
        std::move(outputKeys), EligibleIndex{minHeight, minTimestamp});
  }
}

Xi::Result<CryptoNote::EligibleIndex> CryptoNote::TransactionValidator::validateKeyInput(
    const CryptoNote::KeyInput &keyInput, size_t inputIndex, const CryptoNote::CachedTransaction &transaction) const {
  if (keyInput.outputIndexes.empty()) return Xi::make_error(Error::INPUT_EMPTY_OUTPUT_USAGE);

  const auto globalIndexes = getTransactionInputIndices(keyInput);
  auto extractionResult = extractOutputKeys(keyInput.amount, globalIndexes);
  if (extractionResult.isError()) return extractionResult.error();
  std::vector<Crypto::PublicKey> outputKeys{};
  EligibleIndex index;
  std::tie(outputKeys, index) = extractionResult.take();

  //  if (outputKeys.size() != transaction.getTransaction().outputs.size()) {
  //    return Xi::make_error(Error::INPUT_INVALID_SIGNATURES_COUNT);
  //  }
  if (outputKeys.size() != transaction.getTransaction().signatures[inputIndex].size()) {
    return Xi::make_error(Error::INPUT_INVALID_SIGNATURES_COUNT);
  }

  std::vector<const Crypto::PublicKey *> outputKeyPointers;
  outputKeyPointers.reserve(outputKeys.size());
  std::for_each(outputKeys.begin(), outputKeys.end(),
                [&outputKeyPointers](const Crypto::PublicKey &key) { outputKeyPointers.push_back(&key); });
  if (!Crypto::check_ring_signature(transaction.getTransactionPrefixHash(), keyInput.keyImage, outputKeyPointers.data(),
                                    outputKeyPointers.size(),
                                    transaction.getTransaction().signatures[inputIndex].data(), true)) {
    return Xi::make_error(Error::INPUT_INVALID_SIGNATURES);
  }
  return index;
}

Xi::Result<CryptoNote::EligibleIndex> CryptoNote::TransactionValidator::validateInputs(
    const CryptoNote::CachedTransaction &transaction) const {
  size_t inputIndex = 0;
  uint32_t minHeight = 0;
  uint64_t minTimestamp = 0;
  for (const auto &input : transaction.getTransaction().inputs) {
    if (input.type() == typeid(KeyInput)) {
      const auto &keyInput = boost::get<KeyInput>(input);
      const auto keyInputValidation = validateKeyInput(keyInput, inputIndex, transaction);
      if (keyInputValidation.isError())
        return keyInputValidation.error();
      else {
        auto index = keyInputValidation.value();
        minHeight = std::max(minHeight, index.Height);
        minTimestamp = std::max(minTimestamp, index.Timestamp);
      }
    } else {
      return Xi::make_error(Error::BASE_INPUT_UNEXPECTED_TYPE);
    }
    inputIndex += 1;
  }

  return Xi::make_result<EligibleIndex>(minHeight, minTimestamp);
}

CryptoNote::error::TransactionValidationError CryptoNote::TransactionValidator::validateMixin(
    const CryptoNote::CachedTransaction &transaction) const {
  const uint8_t minMixin = currency().minimumMixin(blockVersion());
  const uint8_t maxMixin = currency().maximumMixin(blockVersion());

  uint64_t ringSize = 1;

  const auto tx = createTransaction(transaction.getTransaction());

  for (size_t i = 0; i < tx->getInputCount(); ++i) {
    if (tx->getInputType(i) != TransactionTypes::InputType::Key) {
      continue;
    }

    KeyInput input;
    tx->getInput(i, input);
    const uint64_t currentRingSize = input.outputIndexes.size();
    if (currentRingSize > ringSize) {
      ringSize = currentRingSize;
    }
  }

  const uint64_t mixin = ringSize - 1;

  if (mixin > maxMixin) {
    return Error::INPUT_MIXIN_TOO_HIGH;
  } else if (mixin < minMixin) {
    return Error::INPUT_MIXIN_TOO_LOW;
  } else {
    return Error::VALIDATION_SUCCESS;
  }
}
