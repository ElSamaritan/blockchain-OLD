﻿/* ============================================================================================== *
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

#include <Xi/Result.h>

#include "CryptoNoteCore/Transactions/CachedTransaction.h"
#include "CryptoNoteCore/Transactions/TransactionValidationResult.h"

namespace CryptoNote {

class ITransactionValidator {
 public:
  ITransactionValidator() = default;
  virtual ~ITransactionValidator() = default;

  Xi::Result<TransactionValidationResult> validate(Transaction transaction) const;
  Xi::Result<TransactionValidationResult> validate(CachedTransaction transaction) const;

  Xi::Result<TransactionValidationResult> updateValidation(const CachedTransaction& transaction) const;

 protected:
  virtual Xi::Result<TransactionValidationResult::EligibleIndex> doValidate(
      const CachedTransaction& transaction) const = 0;
};

}  // namespace CryptoNote