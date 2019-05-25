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

#pragma once

#include <vector>
#include <unordered_set>

#include <Xi/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>
#include <Xi/Error.h>
#include <Xi/Result.h>

#include <crypto/CryptoTypes.h>

#include "CryptoNoteCore/EligibleIndex.h"
#include "CryptoNoteCore/Transactions/CachedTransaction.h"
#include "CryptoNoteCore/Transactions/TransactionValidationErrors.h"

namespace CryptoNote {
/*!
 * \brief The TransactionValidationResult class encapsulates the state of multiple transactions being validated
 *
 * Dursing the validation of a transaction it is not always possible to conclude if its valid or not. This is a result
 * of transactions unlock time. If a transactions output is used as an input it must be unlocked once the unlock time,
 * encoded as timestamp or block index, is passed. The caller of the validator is responsible to check if the
 * requirements are met after.
 *
 * For a block transaction this is straigt forward as it includes height and timestamp but for transactions pushed to
 * a pool the daemon must consider if it is reasonable to queue the transaction already.
 *
 * \attention Filling a block template from the transaction pool should consider the timestamp in the block template
 * in order to create a valid block.
 */
class TransactionValidationResult {
 public:
  /*!
   * \brief TransactionValidationResult constructs a validation result without associated transaction
   * \param index The minimum blockchain state for the transaction to be valid
   *
   * This result will be created if you use the updateValidation.
   */
  explicit TransactionValidationResult(EligibleIndex index);
  /*!
   * \brief TransactionValidationResult construct a result with a valid transaction if the index requirements are
   * fullfilled.
   * \param transaction The validated transaction.
   * \param index The minimum blockchain state for the transaction to be valid
   */
  explicit TransactionValidationResult(CachedTransaction&& transaction, EligibleIndex index);
  XI_DEFAULT_COPY(TransactionValidationResult);
  XI_DEFAULT_MOVE(TransactionValidationResult);
  ~TransactionValidationResult() = default;

  /*!
   * \brief transactions A collection of transactions that have been validated
   * \return Collection of transactions.
   */
  const std::vector<CachedTransaction>& transactions() const;

  /*!
   * \brief eligibleIndex Queries minimum blockchain height/timestamp required by the transactions.
   * \return Minimum height and timestamp required such that all transactions are valid.
   */
  EligibleIndex eligibleIndex() const;

  /*!
   * \brief emplace Merges another valdiation reuslt into this result, may returns an error if they are not compatible
   * \param other Another validation result with differen transactions.
   * \return An error if the transactions encoded in both are not compatible otherwise nothing
   *
   * If both results are not compatible with each other the state of this transaction will not change.
   */
  Xi::Result<void> emplace(TransactionValidationResult&& other);

 private:
  std::vector<CachedTransaction> m_transactions;     ///< all transactions validated
  EligibleIndex m_elgibileIndex;                     ///< minimum index of the blockchain in order to be satisfied
  std::unordered_set<Crypto::KeyImage> m_keyImages;  ///< caches used key images for faster merge validations
};
}  // namespace CryptoNote
