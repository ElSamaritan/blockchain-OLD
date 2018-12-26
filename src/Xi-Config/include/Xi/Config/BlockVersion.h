﻿/* ============================================================================================== *
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

#pragma once

#include <cinttypes>

#include "Xi/Config/_Impl/BeginBlockVersion.h"

namespace Xi {
namespace Config {
namespace BlockVersion {
inline constexpr uint8_t minorVersionNoVotingIndicator() { return 0; }
inline constexpr uint8_t minorVersionVotingIndicator() { return 1; }
inline constexpr bool validateMinorVersion(uint8_t minorVersion) {
  return minorVersion == minorVersionNoVotingIndicator() || minorVersion == minorVersionVotingIndicator();
}
}  // namespace BlockVersion
}  // namespace Config
}  // namespace Xi

// clang-format off
//                        (_Index, _Height,  _Version,  _IsFork)
MakeBlockVersionCheckpoint(     0,       0,         1,    false)
MakeBlockVersionCheckpoint(     1,       1,         2,    false)
MakeBlockVersionCheckpoint(     2,       2,         3,    false)
MakeBlockVersionCheckpoint(     3,       3,         4,    false)
MakeBlockVersionCheckpoint(     4,    1000,         5,    false)
MakeBlockVersionCheckpoint(     5,    3000,         6,     true)
MakeBlockVersionCheckpoint(     6,   10000,         7,    false)
MakeBlockVersionCheckpoint(     7,  259200,         8,    false)
MakeBlockVersionCheckpoint(     8,  518400,         9,    false)
// clang-format on

#define CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX 8

#include "Xi/Config/_Impl/EndBlockVersion.h"