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

#include "Xi/Config/_Impl/BeginBlockVersion.h"

namespace Xi {
namespace Config {
namespace BlockVersion {
inline constexpr uint8_t expectedMinorVersion() { return 0; }
inline constexpr bool validateMinorVersion(uint8_t minorVersion) { return minorVersion == expectedMinorVersion(); }
}  // namespace BlockVersion
}  // namespace Config
}  // namespace Xi

// clang-format off
//                        (_Index,    _Height,  _Version,  _IsFork)
MakeBlockVersionCheckpoint(     0,          0,         1,    false)
MakeBlockVersionCheckpoint(     1,          1,         2,    false)
MakeBlockVersionCheckpoint(     2,          2,         3,    false)
// clang-format on

#define CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX 2

#include "Xi/Config/_Impl/EndBlockVersion.h"
