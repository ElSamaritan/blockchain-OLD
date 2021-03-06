﻿/* ============================================================================================== *
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

#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>

#include "CryptoNoteCore/IBlockchainCache.h"
#include "CryptoNoteCore/Currency.h"

namespace CryptoNote {
/*!
 * \brief The CommonBlockchainCache class partially implements a blockchain refactoring commonly shared
 * procedures of the in memory cache and database cache out.
 */
class CommonBlockchainCache : public IBlockchainCache {
 protected:
  CommonBlockchainCache(Logging::ILogger& logger, const Currency& currency);

 public:
  virtual ~CommonBlockchainCache() override = default;

  // ------------------------------------------ IBlockchainCache ------------------------------------------------------
  [[nodiscard]] bool isTransactionSpendTimeUnlocked(uint64_t unlockTime) const override;
  [[nodiscard]] bool isTransactionSpendTimeUnlocked(uint64_t unlockTime, uint32_t blockIndex) const override;
  [[nodiscard]] bool isTransactionSpendTimeUnlocked(uint64_t unlockTime, uint32_t blockIndex,
                                                    uint64_t timestamp) const override;

  [[nodiscard]] std::vector<uint64_t> getLastTimestamps(size_t count, uint32_t blockIndex, UseGenesis) const override;
  [[nodiscard]] std::vector<uint64_t> getLastBlocksSizes(size_t count, uint32_t blockIndex, UseGenesis) const override;
  [[nodiscard]] std::vector<uint64_t> getLastCumulativeDifficulties(size_t count, uint32_t blockIndex,
                                                                    UseGenesis) const override;

  [[nodiscard]] std::vector<uint64_t> getLastTimestamps(size_t count) const override;
  [[nodiscard]] std::vector<uint64_t> getLastBlocksSizes(size_t count) const override;
  [[nodiscard]] std::vector<uint64_t> getLastCumulativeDifficulties(size_t count) const override;

  [[nodiscard]] uint64_t getCurrentBlockSize() const override;
  [[nodiscard]] uint64_t getCurrentBlockSize(uint32_t blockIndex) const override;

  [[nodiscard]] uint64_t getCurrentTimestamp() const override;
  [[nodiscard]] uint64_t getCurrentTimestamp(uint32_t blockIndex) const override;

  [[nodiscard]] uint64_t getCurrentCumulativeDifficulty() const override;
  [[nodiscard]] uint64_t getCurrentCumulativeDifficulty(uint32_t blockIndex) const override;

  [[nodiscard]] uint64_t getDifficultyForNextBlock(BlockVersion version) const override;
  [[nodiscard]] uint64_t getDifficultyForNextBlock(BlockVersion version, uint32_t blockIndex) const override;

  [[nodiscard]] uint64_t getAlreadyGeneratedCoins() const override;
  [[nodiscard]] uint64_t getAlreadyGeneratedCoins(uint32_t blockIndex) const override;
  [[nodiscard]] uint64_t getAlreadyGeneratedTransactions(uint32_t blockIndex) const override;
  // ------------------------------------------ IBlockchainCache ------------------------------------------------------

 private:
  [[nodiscard]] bool isTransactionSpendTimeUnlockedByBlockIndex(uint64_t unlockTime, uint32_t blockIndex) const;
  [[nodiscard]] bool isTransactionSpendTimeUnlockedByTimestamp(uint64_t unlockTime, uint32_t blockIndex) const;

 private:
  Logging::LoggerRef m_logger;
  const Currency& m_currency;
};
}  // namespace CryptoNote
