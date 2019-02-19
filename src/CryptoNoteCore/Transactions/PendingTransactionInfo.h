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

#include <cinttypes>
#include <chrono>

#include <Xi/Global.h>

#include <crypto/CryptoTypes.h>

#include "CryptoNoteCore/Time/Time.h"
#include "CryptoNoteCore/Transactions/CachedTransaction.h"
#include "CryptoNoteCore/Transactions/TransactionValidationResult.h"

namespace CryptoNote {

/*!
 * \brief The PendingTransactionInfo class stores information about a pending transaction in the pool.
 */
class PendingTransactionInfo {
 public:
  PendingTransactionInfo(CachedTransaction tx, EligibleIndex index, PosixTimestamp time);
  XI_DEFAULT_COPY(PendingTransactionInfo);
  XI_DEFAULT_MOVE(PendingTransactionInfo);
  ~PendingTransactionInfo() = default;

  /*!
   * \brief transaction Content of the pending transaction.
   */
  const CachedTransaction& transaction() const;

  /*!
   * \brief eligibleIndex The minimum blockchain index for the transaction to be eligibale to be mined.
   */
  EligibleIndex eligibleIndex() const;

  /*!
   * \brief receiveTime UTC time the transaction has been recieved.
   * \return UTC timestamp
   */
  PosixTimestamp receiveTime() const;

 private:
  std::shared_ptr<CachedTransaction> m_transaction;
  EligibleIndex m_eligibleIndex;
  PosixTimestamp m_receiveTime;
};

}  // namespace CryptoNote
