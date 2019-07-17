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

#pragma once

#include <chrono>
#include <cinttypes>
#include <string>

#include "Xi/Result.h"

namespace Xi {
namespace Time {

Xi::Result<std::chrono::microseconds> parseDuration(std::string str);

std::string unixToLocalShortString(const uint64_t timestamp);

}  // namespace Time
}  // namespace Xi

inline constexpr std::chrono::nanoseconds operator"" _ns(unsigned long long arg) {
  return std::chrono::nanoseconds{arg};
}
inline constexpr std::chrono::microseconds operator"" _us(unsigned long long arg) {
  return std::chrono::microseconds{arg};
}
inline constexpr std::chrono::milliseconds operator"" _ms(unsigned long long arg) {
  return std::chrono::milliseconds{arg};
}
inline constexpr std::chrono::seconds operator"" _s(unsigned long long arg) {
  return std::chrono::seconds{arg};
}
inline constexpr std::chrono::minutes operator"" _m(unsigned long long arg) {
  return std::chrono::minutes{arg};
}
inline constexpr std::chrono::hours operator"" _h(unsigned long long arg) {
  return std::chrono::hours{arg};
}
