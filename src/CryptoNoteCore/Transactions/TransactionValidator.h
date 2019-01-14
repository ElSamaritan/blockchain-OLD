/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include <Xi/Global.h>

#include "CryptoNoteCore/CryptoNote.h"
#include "CryptoNoteCore/Transactions/ITransactionValidator.h"
#include "CryptoNoteCore/Transactions/CachedTransaction.h"
#include "CryptoNoteCore/Transactions/TransactionValidationContext.h"
#include "CryptoNoteCore/Transactions/TransactionValidationErrors.h"

namespace CryptoNote {

class TransactionValidator : public ITransactionValidator {
  XI_DELETE_COPY(TransactionValidator);
  XI_DELETE_MOVE(TransactionValidator);

 public:
  TransactionValidator(const TransactionValidationContext& ctx);
  ~TransactionValidator() override = default;

  const TransactionValidationContext& context() const;

 protected:
  TransactionValidationResult doValidate(const Transaction& transaction) const override;

 private:
  TransactionValidationResult makeError(error::TransactionValidationError e) const;

  bool hasUnsupportedVersion(const uint8_t version) const;
  bool containsUnsupportedInputTypes(const Transaction& transaction) const;
  bool containsUnsupportedOutputTypes(const Transaction& transaction) const;
  bool containsEmptyOutput(const Transaction& transaction) const;
  bool containsInvalidOutputKey(const std::vector<Crypto::PublicKey>& keys) const;
  bool hasInputOverflow(const Transaction& transaction) const;
  bool hasOutputOverflow(const Transaction& transaction) const;
  bool containsKeyImageDuplicates(const std::vector<Crypto::KeyImage>& keyImages) const;

  static bool isInvalidDomainKeyImage(const Crypto::KeyImage& keyImage);
  bool containsInvalidDomainKeyImage(const std::vector<Crypto::KeyImage>& keyImages) const;

  bool containsSpendedKey(const Crypto::KeyImage& keyImage) const;
  bool containsSpendedKey(const Crypto::KeyImage& keyImage, uint32_t height) const;
  bool containsSpendedKey(const Crypto::KeyImagesSet& keyImages) const;

  error::TransactionValidationError validateKeyInput(const KeyInput& keyInput, size_t inputIndex,
                                                     const CachedTransaction& transaction) const;
  error::TransactionValidationError validateInputs(const CachedTransaction& transaction) const;

  error::TransactionValidationError validateMixin(const CachedTransaction& transaction) const;

 private:
  const TransactionValidationContext& m_context;
};
}  // namespace CryptoNote
