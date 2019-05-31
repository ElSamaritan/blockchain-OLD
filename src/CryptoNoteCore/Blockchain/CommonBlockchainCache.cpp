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

#include "CryptoNoteCore/Blockchain/CommonBlockchainCache.h"

#include <ctime>
#include <cassert>

using Logging::Level;

CryptoNote::CommonBlockchainCache::CommonBlockchainCache(Logging::ILogger &logger, const Currency &currency)
    : m_logger(logger, "CommonBlockchainCache"), m_currency(currency) {}

bool CryptoNote::CommonBlockchainCache::isTransactionSpendTimeUnlocked(uint64_t unlockTime) const {
  return isTransactionSpendTimeUnlocked(unlockTime, getTopBlockIndex());
}

bool CryptoNote::CommonBlockchainCache::isTransactionSpendTimeUnlocked(uint64_t unlockTime, uint32_t blockIndex) const {
  if (m_currency.isLockedBasedOnBlockIndex(unlockTime)) {
    return isTransactionSpendTimeUnlockedByBlockIndex(unlockTime, blockIndex);
  } else {
    return isTransactionSpendTimeUnlockedByTimestamp(unlockTime, blockIndex);
  }
}

bool CryptoNote::CommonBlockchainCache::isTransactionSpendTimeUnlocked(uint64_t unlockTime, uint32_t blockIndex,
                                                                       uint64_t timestamp) const {
  return m_currency.isUnlockSatisfied(unlockTime, blockIndex, timestamp);
}

bool CryptoNote::CommonBlockchainCache::isTransactionSpendTimeUnlockedByBlockIndex(uint64_t unlockTime,
                                                                                   uint32_t blockIndex) const {
  assert(unlockTime < m_currency.maxBlockHeight());
  return m_currency.isUnlockSatisfied(unlockTime, blockIndex, 0);
}

bool CryptoNote::CommonBlockchainCache::isTransactionSpendTimeUnlockedByTimestamp(uint64_t unlockTime,
                                                                                  uint32_t blockIndex) const {
  assert(unlockTime > m_currency.maxBlockHeight());
  assert(blockIndex < getTopBlockIndex() + 1);

  if (blockIndex > getTopBlockIndex()) {
    m_logger(Level::FATAL)
        << "Cannot query children for transaction spend time unlock timestamp, this would be ambiguous.";
    return false;
  }

  if (blockIndex < getStartBlockIndex()) {
    auto parent = getParent();
    if (parent != nullptr) {
      return parent->isTransactionSpendTimeUnlocked(unlockTime, blockIndex);
    } else {
      m_logger(Level::FATAL) << "Unable to load block timestamp, block not contained by this and no parent given.";
      return false;
    }
  }

  uint64_t timestamp = 0;
  time_t localTime = time(nullptr);
  if (localTime < 0) {
    m_logger(Level::FATAL) << "Unable to retrieve unix timestamp. Expected > 0, actual: " << timestamp;
    return false;
  } else {
    timestamp = static_cast<uint64_t>(localTime);
  }

  return m_currency.isUnlockSatisfied(unlockTime, blockIndex, timestamp);
}
