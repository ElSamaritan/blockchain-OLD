﻿#pragma once

#include <cinttypes>
#include <algorithm>

#include <Xi/Utils/Conversion.h>

#include "config/Time.h"

namespace CryptoNote {
namespace Config {
namespace Limits {

/* +++ Block Sizes +++ */
inline constexpr uint32_t maximumHeight() { return (uint32_t)500_M; }
inline constexpr uint64_t maximumTransactionSize() { return 1_T; }

inline constexpr uint64_t maximumBlockBlobSize() { return 10_MB; }
inline constexpr uint64_t maximumBlockExtraSize() { return 140_kB; }

inline constexpr uint64_t initialBlockBlobSizeLimit() { return 200_kB; }
inline constexpr std::chrono::seconds blockBlobSizeGrowthTimeWindow() { return 24_h; }
inline constexpr uint64_t blockBlobSizeGrowthIncrementor() { return 100_kB; }
inline constexpr uint64_t blockBlobCoinbaseReservedSize() { return 600_Bytes; }

inline constexpr uint64_t blockBlobSizeGrowthNumerator() { return blockBlobSizeGrowthIncrementor(); }
inline constexpr uint64_t blockBlobSizeGrowthDenominator() {
  return static_cast<uint64_t>(std::chrono::seconds{blockBlobSizeGrowthTimeWindow()}.count()) /
         CryptoNote::Config::Time::blockTimeSeconds();
}

/* +++ Voting +++ */
inline constexpr uint8_t upgradeVotingThreshold() { return 90; }
inline constexpr uint32_t upgradeVotingWindow() { return Time::expectedBlocksPerDay(); }
inline constexpr uint32_t upgradeWindow() { return Time::expectedBlocksPerDay(); }

static_assert(upgradeVotingWindow() > 1u, "Bad upgrade votin window.");
static_assert(upgradeVotingThreshold() > 0u && upgradeVotingThreshold() <= 100u, "Bad upgrade voting threshold (%).");

/* +++ RPC +++ */
inline constexpr uint64_t maximumRPCBlocksQueryCount() { return 500; }

/* +++ Transactions +++ */
inline constexpr uint32_t maximumBlockWindowForLockedTransation() { return 1; }
inline constexpr std::chrono::seconds maximumTimeWindowForLockedTransation() {
  return maximumBlockWindowForLockedTransation() * CryptoNote::Config::Time::blockTime();
}

inline constexpr std::chrono::seconds maximumTransactionLivetimeSpan() { return 1_d; }
inline constexpr std::chrono::seconds maximumTransactionLivetimeSpanFromAltBlocks() { return 7_d; }

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
}  // namespace CryptoNote
