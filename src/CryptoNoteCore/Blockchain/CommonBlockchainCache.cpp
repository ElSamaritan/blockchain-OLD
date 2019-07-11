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

#include "CryptoNoteCore/Blockchain/CommonBlockchainCache.h"

#include <ctime>
#include <cassert>

#include <Xi/Exceptions.hpp>

using namespace Xi;
using Logging::Level;

CryptoNote::CommonBlockchainCache::CommonBlockchainCache(Logging::ILogger& logger, const Currency& currency)
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

std::vector<uint64_t> CryptoNote::CommonBlockchainCache::getLastTimestamps(size_t count, uint32_t blockIndex,
                                                                           UseGenesis _) const {
  return getLastUnits(count, blockIndex, _, [](const CachedBlockInfo& info) { return info.timestamp; });
}

std::vector<uint64_t> CryptoNote::CommonBlockchainCache::getLastBlocksSizes(size_t count, uint32_t blockIndex,
                                                                            UseGenesis _) const {
  return getLastUnits(count, blockIndex, _, [](const auto& info) { return info.blobSize; });
}

std::vector<uint64_t> CryptoNote::CommonBlockchainCache::getLastCumulativeDifficulties(size_t count,
                                                                                       uint32_t blockIndex,
                                                                                       UseGenesis _) const {
  return getLastUnits(count, blockIndex, _, [](const CachedBlockInfo& info) { return info.cumulativeDifficulty; });
}

std::vector<uint64_t> CryptoNote::CommonBlockchainCache::getLastTimestamps(size_t count) const {
  return getLastTimestamps(count, getTopBlockIndex(), UseGenesis{false});
}

std::vector<uint64_t> CryptoNote::CommonBlockchainCache::getLastBlocksSizes(size_t count) const {
  return getLastBlocksSizes(count, getTopBlockIndex(), UseGenesis{false});
}

std::vector<uint64_t> CryptoNote::CommonBlockchainCache::getLastCumulativeDifficulties(size_t count) const {
  return getLastCumulativeDifficulties(count, getTopBlockIndex(), UseGenesis{false});
}

uint64_t CryptoNote::CommonBlockchainCache::getCurrentCumulativeDifficulty() const {
  return getCurrentCumulativeDifficulty(getTopBlockIndex());
}

uint64_t CryptoNote::CommonBlockchainCache::getCurrentCumulativeDifficulty(uint32_t blockIndex) const {
  auto diffs = getLastCumulativeDifficulties(1, blockIndex, UseGenesis{true});
  exceptional_if<NotFoundError>(diffs.empty());
  return diffs.back();
}

uint64_t CryptoNote::CommonBlockchainCache::getDifficultyForNextBlock(BlockVersion version) const {
  return getDifficultyForNextBlock(version, getTopBlockIndex());
}
uint64_t CryptoNote::CommonBlockchainCache::getDifficultyForNextBlock(BlockVersion version, uint32_t blockIndex) const {
  exceptional_if<InvalidArgumentError>(blockIndex > getTopBlockIndex());
  const uint64_t blockWindow = m_currency.difficultyBlocksCountByVersion(version);
  auto timestamps = getLastTimestamps(blockWindow, blockIndex, UseGenesis{false});
  auto commulativeDifficulties = getLastCumulativeDifficulties(blockWindow, blockIndex, UseGenesis{false});
  return m_currency.nextDifficulty(version, blockIndex, std::move(timestamps), std::move(commulativeDifficulties));
}

uint64_t CryptoNote::CommonBlockchainCache::getAlreadyGeneratedCoins() const {
  return getAlreadyGeneratedCoins(getTopBlockIndex());
}

uint64_t CryptoNote::CommonBlockchainCache::getAlreadyGeneratedCoins(uint32_t blockIndex) const {
  auto coins =
      getLastUnits(1, blockIndex, UseGenesis{true}, [](const auto& info) { return info.alreadyGeneratedCoins; });
  exceptional_if<NotFoundError>(coins.empty());
  return coins.back();
}

uint64_t CryptoNote::CommonBlockchainCache::getAlreadyGeneratedTransactions(uint32_t blockIndex) const {
  auto transactions =
      getLastUnits(1, blockIndex, UseGenesis{true}, [](const auto& info) { return info.alreadyGeneratedTransactions; });
  exceptional_if<NotFoundError>(transactions.empty());
  return transactions.back();
}

uint64_t CryptoNote::CommonBlockchainCache::getCurrentBlockSize() const {
  return getCurrentBlockSize(getTopBlockIndex());
}

uint64_t CryptoNote::CommonBlockchainCache::getCurrentBlockSize(uint32_t blockIndex) const {
  const auto sizes = getLastBlocksSizes(1, blockIndex, UseGenesis{true});
  exceptional_if<NotFoundError>(sizes.empty());
  return sizes.back();
}

bool CryptoNote::CommonBlockchainCache::isTransactionSpendTimeUnlockedByBlockIndex(uint64_t unlockTime,
                                                                                   uint32_t blockIndex) const {
  assert(unlockTime <= BlockHeight::max().toIndex());
  return m_currency.isUnlockSatisfied(unlockTime, blockIndex, 0);
}

bool CryptoNote::CommonBlockchainCache::isTransactionSpendTimeUnlockedByTimestamp(uint64_t unlockTime,
                                                                                  uint32_t blockIndex) const {
  assert(unlockTime > BlockHeight::max().toIndex());
  assert(blockIndex < getTopBlockIndex() + 1);

  if (blockIndex > getTopBlockIndex()) {
    m_logger(Level::Fatal)
        << "Cannot query children for transaction spend time unlock timestamp, this would be ambiguous.";
    return false;
  }

  if (blockIndex < getStartBlockIndex()) {
    auto parent = getParent();
    if (parent != nullptr) {
      return parent->isTransactionSpendTimeUnlocked(unlockTime, blockIndex);
    } else {
      m_logger(Level::Fatal) << "Unable to load block timestamp, block not contained by this and no parent given.";
      return false;
    }
  }

  uint64_t timestamp = 0;
  time_t localTime = time(nullptr);
  if (localTime < 0) {
    m_logger(Level::Fatal) << "Unable to retrieve unix timestamp. Expected > 0, actual: " << timestamp;
    return false;
  } else {
    timestamp = static_cast<uint64_t>(localTime);
  }

  return m_currency.isUnlockSatisfied(unlockTime, blockIndex, timestamp);
}
