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

#include <tuple>

#include <Xi/Global.h>
#include <Xi/Result.h>

#include "CryptoNoteCore/CryptoNote.h"
#include "CryptoNoteCore/IBlockchainCache.h"
#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/Transactions/ITransactionValidator.h"
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
  TransactionValidator(uint8_t blockVersion, const IBlockchainCache& chain, const Currency& currency);
  ~TransactionValidator() override = default;

  /*!
   * \brief blockVersion the actual or expected block major version for the block containing the transaction
   *
   * The block major version is used to query constraints setup on the currency that change over the evolution over the
   * blockchain.
   *
   * \todo It would be reasonable to check for a range of heights and return eligible block versions. That could help
   * for a fork to accept some transactions that are left in pool. For now once a fork happened all transactions should
   * be validated again with the new blockVersion and conditionally be deleted.
   */
  uint8_t blockVersion() const;

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
  Xi::Result<EligibleIndex> doValidate(
      const CachedTransaction& transaction) const override;

  virtual bool checkIfKeyImageIsAlreadySpent(const Crypto::KeyImage& keyImage) const = 0;
  virtual bool isInCheckpointRange() const = 0;
  virtual bool isFeeInsufficient(const CachedTransaction& transaction) const = 0;

 protected:
  error::TransactionValidationError getErrorCode(ExtractOutputKeysResult e) const;

 private:
  /*!
   * \return true iff the transaction version is not supported in the current context.
   */
  bool hasUnsupportedVersion(const uint8_t version) const;
  bool containsUnsupportedInputTypes(const Transaction& transaction) const;
  bool containsUnsupportedOutputTypes(const Transaction& transaction) const;
  bool containsEmptyOutput(const Transaction& transaction) const;
  bool containsInvalidOutputKey(const std::vector<Crypto::PublicKey>& keys) const;
  bool hasInputOverflow(const Transaction& transaction) const;
  bool hasOutputOverflow(const Transaction& transaction) const;
  bool containsKeyImageDuplicates(const std::vector<Crypto::KeyImage>& keyImages) const;
  bool isExtraTooLarge(const Transaction& transaction) const;

  static bool isInvalidDomainKeyImage(const Crypto::KeyImage& keyImage);
  bool containsInvalidDomainKeyImage(const std::vector<Crypto::KeyImage>& keyImages) const;

  bool containsSpendedKey(const Crypto::KeyImagesSet& keyImages) const;

  /*!
   * \brief extractOutputKeys extracts the used output keys used as input for the transaction
   * \param amount The amount used for the input
   * \param indices The key indices referenced as input
   * \return An error or the public keys of the referenced inputs together with a minimum height/timestamp required for
   * the referenced outputs to be unlocked.
   */
  Xi::Result<std::tuple<std::vector<Crypto::PublicKey>, EligibleIndex>> extractOutputKeys(
      uint64_t amount, const std::vector<uint32_t>& indices) const;

  Xi::Result<EligibleIndex> validateKeyInput(const KeyInput& keyInput, size_t inputIndex,
                                                                          const CachedTransaction& transaction) const;
  Xi::Result<EligibleIndex> validateInputs(const CachedTransaction& transaction) const;

  error::TransactionValidationError validateMixin(const CachedTransaction& transaction) const;

 private:
  uint8_t m_blockVersion;
  const IBlockchainCache& m_chain;
  const Currency& m_currency;
};
}  // namespace CryptoNote
