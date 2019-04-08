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

namespace Xi {
/*!
 * \brief pow computes the to the power of function (base^exponent)
 * \param base the base of the formula
 * \param exponent the exponent of the formula
 * \return base^exponent, 1 if exponent is 0
 */
static inline constexpr uint32_t pow(uint32_t base, uint32_t exponent) {
  return exponent == 0 ? 1 : base * pow(base, exponent - 1);
}
}  // namespace Xi

static inline constexpr uint64_t operator"" _k(uint64_t kilo) { return kilo * Xi::pow(10, 3); }

static inline constexpr uint64_t operator"" _M(uint64_t mega) { return mega * Xi::pow(10, 6); }

static inline constexpr uint64_t operator"" _G(uint64_t giga) { return giga * Xi::pow(10, 9); }

static inline constexpr uint64_t operator"" _T(uint64_t tera) { return tera * Xi::pow(10, 12); }
