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

#include "CryptoNoteCore/Transactions/TransactionPriortiyComparator.h"

#include <Common/int-util.h>

#include "CryptoNoteCore/Transactions/PendingTransactionInfo.h"

bool CryptoNote::TransactionPriorityComparator::operator()(const CryptoNote::PendingTransactionInfo& lhs,
                                                           const CryptoNote::PendingTransactionInfo& rhs) const {
  const CachedTransaction& left = lhs.transaction();
  const CachedTransaction& right = rhs.transaction();

  // price(lhs) = lhs.fee / lhs.blobSize
  // price(lhs) > price(rhs) -->
  // lhs.fee / lhs.blobSize > rhs.fee / rhs.blobSize -->
  // lhs.fee * rhs.blobSize > rhs.fee * lhs.blobSize
  uint64_t lhs_hi, lhs_lo = mul128(left.getTransactionFee(), right.getBlobSize(), &lhs_hi);
  uint64_t rhs_hi, rhs_lo = mul128(right.getTransactionFee(), left.getBlobSize(), &rhs_hi);

  return
      // prefer more profitable transactions
      (lhs_hi > rhs_hi) || (lhs_hi == rhs_hi && lhs_lo > rhs_lo) ||
      // prefer smaller
      (lhs_hi == rhs_hi && lhs_lo == rhs_lo && left.getBlobSize() < right.getBlobSize()) ||
      // prefer older
      (lhs_hi == rhs_hi && lhs_lo == rhs_lo && left.getBlobSize() == right.getBlobSize() &&
       lhs.receiveTime() < rhs.receiveTime());
}
