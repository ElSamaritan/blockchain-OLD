/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

using Error = CryptoNote::error::TransactionValidationError;

CryptoNote::TransactionValidator::TransactionValidator(const TransactionValidationContext &ctx) : m_context{ctx} {}

const CryptoNote::TransactionValidationContext &CryptoNote::TransactionValidator::context() const { return m_context; }

CryptoNote::TransactionValidationResult CryptoNote::TransactionValidator::doValidate(const Transaction &tx) const {
  CachedTransaction transaction{tx};

  if (transaction.getBlobSize() > m_context.currency().maxTxSize()) return makeError(Error::TOO_LARGE);

  if (hasUnsupportedVersion(tx.version)) return makeError(Error::INVALID_VERSION);
  if (tx.inputs.empty()) return makeError(Error::EMPTY_INPUTS);

  if (containsUnsupportedInputTypes(tx)) return makeError(Error::BASE_INPUT_UNEXPECTED_TYPE);
  if (containsUnsupportedOutputTypes(tx)) return makeError(Error::OUTPUT_UNEXPECTED_TYPE);
  if (containsEmptyOutput(tx)) return makeError(Error::OUTPUT_ZERO_AMOUNT);
  if (containsInvalidOutputKey(transaction.getOutputKeys())) return makeError(Error::OUTPUT_INVALID_KEY);
  if (hasInputOverflow(tx)) return makeError(Error::INPUTS_AMOUNT_OVERFLOW);
  if (hasOutputOverflow(tx)) return makeError(Error::OUTPUTS_AMOUNT_OVERFLOW);
  if (containsKeyImageDuplicates(transaction.getKeyImages())) return makeError(Error::INPUT_IDENTICAL_KEYIMAGES);
  if (containsSpendedKey(transaction.getKeyImagesSet())) return makeError(Error::INPUT_KEYIMAGE_ALREADY_SPENT);

  {
    const auto mixinValidationResult = validateMixin(transaction);
    if (mixinValidationResult != Error::VALIDATION_SUCCESS) return makeError(mixinValidationResult);
  }

  const uint64_t inputAmount = transaction.getInputAmount();
  const uint64_t outputAmount = transaction.getOutputAmount();

  if (inputAmount < outputAmount) return makeError(Error::INPUT_AMOUNT_INSUFFICIENT);

  const uint64_t fee = transaction.getTransactionFee();
  if (!m_context.isContainedInBlock()) {
    const bool isFusionTransaction =
        fee == 0 && m_context.currency().isFusionTransaction(tx, m_context.blockchain().getTopBlockIndex() + 1);
    if (!isFusionTransaction && fee < m_context.currency().minimumFee()) return makeError(Error::FEE_INSUFFICIENT);
  }

  if (!m_context.isContainedInCheckpointsRange()) {
    const auto inputValidationResult = validateInputs(transaction);
    if (inputValidationResult != Error::VALIDATION_SUCCESS) return makeError(inputValidationResult);
    if (containsInvalidDomainKeyImage(transaction.getKeyImages()))
      return makeError(Error::INPUT_INVALID_DOMAIN_KEYIMAGES);
  }

  return TransactionValidationResult{std::move(transaction)};
}

CryptoNote::TransactionValidationResult CryptoNote::TransactionValidator::makeError(
    CryptoNote::error::TransactionValidationError e) const {
  return TransactionValidationResult{Xi::Error{error::make_error_code(e)}};
}

bool CryptoNote::TransactionValidator::hasUnsupportedVersion(const uint8_t version) const {
  return version < m_context.currency().minTxVersion() || m_context.currency().minTxVersion();
}

bool CryptoNote::TransactionValidator::containsUnsupportedInputTypes(const CryptoNote::Transaction &transaction) const {
  return std::any_of(transaction.inputs.begin(), transaction.inputs.end(),
                     [](const auto &input) { return input.type() != typeid(KeyInput); });
}

bool CryptoNote::TransactionValidator::containsUnsupportedOutputTypes(
    const CryptoNote::Transaction &transaction) const {
  return std::any_of(transaction.outputs.begin(), transaction.outputs.end(),
                     [](const auto &output) { return output.target.type() != typeid(TransactionOutput); });
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

bool CryptoNote::TransactionValidator::containsSpendedKey(const Crypto::KeyImage &keyImage) const {
  if (m_context.keyImages().find(keyImage) != m_context.keyImages().end()) {
    return true;
  } else {
    return m_context.blockchain().checkIfSpent(keyImage);
  }
}

bool CryptoNote::TransactionValidator::containsSpendedKey(const Crypto::KeyImage &keyImage, uint32_t height) const {
  if (m_context.keyImages().find(keyImage) != m_context.keyImages().end()) {
    return true;
  } else {
    return m_context.blockchain().checkIfSpent(keyImage, height);
  }
}

bool CryptoNote::TransactionValidator::containsSpendedKey(const Crypto::KeyImagesSet &keyImages) const {
  if (m_context.isContainedInCheckpointsRange()) {
    return false;
  } else {
    if (m_context.isContainedInBlock()) {
      return std::any_of(keyImages.begin(), keyImages.end(), [&, this](const auto &keyImage) {
        return containsSpendedKey(keyImage, m_context.validationHeight());
      });
    } else {
      return std::any_of(keyImages.begin(), keyImages.end(),
                         [&, this](const auto &keyImage) { return containsSpendedKey(keyImage); });
    }
  }
}

CryptoNote::error::TransactionValidationError CryptoNote::TransactionValidator::validateKeyInput(
    const CryptoNote::KeyInput &keyInput, size_t inputIndex, const CryptoNote::CachedTransaction &transaction) const {
  if (keyInput.outputIndexes.empty()) return Error::INPUT_EMPTY_OUTPUT_USAGE;

  const auto globalIndexes = getTransactionInputIndices(keyInput);
  const uint32_t blockIndex = m_context.validationHeight();
  std::vector<Crypto::PublicKey> outputKeys;
  ExtractOutputKeysResult extractionResult;
  if (m_context.isContainedInBlock()) {
    extractionResult = m_context.blockchain().extractKeyOutputKeys(
        keyInput.amount, blockIndex, {globalIndexes.data(), globalIndexes.size()}, outputKeys,
        m_context.containingBlock().Header.timestamp);
  } else {
    extractionResult = m_context.blockchain().extractKeyOutputKeys(
        keyInput.amount, blockIndex, {globalIndexes.data(), globalIndexes.size()}, outputKeys);
  }
  if (extractionResult == ExtractOutputKeysResult::INVALID_GLOBAL_INDEX) {
    return Error::INPUT_INVALID_GLOBAL_INDEX;
  } else if (extractionResult == ExtractOutputKeysResult::OUTPUT_LOCKED) {
    return Error::INPUT_SPEND_LOCKED_OUT;
  } else if (extractionResult != ExtractOutputKeysResult::SUCCESS) {
    return Error::INPUT_INVALID_UNKNOWN;
  }

  if (outputKeys.size() != transaction.getTransaction().outputs.size()) {
    return Error::INPUT_INVALID_SIGNATURES_COUNT;
  }
  if (outputKeys.size() != transaction.getTransaction().signatures[inputIndex].size()) {
    return Error::INPUT_INVALID_SIGNATURES_COUNT;
  }

  std::vector<const Crypto::PublicKey *> outputKeyPointers;
  outputKeyPointers.reserve(outputKeys.size());
  std::for_each(outputKeys.begin(), outputKeys.end(),
                [&outputKeyPointers](const Crypto::PublicKey &key) { outputKeyPointers.push_back(&key); });
  if (!Crypto::check_ring_signature(transaction.getTransactionPrefixHash(), keyInput.keyImage, outputKeyPointers.data(),
                                    outputKeyPointers.size(),
                                    transaction.getTransaction().signatures[inputIndex].data(), true)) {
    return Error::INPUT_INVALID_SIGNATURES;
  }
  return Error::VALIDATION_SUCCESS;
}

CryptoNote::error::TransactionValidationError CryptoNote::TransactionValidator::validateInputs(
    const CryptoNote::CachedTransaction &transaction) const {
  size_t inputIndex = 0;
  for (const auto &input : transaction.getTransaction().inputs) {
    if (input.type() == typeid(KeyInput)) {
      const auto &keyInput = boost::get<KeyInput>(input);
      const auto keyInputValidation = validateKeyInput(keyInput, inputIndex, transaction);
      if (keyInputValidation != Error::VALIDATION_SUCCESS) return keyInputValidation;
    } else {
      return Error::BASE_INPUT_UNEXPECTED_TYPE;
    }
    inputIndex += 1;
  }

  return Error::VALIDATION_SUCCESS;
}

CryptoNote::error::TransactionValidationError CryptoNote::TransactionValidator::validateMixin(
    const CryptoNote::CachedTransaction &transaction) const {
  const uint8_t blockVersion = m_context.validationMajorBlockVersion();
  const uint8_t minMixin = m_context.currency().minimumMixin(blockVersion);
  const uint8_t maxMixin = m_context.currency().maximumMixin(blockVersion);

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
