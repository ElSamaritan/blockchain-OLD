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

#include "CryptoNoteCore/Transactions/ITransactionValidator.h"

Xi::Result<CryptoNote::TransactionValidationResult> CryptoNote::ITransactionValidator::validate(
    Transaction transaction) const {
  return validate(CachedTransaction{std::move(transaction)});
}

Xi::Result<CryptoNote::TransactionValidationResult> CryptoNote::ITransactionValidator::validate(
    CachedTransaction transaction) const {
  try {
    const auto result = doValidate(transaction);
    if (result.isError()) {
      return result.error();
    } else {
      return Xi::emplaceSuccess<TransactionValidationResult>(std::move(transaction), result.value());
    }
  } catch (...) {
    return Xi::failure(std::current_exception());
  }
}

Xi::Result<CryptoNote::TransactionValidationResult> CryptoNote::ITransactionValidator::updateValidation(
    const CryptoNote::CachedTransaction &transaction) const {
  try {
    const auto result = doValidate(transaction);
    if (result.isError()) {
      return result.error();
    } else {
      return Xi::emplaceSuccess<TransactionValidationResult>(result.value());
    }
  } catch (...) {
    return Xi::failure(std::current_exception());
  }
}
