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

#include <Xi/Utils/ConstExprMath.h>

static inline constexpr uint64_t operator"" _Bytes(unsigned long long bytes) { return bytes; }

static inline constexpr uint64_t operator"" _kB(unsigned long long kiloBytes) { return kiloBytes * 1024; }

static inline constexpr uint64_t operator"" _MB(unsigned long long megaBytes) { return megaBytes * 1024 * 1024; }

static inline constexpr std::chrono::milliseconds operator"" _ms(unsigned long long milliseconds) {
  return std::chrono::milliseconds{milliseconds};
}

static inline constexpr std::chrono::seconds operator"" _s(unsigned long long seconds) {
  return std::chrono::seconds{seconds};
}

static inline constexpr std::chrono::minutes operator"" _m(unsigned long long minutes) {
  return std::chrono::minutes{minutes};
}

static inline constexpr std::chrono::hours operator"" _h(unsigned long long hours) { return std::chrono::hours{hours}; }

static inline constexpr std::chrono::hours operator"" _d(unsigned long long days) {
  return std::chrono::hours{days * 24};
}

static inline constexpr uint64_t operator"" _k(unsigned long long kilo) { return kilo * Xi::pow(10, 3); }

static inline constexpr uint64_t operator"" _M(unsigned long long mega) { return mega * Xi::pow(10, 6); }

static inline constexpr uint64_t operator"" _T(unsigned long long giga) { return giga * Xi::pow(10, 9); }

static inline constexpr uint64_t operator"" _G(unsigned long long tera) { return tera * Xi::pow(10, 12); }
