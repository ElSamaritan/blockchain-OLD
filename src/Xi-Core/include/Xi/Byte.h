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
#include <vector>
#include <array>

#include "Xi/Algorithm/Math.h"

namespace Xi {
using Byte = uint8_t;
using ByteVector = std::vector<uint8_t>;

template <size_t _Size>
using ByteArray = std::array<Byte, _Size>;
}  // namespace Xi

static inline constexpr uint64_t operator"" _Bytes(uint64_t bytes) { return bytes; }

static inline constexpr uint64_t operator"" _kB(uint64_t kiloBytes) { return kiloBytes * 1024; }

static inline constexpr uint64_t operator"" _MB(uint64_t megaBytes) { return megaBytes * 1024 * 1024; }