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

#include "CryptoNoteCore/Transactions/KeyImagesMap.h"

#include <algorithm>
#include <iterator>

CryptoNote::KeyImagesMap::KeyImagesMap(Logging::ILogger &logger) : m_logger{logger, "TxPool"} {}

CryptoNote::KeyImagesMap::~KeyImagesMap() {}

Crypto::KeyImagesSet CryptoNote::KeyImagesMap::keyImages() const {
  XI_CONCURRENT_LOCK_READ(m_mutationLock);
  Crypto::KeyImagesSet reval;
  std::transform(m_keyImages.begin(), m_keyImages.end(), std::inserter(reval, reval.begin()),
                 [](const auto &pair) { return pair.first; });
  return reval;
}

std::unordered_set<CryptoNote::KeyImagesMap::transaction_id_t> CryptoNote::KeyImagesMap::transactionsByKeyImage(
    const CryptoNote::KeyImagesMap::key_image_t &keyImage) const {
  XI_CONCURRENT_LOCK_READ(m_mutationLock);
  auto search = m_keyImages.find(keyImage);
  if (search == m_keyImages.end())
    return std::unordered_set<transaction_id_t>{};
  else
    return search->second;
}

bool CryptoNote::KeyImagesMap::contains(const CryptoNote::KeyImagesMap::key_image_t &keyImage) const {
  XI_CONCURRENT_LOCK_READ(m_mutationLock);
  return m_keyImages.find(keyImage) != m_keyImages.end();
}

bool CryptoNote::KeyImagesMap::addTransactionInputs(const CryptoNote::KeyImagesMap::transaction_id_t &txId,
                                                    const CryptoNote::Transaction &tx) {
  XI_CONCURRENT_LOCK_WRITE(m_mutationLock);
  (void)txId;
  for (const auto &input : tx.inputs) {
    if (input.type() != typeid(KeyInput)) continue;
  }
  return true;
}
