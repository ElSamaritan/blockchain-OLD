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

#include "CryptoNoteCore/Transactions/PoolTransactionValidator.h"

CryptoNote::PoolTransactionValidator::PoolTransactionValidator(const CryptoNote::ITransactionPool &pool,
                                                               uint8_t blockVersion, const IBlockchainCache &chain,
                                                               const Currency &currency)
    : TransactionValidator(blockVersion, chain, currency), m_pool{pool} {}

Xi::Result<CryptoNote::TransactionValidationResult::EligibleIndex> CryptoNote::PoolTransactionValidator::doValidate(
    const CryptoNote::CachedTransaction &transaction) const {
  if (m_pool.containsTransaction(transaction.getTransactionHash())) {
    return Xi::make_error(error::TransactionValidationError::EXISTS_IN_POOL);
  } else if (!currency().isFusionTransaction(transaction.getTransaction(), chain().getTopBlockIndex() + 1) &&
             transaction.getBlobSize() > transactionWeightLimit()) {
    return Xi::make_error(error::TransactionValidationError::TOO_LARGE_FOR_REWARD_ZONE);
  } else {
    return this->TransactionValidator::doValidate(transaction);
  }
}

bool CryptoNote::PoolTransactionValidator::checkIfKeyImageIsAlreadySpent(const Crypto::KeyImage &keyImage) const {
  return chain().checkIfSpent(keyImage) || m_pool.containsKeyImage(keyImage);
}

bool CryptoNote::PoolTransactionValidator::isInCheckpointRange() const { return false; }

bool CryptoNote::PoolTransactionValidator::isFeeInsufficient(const CachedTransaction &transaction) const {
  const uint64_t fee = transaction.getTransactionFee();
  const bool isFusionTransaction =
      fee == 0 && currency().isFusionTransaction(transaction.getTransaction(), chain().getTopBlockIndex() + 1);
  return !isFusionTransaction && fee < currency().minimumFee();
}

uint64_t CryptoNote::PoolTransactionValidator::transactionWeightLimit() const {
  return currency().blockGrantedFullRewardZoneByBlockVersion(blockVersion()) - currency().minerTxBlobReservedSize();
}
