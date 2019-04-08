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

#include <cinttypes>
#include <algorithm>

#include <Xi/Byte.h>
#include <Xi/Algorithm/Math.h>

#include "Xi/Config/Time.h"

namespace Xi {
namespace Config {
namespace Limits {

/* +++ Block Sizes +++ */
inline constexpr uint32_t maximumHeight() { return (uint32_t)500_M; }
inline constexpr uint64_t maximumTransactionSize() { return 1_T; }

inline constexpr uint64_t maximumBlockBlobSize() { return 10_MB; }
inline constexpr uint64_t maximumBlockExtraSize() { return 140_kB; }

inline constexpr uint64_t initialBlockBlobSizeLimit() { return 200_kB; }
inline constexpr std::chrono::seconds blockBlobSizeGrowthTimeWindow() { return 24_h; }
inline constexpr uint64_t blockBlobSizeGrowthIncrementor() { return 5_kB; }
inline constexpr uint64_t blockBlobCoinbaseReservedSize() { return 600_Bytes; }

inline constexpr uint64_t blockBlobSizeGrowthNumerator() { return blockBlobSizeGrowthIncrementor(); }
inline constexpr uint64_t blockBlobSizeGrowthDenominator() {
  return static_cast<uint64_t>(std::chrono::seconds{blockBlobSizeGrowthTimeWindow()}.count()) /
         Xi::Config::Time::blockTimeSeconds();
}

/* +++ RPC +++ */
inline constexpr uint64_t maximumRPCBlocksQueryCount() { return 500; }

/* +++ Transactions +++ */
inline constexpr uint32_t maximumBlockWindowForLockedTransation() { return 1; }
inline constexpr std::chrono::seconds maximumTimeWindowForLockedTransation() {
  return maximumBlockWindowForLockedTransation() * Xi::Config::Time::blockTime();
}

inline constexpr std::chrono::seconds maximumTransactionLivetimeSpan() { return 24_h; }
inline constexpr std::chrono::seconds maximumTransactionLivetimeSpanFromAltBlocks() { return 7 * 24_h; }

/*!
 * \brief minimumTransactionLivetimeSpansUntilDeletion is the amount of lifetime spans a transaction must be hold in
 * the transaction pool before it can be deleted.
 *
 * Thus the minimum amount of time a transaction will remain in the transaction pool is:
 *   maximumTransactionLivetimeSpan() * minimumTransactionLivetimeSpansUntilDeletion()
 */
inline constexpr uint64_t minimumTransactionLivetimeSpansUntilDeletion() { return 7; }

inline constexpr uint64_t maximumFusionTransactionSize() { return 32_kB; }
inline constexpr uint64_t minimumFusionTransactionInputCount() { return 12; }
inline constexpr uint64_t minimumFusionTransactionInputOutputRatio() { return 4; }
}  // namespace Limits
}  // namespace Config
}  // namespace Xi
