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

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Global.h>
#include <Xi/Error.h>
#include <Xi/Result.h>

#include <crypto/CryptoTypes.h>

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
   * \brief The EligibleIndex struct contains the minimum height and timestamp of a block required for the transactions
   * to be valid.
   */
  struct EligibleIndex {
    static const EligibleIndex Always;  ///< Has no constraints and will always be valid.
    static const EligibleIndex Never;   ///< Has maximum constraints and will never be valid.

    /*!
     * \brief lowerBound computes the least restrictive index satisfying both indices.
     * \param lhs The first index to satisfy
     * \param rhs The second index to satisfy
     * \return An index if satisfied also satisfies both indices provided
     */
    static EligibleIndex lowerBound(const EligibleIndex& lhs, const EligibleIndex& rhs);

    uint32_t Height;     ///< The minimum blockchain height for all transaction inputs to be valid.
    uint64_t Timestamp;  ///< The minimum timestamp for all all transaciton inputs to be valid.

    /*!
     * \brief EligibleIndex constructs a default index with no requirements (Height = 0, Teimstamp = 0)).
     */
    EligibleIndex();

    /*!
     * \brief EligibleIndex construct an index emplacing the given values.
     * \param height Minimum height for transaction inputs to be unlocked.
     * \param timestamp Minimum timestamp for transaction inputs to be unlocked.
     */
    EligibleIndex(uint32_t height, uint64_t timestamp);
    XI_DEFAULT_COPY(EligibleIndex);
    XI_DEFAULT_MOVE(EligibleIndex);
    ~EligibleIndex() = default;

    /*!
     * \brief isSatisfiedByIndex checks if a given index is far enough in the blockchain such that all inputs are
     * unlocked.
     * \param index The current eligible index to check against.
     * \return True if the index is eligible lower than the given index otherwise false.
     *
     * Eligible lower means height and timestamp are not greater than in the given index.
     */
    bool isSatisfiedByIndex(const EligibleIndex& index) const;
  };

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
