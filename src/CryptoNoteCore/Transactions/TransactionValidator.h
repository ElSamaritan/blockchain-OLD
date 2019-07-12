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

#include <tuple>

#include <Xi/Global.hh>
#include <Xi/Result.h>

#include "CryptoNoteCore/CryptoNote.h"
#include "CryptoNoteCore/IBlockchainCache.h"
#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/Transactions/ITransactionValidator.h"
#include "CryptoNoteCore/Transactions/Validation.h"
#include "CryptoNoteCore/Transactions/CachedTransaction.h"
#include "CryptoNoteCore/Transactions/TransactionValidationErrors.h"

namespace CryptoNote {

/*!
 * \brief The TransactionValidator class implements common transaction validation that are used for block and pool
 * transactions.
 */
class TransactionValidator : public ITransactionValidator {
  XI_DELETE_COPY(TransactionValidator);
  XI_DELETE_MOVE(TransactionValidator);

 public:
  TransactionValidator(BlockVersion blockVersion, const IBlockchainCache& chain, const Currency& currency);
  ~TransactionValidator() override = default;

  /*!
   * \brief blockVersion the actual or expected block version for the block containing the transaction
   *
   * The block version is used to query constraints setup on the currency that change over the evolution over the
   * blockchain.
   *
   * \todo It would be reasonable to check for a range of heights and return eligible block versions. That could help
   * for a fork to accept some transactions that are left in pool. For now once a fork happened all transactions should
   * be validated again with the new blockVersion and conditionally be deleted.
   */
  BlockVersion blockVersion() const;

  /*!
   * \brief chain is the state of the blockchain the transaction will be validated against
   * \return a leaf of the blockchain
   */
  const IBlockchainCache& chain() const;

  /*!
   * \brief currency The currency configuration of the blockchain
   */
  const Currency& currency() const;

 protected:
  Xi::Result<EligibleIndex> doValidate(const CachedTransaction& transaction) const override;

 protected:
  [[nodiscard]] virtual bool checkIfAnySpent(const Crypto::KeyImageSet& keyImages) const = 0;
  virtual void fillContext(TransferValidationContext& context) const = 0;

 private:
  BlockVersion m_blockVersion;
  const IBlockchainCache& m_chain;
  const Currency& m_currency;
};
}  // namespace CryptoNote
