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

#include "CryptoNoteCore/Transactions/TransactionValidationResult.h"

#include <utility>
#include <limits>

CryptoNote::TransactionValidationResult::TransactionValidationResult(CryptoNote::EligibleIndex index)
    : m_transactions{}, m_elgibileIndex{index} {}

CryptoNote::TransactionValidationResult::TransactionValidationResult(CryptoNote::CachedTransaction &&transaction,
                                                                     EligibleIndex index)
    : m_transactions{}, m_elgibileIndex{index} {
  for (const auto &keyImage : transaction.getKeyImagesSet()) {
    m_keyImages.insert(keyImage);
  }
  m_transactions.emplace_back(std::move(transaction));
}

CryptoNote::EligibleIndex CryptoNote::TransactionValidationResult::eligibleIndex() const { return m_elgibileIndex; }

Xi::Result<void> CryptoNote::TransactionValidationResult::emplace(CryptoNote::TransactionValidationResult &&other) {
  for (const auto &keyImage : other.m_keyImages) {
    if (m_keyImages.find(keyImage) != m_keyImages.end()) {
      return Xi::make_error(CryptoNote::error::TransactionValidationError::INPUT_IDENTICAL_KEYIMAGES);
    }
  }
  for (const auto &keyImage : other.m_keyImages) {
    m_keyImages.insert(keyImage);
  }
  for (auto &transaction : other.m_transactions) {
    m_transactions.emplace_back(std::move(transaction));
  }
  m_elgibileIndex = EligibleIndex::lowerBound(m_elgibileIndex, other.m_elgibileIndex);
  return Xi::success();
}
