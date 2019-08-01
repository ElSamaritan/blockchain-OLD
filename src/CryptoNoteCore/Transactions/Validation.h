/* ============================================================================================== *
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

#pragma once

#include <unordered_map>

#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/IBlockchainCache.h"
#include "CryptoNoteCore/Transactions/Transaction.h"
#include "CryptoNoteCore/Transactions/CachedTransaction.h"
#include "CryptoNoteCore/Transactions/TransactionValidationErrors.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"

namespace CryptoNote {

error::TransactionValidationError validateExtra(const Transaction& tx);

bool validateExtraSize(const Transaction& tx);
bool validateExtraCumulativePadding(const std::vector<TransactionExtraField>& fields);

bool validateExtraPublicKeys(const std::vector<TransactionExtraField>& fields);
bool validateExtraPublicKeys(const TransactionExtraPublicKey& pk);

bool validateExtraNonce(const std::vector<TransactionExtraField>& fields);
bool validateExtraNonce(const TransactionExtraNonce& nonce);

bool validateCanonicalDecomposition(const Transaction& tx);

struct TransferValidationContext {
  const Currency& currency;
  const IBlockchainCache& segment;
  BlockVersion blockVersion = BlockVersion::Genesis;
  uint32_t previousBlockIndex = 0;
  uint64_t timestamp = 0;
  bool inCheckpointRange = false;

  uint8_t minimumMixin = 0;
  uint8_t maximumMixin = 0;
  uint64_t upgradeMixin = 0;

  explicit TransferValidationContext(const Currency& _currency, const IBlockchainCache& _segment)
      : currency{_currency}, segment{_segment} {
  }
};

/// Contains all informations gathered by the pre validate transfer call.
struct TransferValidationState {
  Crypto::KeyImageSet usedKeyImages{};

  std::unordered_map<Amount, GlobalOutputIndexSet> globalOutputIndicesUsed{};

  Amount fee = 0;
  Amount inputSum = 0;
  Amount outputSum = 0;
};

/// Contains all informations that need to be cached between the pre and post transfer validation call.
struct TransferValidationCache {
  std::vector<GlobalOutputIndexVector> globalIndicesForInput{};
};

/// This info is gathered between pre and post validation and provides information that was gathered using a single
/// batch.
struct TransferValidationInfo {
  std::map<Amount, std::map<GlobalOutputIndex, KeyOutputInfo>> outputs;
  std::map<Amount, uint64_t> mixinsUpgradeThreshold;
  std::map<Amount, uint64_t> requiredMixins;
};

[[nodiscard]] std::error_code preValidateTransfer(const CachedTransaction& transaction,
                                                  const TransferValidationContext& context,
                                                  TransferValidationCache& cache, TransferValidationState& out);

[[nodiscard]] std::error_code postValidateTransfer(const CachedTransaction& transaction,
                                                   const TransferValidationContext& context,
                                                   TransferValidationCache& cache, const TransferValidationInfo& info);

[[nodiscard]] std::error_code makeTransferValidationInfo(const IBlockchainCache& segment,
                                                         const TransferValidationContext& context,
                                                         const std::unordered_map<Amount, GlobalOutputIndexSet>& refs,
                                                         uint32_t blockIndex, TransferValidationInfo& info);

}  // namespace CryptoNote
