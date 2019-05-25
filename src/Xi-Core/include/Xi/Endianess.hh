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

#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>

static uint16_t xi_endianess_swap_16(const uint16_t value) {
  return ((uint16_t)((value & 0x00FFU) << 8) | (uint16_t)((value & 0xFF00U) >> 8));
}
static uint32_t xi_endianess_swap_32(const uint32_t value) {
  return ((uint32_t)((value & 0x000000FFU) << 24) | (uint32_t)((value & 0x0000FF00U) << 8) |
          (uint32_t)((value & 0x00FF0000U) >> 8) | (uint32_t)((value & 0xFF000000U) >> 24));
}
static uint64_t xi_endianess_swap_64(const uint64_t value) {
  return ((uint64_t)((value & 0x00000000000000FFULL) << 56) | (uint64_t)((value & 0x000000000000FF00ULL) << 40) |
          (uint64_t)((value & 0x0000000000FF0000ULL) << 24) | (uint64_t)((value & 0x00000000FF000000ULL) << 8) |
          (uint64_t)((value & 0x000000FF00000000ULL) >> 8) | (uint64_t)((value & 0x0000FF0000000000ULL) >> 24) |
          (uint64_t)((value & 0x00FF000000000000ULL) >> 40) | (uint64_t)((value & 0xFF00000000000000ULL) >> 56));
}

#if defined(XI_COMPILER_ENDIANESS_LITTLE)
static uint16_t xi_endianess_little_16(const uint16_t value) { return value; }
static uint32_t xi_endianess_little_32(const uint32_t value) { return value; }
static uint64_t xi_endianess_little_64(const uint64_t value) { return value; }

static uint16_t xi_endianess_big_16(const uint16_t value) { return xi_endianess_swap_16(value); }
static uint32_t xi_endianess_big_32(const uint32_t value) { return xi_endianess_swap_32(value); }
static uint64_t xi_endianess_big_64(const uint64_t value) { return xi_endianess_swap_64(value); }
#endif

#if defined(XI_COMPILER_ENDIANESS_BIG)
static uint16_t xi_endianess_little_16(const uint16_t value) { return xi_endianess_swap_16(value); }
static uint32_t xi_endianess_little_32(const uint32_t value) { return xi_endianess_swap_32(value); }
static uint64_t xi_endianess_little_64(const uint64_t value) { return xi_endianess_swap_64(value); }

static uint16_t xi_endianess_big_16(const uint16_t value) { return value; }
static uint32_t xi_endianess_big_32(const uint32_t value) { return value; }
static uint64_t xi_endianess_big_64(const uint64_t value) { return value; }
#endif

#if defined(__cplusplus)
}
#endif
