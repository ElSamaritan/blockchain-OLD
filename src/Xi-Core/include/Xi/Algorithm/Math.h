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

/*!
 * \brief pow computes the to the power of function (base^exponent)
 * \param base the base of the formula
 * \param exponent the exponent of the formula
 * \return base^exponent, 1 if exponent is 0
 */
static inline constexpr uint64_t pow64(uint64_t base, uint64_t exponent) {
  return exponent == 0 ? 1 : base * pow64(base, exponent - 1);
}
}  // namespace Xi

static inline constexpr uint64_t operator"" _k(unsigned long long kilo) { return kilo * Xi::pow(10, 3); }
static inline constexpr uint64_t operator"" _M(unsigned long long mega) { return mega * Xi::pow(10, 6); }
static inline constexpr uint64_t operator"" _G(unsigned long long giga) { return giga * Xi::pow(10, 9); }
static inline constexpr uint64_t operator"" _T(unsigned long long tera) { return tera * Xi::pow(10, 12); }
