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

#include "CryptoNoteCore/Transactions/TransactionValidator.h"

#include <algorithm>
#include <functional>

#include <crypto/crypto.h>

#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/IBlockchainCache.h"
#include "CryptoNoteCore/Transactions/TransactionApi.h"
#include "CryptoNoteCore/Transactions/TransactionUtils.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"
#include "CryptoNoteCore/Transactions/Validation.h"
#include "CryptoNoteCore/BlockchainCache.h"

using Error = CryptoNote::error::TransactionValidationError;

CryptoNote::TransactionValidator::TransactionValidator(BlockVersion _blockVersion, const IBlockchainCache &chain,
                                                       const Currency &currency)
    : m_blockVersion{_blockVersion}, m_chain{chain}, m_currency{currency} {
}

CryptoNote::BlockVersion CryptoNote::TransactionValidator::blockVersion() const {
  return m_blockVersion;
}
const CryptoNote::IBlockchainCache &CryptoNote::TransactionValidator::chain() const {
  return m_chain;
}
const CryptoNote::Currency &CryptoNote::TransactionValidator::currency() const {
  return m_currency;
}

Xi::Result<CryptoNote::EligibleIndex> CryptoNote::TransactionValidator::doValidate(
    const CachedTransaction &transaction) const {
  EligibleIndex eligibleIndex{};

  TransferValidationContext context{currency(), chain()};
  context.minimumMixin = currency().mixinLowerBound(blockVersion());
  context.maximumMixin = currency().mixinUpperBound(blockVersion());
  fillContext(context);

  TransferValidationState state{};
  TransferValidationCache cache{};

  if (const auto ec = preValidateTransfer(transaction, context, cache, state)) {
    return Xi::makeError(ec);
  }

  if (checkIfAnySpent(state.usedKeyImages)) {
    return Xi::makeError(make_error_code(Error::INPUT_KEYIMAGE_ALREADY_SPENT));
  }

  TransferValidationInfo info{};
  info.outputs = chain().extractKeyOutputs(state.globalOutputIndicesUsed, context.previousBlockIndex);

  if (const auto ec = postValidateTransfer(transaction, context, cache, info)) {
    return Xi::makeError(ec);
  }

  for (const auto &amountRefs : info.outputs) {
    for (const auto &iOutputRef : amountRefs.second) {
      const auto unlockTime = iOutputRef.second.unlockTime;
      if (currency().isLockedBasedOnBlockIndex(unlockTime)) {
        eligibleIndex.Height = std::max(eligibleIndex.Height, static_cast<uint32_t>(unlockTime));
      } else if (currency().isLockedBasedOnTimestamp(unlockTime)) {
        eligibleIndex.Timestamp = std::max(eligibleIndex.Timestamp, unlockTime);
      }
    }
  }

  const auto unlock = transaction.getTransaction().unlockTime;
  if (unlock > 0) {
    if (currency().isLockedBasedOnBlockIndex(unlock)) {
      const auto maxUnlockDiff =
          currency().transaction(blockVersion()).futureUnlockLimit() / currency().coin().blockTime() + 1;
      if (maxUnlockDiff < unlock) {
        eligibleIndex.Height = std::max(eligibleIndex.Height, static_cast<uint32_t>(unlock - maxUnlockDiff + 1));
      }
    } else {
      const auto maxUnlockDiff = currency().transaction(blockVersion()).futureUnlockLimit();
      eligibleIndex.Timestamp = std::max(eligibleIndex.Timestamp, unlock - maxUnlockDiff + 1);
    }
  }

  return Xi::success(eligibleIndex);
}
