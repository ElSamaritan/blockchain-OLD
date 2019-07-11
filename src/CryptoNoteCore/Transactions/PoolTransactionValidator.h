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

#include "CryptoNoteCore/IUpgradeManager.h"
#include "CryptoNoteCore/Transactions/ITransactionPool.h"
#include "CryptoNoteCore/Transactions/TransactionValidator.h"

namespace CryptoNote {
class PoolTransactionValidator : public TransactionValidator {
 public:
  PoolTransactionValidator(const ITransactionPool& pool, BlockVersion blockVersion, const IBlockchainCache& chain,
                           const Currency& currency);

 protected:
  Xi::Result<EligibleIndex> doValidate(const CachedTransaction& transaction) const override;

  bool checkIfKeyImageIsAlreadySpent(const Crypto::KeyImage& keyImage) const override;
  bool isInCheckpointRange() const override;

 private:
  uint64_t transactionWeightLimit() const;

 private:
  const ITransactionPool& m_pool;
};

}  // namespace CryptoNote
