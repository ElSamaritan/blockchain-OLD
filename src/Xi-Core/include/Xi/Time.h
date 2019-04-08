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
#include <chrono>
#include <string>

#include "Xi/Result.h"

namespace Xi {
namespace Time {

Xi::Result<std::chrono::microseconds> parseDuration(std::string str);

}
}  // namespace Xi

inline std::chrono::nanoseconds operator"" _ns(uint64_t arg) { return std::chrono::nanoseconds{arg}; }
inline std::chrono::microseconds operator"" _us(uint64_t arg) { return std::chrono::microseconds{arg}; }
inline std::chrono::milliseconds operator"" _ms(uint64_t arg) { return std::chrono::milliseconds{arg}; }
inline std::chrono::seconds operator"" _s(uint64_t arg) { return std::chrono::seconds{arg}; }
inline std::chrono::minutes operator"" _m(uint64_t arg) { return std::chrono::minutes{arg}; }
inline std::chrono::hours operator"" _h(uint64_t arg) { return std::chrono::hours{arg}; }
