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

#include <cinttypes>

#include <Xi/Global.hh>

namespace CryptoNote {
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
}  // namespace CryptoNote
