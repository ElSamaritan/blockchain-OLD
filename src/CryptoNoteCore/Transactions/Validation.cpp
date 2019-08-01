﻿/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
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

#include "CryptoNoteCore/Transactions/Validation.h"

#include <algorithm>
#include <iterator>
#include <cassert>

#include <Xi/Algorithm/Math.h>
#include <Xi/Exceptional.hpp>

#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"

using Error = CryptoNote::error::TransactionValidationError;

Error CryptoNote::validateExtra(const CryptoNote::Transaction &tx, CryptoNote::TransactionExtraFeature exact) {
  return validateExtra(tx, exact, exact);
}

Error CryptoNote::validateExtra(const CryptoNote::Transaction &tx, TransactionExtraFeature required,
                                TransactionExtraFeature allowed) {
  const auto &extra = tx.extra;
  XI_RETURN_EC_IF_NOT(hasFlag(extra.features, required), Error::EXTRA_ILL_FORMED);
  XI_RETURN_EC_IF_NOT(discardFlag(extra.features, allowed) == TransactionExtraFeature::None, Error::EXTRA_ILL_FORMED);
  XI_RETURN_EC_IF_NOT(hasFlag(extra.features, TransactionExtraFeature::PublicKey) == extra.publicKey.has_value(),
                      Error::EXTRA_ILL_FORMED);
  XI_RETURN_EC_IF(extra.publicKey && !extra.publicKey->isValid(), Error::EXTRA_INVALID_PUBLIC_KEY);
  XI_RETURN_EC_IF_NOT(hasFlag(extra.features, TransactionExtraFeature::PaymentId) == extra.paymentId.has_value(),
                      Error::EXTRA_ILL_FORMED);
  XI_RETURN_EC_IF(extra.paymentId && !extra.paymentId->isValid(), Error::EXTRA_INVALID_PAYMENT_ID);
  return Error::VALIDATION_SUCCESS;
}

bool CryptoNote::validateCanonicalDecomposition(const CryptoNote::Transaction &tx) {
  for (const auto &output : tx.outputs) {
    if (const auto &amountOut = std::get_if<TransactionAmountOutput>(std::addressof(output))) {
      XI_RETURN_EC_IF_NOT(isCanonicalAmount(amountOut->amount), false);
    }
  }
  XI_RETURN_SC(true);
}

std::error_code CryptoNote::preValidateTransfer(const CryptoNote::CachedTransaction &transaction,
                                                const TransferValidationContext &context,
                                                TransferValidationCache &cache,
                                                CryptoNote::TransferValidationState &out) {
  const auto &tx = transaction.getTransaction();

  XI_RETURN_EC_IF(transaction.getBlobSize() > context.currency.maxTxSize(context.blockVersion), Error::TOO_LARGE);

  const auto unlock = tx.unlockTime;
  XI_RETURN_EC_IF(unlock > context.currency.unlockLimit(context.blockVersion), Error::UNLOCK_TOO_LARGE);
  if (context.currency.isLockedBasedOnTimestamp(unlock)) {
    XI_RETURN_EC_IF(unlock < context.currency.coin().startTimestamp(), Error::UNLOCK_ILL_FORMED);
    XI_RETURN_EC_IF(unlock > context.timestamp + context.currency.transaction(context.blockVersion).futureUnlockLimit(),
                    Error::UNLOCK_TOO_LARGE);
  } else {
    uint32_t blockFutureLimit =
        context.currency.transaction(context.blockVersion).futureUnlockLimit() / context.currency.coin().blockTime() +
        1;
    XI_RETURN_EC_IF(context.previousBlockIndex + 1 + blockFutureLimit < unlock, Error::UNLOCK_TOO_LARGE);
  }

  XI_RETURN_EC_IF_NOT(context.currency.isTransferVersionSupported(context.blockVersion, tx.version),
                      Error::INVALID_VERSION);

  if (const auto ec = validateExtra(tx, TransactionExtraFeature::PublicKey,
                                    TransactionExtraFeature::PublicKey | TransactionExtraFeature::PaymentId);
      ec != Error::VALIDATION_SUCCESS) {
    XI_RETURN_EC(ec);
  }

  XI_RETURN_EC_IF(tx.outputs.empty(), Error::EMPTY_OUTPUTS);
  for (const auto &anyOutput : tx.outputs) {
    XI_RETURN_EC_IF_NOT(std::holds_alternative<TransactionAmountOutput>(anyOutput), Error::OUTPUT_UNKNOWN_TYPE);
    const auto &output = std::get<TransactionAmountOutput>(anyOutput);
    XI_RETURN_EC_IF_NOT(std::holds_alternative<KeyOutput>(output.target), Error::OUTPUT_UNKNOWN_TYPE);
    const auto &keyTarget = std::get<KeyOutput>(output.target);
    if (!context.inCheckpointRange) {
      XI_RETURN_EC_IF_NOT(keyTarget.key.isValid(), Error::OUTPUT_INVALID_KEY);
    }
    XI_RETURN_EC_IF(output.amount == 0, Error::OUTPUT_ZERO_AMOUNT);
    XI_RETURN_EC_IF_NOT(isCanonicalAmount(output.amount), Error::OUTPUTS_NOT_CANONCIAL);
    if (Xi::hasAdditionOverflow(out.outputSum, output.amount, &out.outputSum)) {
      XI_RETURN_EC(Error::OUTPUTS_AMOUNT_OVERFLOW);
    }
  }
  XI_RETURN_EC_IF(out.outputSum == 0, Error::OUTPUT_ZERO);

  XI_RETURN_EC_IF(tx.inputs.empty(), Error::EMPTY_INPUTS);
  XI_RETURN_EC_IF_NOT(tx.signatures.has_value(), Error::INVALID_SIGNATURE_TYPE);
  XI_RETURN_EC_IF_NOT(std::holds_alternative<TransactionSignatureCollection>(*tx.signatures),
                      Error::INVALID_SIGNATURE_TYPE);
  const auto &signatures = std::get<TransactionSignatureCollection>(*tx.signatures);
  cache.globalIndicesForInput.reserve(tx.inputs.size());
  XI_RETURN_EC_IF_NOT(tx.inputs.size() == signatures.size(), Error::INPUT_INVALID_SIGNATURES_COUNT);
  for (size_t i = 0; i < tx.inputs.size(); ++i) {
    const auto &input = tx.inputs[i];
    XI_RETURN_EC_IF_NOT(std::holds_alternative<KeyInput>(input), Error::INPUT_UNKNOWN_TYPE);
    const auto &keyInput = std::get<KeyInput>(input);

    if (!context.inCheckpointRange) {
      XI_RETURN_EC_IF_NOT(keyInput.keyImage.isValid(), Error::INPUT_INVALID_DOMAIN_KEYIMAGES);
    }
    XI_RETURN_EC_IF(keyInput.amount == 0, Error::INPUT_ZERO_AMOUNT);
    XI_RETURN_EC_IF_NOT(isCanonicalAmount(keyInput.amount), Error::INPUTS_NOT_CANONICAL);
    if (Xi::hasAdditionOverflow(out.inputSum, keyInput.amount, &out.inputSum)) {
      XI_RETURN_EC(Error::INPUTS_AMOUNT_OVERFLOW);
    }

    XI_RETURN_EC_IF(keyInput.outputIndices.empty(), Error::INPUT_EMPTY_OUTPUT_USAGE);
    XI_RETURN_EC_IF_NOT(std::holds_alternative<TransactionRingSignature>(signatures[i]), Error::INVALID_SIGNATURE_TYPE);
    const auto &iRingSignature = std::get<TransactionRingSignature>(signatures[i]);
    XI_RETURN_EC_IF_NOT(keyInput.outputIndices.size() == iRingSignature.size(), Error::INPUT_INVALID_SIGNATURES_COUNT);
    for (const auto &signature : iRingSignature) {
      XI_RETURN_EC_IF_NOT(signature.isValid(), Error::INPUT_INVALID_SIGNATURES);
    }

    if (keyInput.outputIndices.size() > 1) {
      XI_RETURN_EC_IF(keyInput.outputIndices.size() < static_cast<size_t>(context.minimumMixin + 1),
                      Error::INPUT_MIXIN_TOO_LOW);
      XI_RETURN_EC_IF(keyInput.outputIndices.size() > static_cast<size_t>(context.maximumMixin + 1),
                      Error::INPUT_MIXIN_TOO_HIGH);
    }
    XI_RETURN_EC_IF_NOT(out.usedKeyImages.insert(keyInput.keyImage).second, Error::INPUT_IDENTICAL_KEYIMAGES);
    cache.globalIndicesForInput.emplace_back(relativeOutputOffsetsToAbsolute(keyInput.outputIndices));
    for (const auto &globalOutputIndex : cache.globalIndicesForInput.back()) {
      XI_RETURN_EC_IF_NOT(out.globalOutputIndicesUsed[keyInput.amount].insert(globalOutputIndex).second,
                          Error::INPUT_IDENTICAL_OUTPUT_INDEXES);
    }
  }
  XI_RETURN_EC_IF(out.inputSum < out.outputSum, Error::INPUT_AMOUNT_INSUFFICIENT);
  out.fee = out.inputSum - out.outputSum;

  const auto isFusionTransaction = out.fee == 0 && context.currency.isFusionTransaction(tx, context.blockVersion);
  if (!isFusionTransaction) {
    const auto canoncialBuckets = countCanonicalDecomposition(tx);
    XI_RETURN_EC_IF(out.fee < context.currency.minimumFee(context.blockVersion, canoncialBuckets),
                    Error::FEE_INSUFFICIENT);
  }

  return Error::VALIDATION_SUCCESS;
}

std::error_code CryptoNote::postValidateTransfer(const CryptoNote::CachedTransaction &transaction,
                                                 const CryptoNote::TransferValidationContext &context,
                                                 TransferValidationCache &cache, const TransferValidationInfo &info) {
  XI_UNUSED(context);

  const auto &tx = transaction.getTransaction();
  assert(tx.signatures.has_value());
  assert(std::holds_alternative<TransactionSignatureCollection>(*tx.signatures));
  const auto &signatures = std::get<TransactionSignatureCollection>(*tx.signatures);
  assert(tx.inputs.size() == signatures.size());

  for (size_t i = 0; i < tx.inputs.size(); ++i) {
    const auto &input = tx.inputs[i];
    assert(std::holds_alternvative<TransactionRingSignature>(signatures[i]));
    const auto &inputSignatures = std::get<TransactionRingSignature>(signatures[i]);
    assert(std::holds_alternative<KeyInput>(input));
    const auto &keyInput = std::get<KeyInput>(input);

    auto requiredMixinSearch = info.requiredMixins.find(keyInput.amount);
    assert(requiredMixinSearch != info.requiredMixins.end());
    const auto requiredRingSize = requiredMixinSearch->second + 1;
    XI_RETURN_EC_IF(keyInput.outputIndices.size() < requiredRingSize, Error::INPUT_MIXIN_TOO_LOW);
    XI_RETURN_EC_IF(keyInput.outputIndices.size() > requiredRingSize, Error::INPUT_MIXIN_TOO_HIGH);

    assert(inputSignatures.size() == keyInput.outputIndices.size());
    std::vector<const Crypto::PublicKey *> publicKeysReferenced{};
    publicKeysReferenced.reserve(keyInput.outputIndices.size());

    const auto &iGlobalIndicesUsed = cache.globalIndicesForInput[i];
    assert(iGlobalIndicesUsed.size() == keyInput.outputIndices.size());

    const auto referencedAmountsSearch = info.outputs.find(keyInput.amount);
    XI_RETURN_EC_IF(referencedAmountsSearch == info.outputs.end(), Error::INPUT_INVALID_GLOBAL_INDEX);
    for (const auto iGlobalIndex : iGlobalIndicesUsed) {
      const auto referencedPublicKeySearch = referencedAmountsSearch->second.find(iGlobalIndex);
      XI_RETURN_EC_IF(referencedPublicKeySearch == referencedAmountsSearch->second.end(),
                      Error::INPUT_INVALID_GLOBAL_INDEX);

      XI_RETURN_EC_IF_NOT(context.currency.isUnlockSatisfied(referencedPublicKeySearch->second.unlockTime,
                                                             context.previousBlockIndex + 1, context.timestamp),
                          Error::INPUT_SPEND_LOCKED_OUT);

      publicKeysReferenced.emplace_back(std::addressof(referencedPublicKeySearch->second.publicKey));
    }

    XI_RETURN_EC_IF_NOT(
        Crypto::check_ring_signature(transaction.getTransactionPrefixHash(), keyInput.keyImage,
                                     publicKeysReferenced.data(), publicKeysReferenced.size(), inputSignatures.data()),
        Error::INPUT_INVALID_SIGNATURES);
  }

  return Error::VALIDATION_SUCCESS;
}

std::error_code CryptoNote::makeTransferValidationInfo(const IBlockchainCache &segment,
                                                       const TransferValidationContext &context,
                                                       const std::unordered_map<Amount, GlobalOutputIndexSet> &refs,
                                                       uint32_t blockIndex, CryptoNote::TransferValidationInfo &info) {
  // Currently disabled due to an issue of outputs not found by the in memory cache.
  // info.outputs = segment.extractKeyOutputs(refs, blockIndex);
  for (const auto &iAmountRefs : refs) {
    GlobalOutputIndexVector indices{};
    indices.reserve(iAmountRefs.second.size());
    std::copy(iAmountRefs.second.begin(), iAmountRefs.second.end(), std::back_inserter(indices));
    const auto extractionResult = segment.extractKeyOutputs(
        iAmountRefs.first, context.previousBlockIndex,
        Common::ArrayView<GlobalOutputIndex>{indices.data(), indices.size()},
        [&](const CachedTransactionInfo &txinfo, PackedOutIndex index, uint32_t globalIndex) {
          KeyOutputInfo iInfo{};
          iInfo.index = index;
          iInfo.transactionHash = txinfo.transactionHash;
          iInfo.unlockTime = txinfo.unlockTime;
          if (!std::holds_alternative<TransactionAmountOutput>(txinfo.outputs[index.data.outputIndex])) {
            return ExtractOutputKeysResult::INVALID_TYPE;
          }
          const auto &amountOutput = std::get<TransactionAmountOutput>(txinfo.outputs[index.data.outputIndex]);
          if (!std::holds_alternative<KeyOutput>(amountOutput.target)) {
            return ExtractOutputKeysResult::INVALID_TYPE;
          }
          const auto &keyOutput = std::get<KeyOutput>(amountOutput.target);

          iInfo.publicKey = keyOutput.key;
          info.outputs[iAmountRefs.first].emplace(std::make_pair(globalIndex, std::move(iInfo)));
          return ExtractOutputKeysResult::SUCCESS;
        });
    if (extractionResult != ExtractOutputKeysResult::SUCCESS) {
      return Error::INPUT_INVALID_GLOBAL_INDEX;
    }
  }

  const auto queryMixinThreshold = context.maximumMixin * context.upgradeMixin + 1;
  for (const auto &inputUsed : refs) {
    const auto amount = inputUsed.first;
    const auto availableMixins = segment.getAvailableMixinsCount(amount, blockIndex, queryMixinThreshold);
    const auto requiredMixins = availableMixins / context.upgradeMixin;
    if (requiredMixins >= context.maximumMixin) {
      info.requiredMixins[amount] = context.maximumMixin;
    } else if (requiredMixins < context.minimumMixin) {
      info.requiredMixins[amount] = 0;
      info.mixinsUpgradeThreshold[amount] = context.minimumMixin * context.upgradeMixin - availableMixins;
    } else {
      info.requiredMixins[amount] = requiredMixins;
      info.mixinsUpgradeThreshold[amount] = (requiredMixins + 1) * context.upgradeMixin - availableMixins;
    }
  }

  return Error::VALIDATION_SUCCESS;
}
