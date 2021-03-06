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

#include "CryptoNoteCore/Transactions/PoolTransactionValidator.h"

#include "CryptoNoteCore/CryptoNoteTools.h"

CryptoNote::PoolTransactionValidator::PoolTransactionValidator(const CryptoNote::ITransactionPool &pool,
                                                               BlockVersion blockVersion, const IBlockchainCache &chain,
                                                               const Currency &currency)
    : TransactionValidator(blockVersion, chain, currency), m_pool{pool} {
}

Xi::Result<CryptoNote::EligibleIndex> CryptoNote::PoolTransactionValidator::doValidate(
    const CryptoNote::CachedTransaction &transaction) const {
  if (m_pool.containsTransaction(transaction.getTransactionHash())) {
    return Xi::make_error(error::TransactionValidationError::EXISTS_IN_POOL);
  } else if (transaction.getBlobSize() > transactionWeightLimit()) {
    return Xi::make_error(error::TransactionValidationError::TOO_LARGE_FOR_REWARD_ZONE);
  } else {
    return this->TransactionValidator::doValidate(transaction);
  }
}

bool CryptoNote::PoolTransactionValidator::checkIfAnySpent(const Crypto::KeyImageSet &keyImages) const {
  XI_RETURN_EC_IF(chain().checkIfAnySpent(keyImages, chain().getTopBlockIndex()), true);
  for (const auto &keyImage : keyImages) {
    XI_RETURN_EC_IF(m_pool.containsKeyImage(keyImage), true);
  }
  XI_RETURN_SC(false);
}

void CryptoNote::PoolTransactionValidator::fillContext(CryptoNote::TransferValidationContext &context) const {
  context.blockVersion = blockVersion();
  context.previousBlockIndex = chain().getTopBlockIndex();
  context.timestamp = static_cast<uint64_t>(time(nullptr));
  context.inCheckpointRange = false;
}

uint64_t CryptoNote::PoolTransactionValidator::transactionWeightLimit() const {
  return currency().maxTxSize(blockVersion());
}
