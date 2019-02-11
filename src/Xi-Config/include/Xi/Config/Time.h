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

#include <chrono>
#include <limits>

#include <Xi/Utils/Conversion.h>

#include "Xi/Config/Coin.h"

namespace Xi {
namespace Config {
namespace Time {
constexpr std::chrono::seconds blockTime() { return 1_m; }
constexpr std::chrono::seconds minerRewardUnlockTime() { return 10_m; }

constexpr uint32_t minerRewardUnlockBlocksCount() {
  return static_cast<uint32_t>(std::chrono::seconds{minerRewardUnlockTime()}.count() /
                               std::chrono::seconds{blockTime()}.count());
}
constexpr uint32_t expectedBlocksPerDay() {
  return static_cast<uint32_t>(std::chrono::seconds{std::chrono::hours{24}}.count() /
                               std::chrono::seconds{blockTime()}.count());
}

constexpr uint16_t blockTimeSeconds() { return static_cast<uint16_t>(std::chrono::seconds{blockTime()}.count()); }
constexpr std::chrono::seconds expectedBlockhainUptimeForHeight(uint32_t height) { return height * blockTime(); }

static_assert(std::chrono::seconds{blockTime()}.count() < std::numeric_limits<uint16_t>::max(),
              "Only a maximum of 2^16-1 seconds is allowed as block time.");
static_assert(blockTimeSeconds() > 0, "blockTime must be convert to positve none zero seconds.");
static_assert(minerRewardUnlockBlocksCount() > 0,
              "minerRewardUnlockBlocksCount must be convert to positve none zero amount of blocks.");
}  // namespace Time
}  // namespace Config
}  // namespace Xi

#include "Xi/Config/_Impl/BeginTime.h"

// clang-format off
//                (_Index, _Version, _PastWindow, _FutureLimit)
MakeTimeCheckpoint(     0,        1,          50,         10_m)
MakeTimeCheckpoint(     1,        5,          16,         10_m)
// clang-format on

#define CURRENT_TIME_CHECKPOINT_INDEX 1

#include "Xi/Config/_Impl/EndTime.h"
