﻿/* ============================================================================================== *
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

#include "Xi/Endianess/Identity.hh"
#include "Xi/Endianess/Swap.hh"

#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
static inline uint16_t xi_endianess_little_u16(const uint16_t value) {
#if defined(XI_COMPILER_ENDIANESS_LITTLE)
  return xi_endianess_identity_u16(value);
#else
  return xi_endianess_swap_u16(value);
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
static inline uint32_t xi_endianess_little_u32(const uint32_t value) {
#if defined(XI_COMPILER_ENDIANESS_LITTLE)
  return xi_endianess_identity_u32(value);
#else
  return xi_endianess_swap_u32(value);
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
static inline uint64_t xi_endianess_little_u64(const uint64_t value) {
#if defined(XI_COMPILER_ENDIANESS_LITTLE)
  return xi_endianess_identity_u64(value);
#else
  return xi_endianess_swap_u64(value);
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
static inline int16_t xi_endianess_little_16(const int16_t value) {
#if defined(XI_COMPILER_ENDIANESS_LITTLE)
  return xi_endianess_identity_16(value);
#else
  return xi_endianess_swap_16(value);
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
static inline int32_t xi_endianess_little_32(const int32_t value) {
#if defined(XI_COMPILER_ENDIANESS_LITTLE)
  return xi_endianess_identity_32(value);
#else
  return xi_endianess_swap_32(value);
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
static inline int64_t xi_endianess_little_64(const int64_t value) {
#if defined(XI_COMPILER_ENDIANESS_LITTLE)
  return xi_endianess_identity_64(value);
#else
  return xi_endianess_swap_64(value);
#endif
}

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

namespace Xi {
namespace Endianess {

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
[[nodiscard]] static inline constexpr uint16_t little(uint16_t value) {
#if defined(XI_COMPILER_ENDIANESS_BIG)
  return swap(value);
#else
  return value;
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
[[nodiscard]] static inline constexpr uint32_t little(uint32_t value) {
#if defined(XI_COMPILER_ENDIANESS_BIG)
  return swap(value);
#else
  return value;
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
[[nodiscard]] static inline constexpr uint64_t little(uint64_t value) {
#if defined(XI_COMPILER_ENDIANESS_BIG)
  return swap(value);
#else
  return value;
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
[[nodiscard]] static inline constexpr int16_t little(int16_t value) {
#if defined(XI_COMPILER_ENDIANESS_BIG)
  return swap(value);
#else
  return value;
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
[[nodiscard]] static inline constexpr int32_t little(int32_t value) {
#if defined(XI_COMPILER_ENDIANESS_BIG)
  return swap(value);
#else
  return value;
#endif
}

/*!
 * \brief Converts a native value into little endian representation.
 * \return Value in little endian representation.
 */
[[nodiscard]] static inline constexpr int64_t little(int64_t value) {
#if defined(XI_COMPILER_ENDIANESS_BIG)
  return swap(value);
#else
  return value;
#endif
}

}  // namespace Endianess
}  // namespace Xi

#endif
