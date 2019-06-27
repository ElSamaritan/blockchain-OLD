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

#include "Xi/Endianess/Identity.hh"

#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
static inline uint16_t xi_endianess_swap_u16(const uint16_t value) {
  return ((uint16_t)((value & 0x00FFU) << 8) | (uint16_t)((value & 0xFF00U) >> 8));
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
static inline uint32_t xi_endianess_swap_u32(const uint32_t value) {
  return ((uint32_t)((value & 0x000000FFU) << 24) | (uint32_t)((value & 0x0000FF00U) << 8) |
          (uint32_t)((value & 0x00FF0000U) >> 8) | (uint32_t)((value & 0xFF000000U) >> 24));
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
static inline uint64_t xi_endianess_swap_u64(const uint64_t value) {
  return ((uint64_t)((value & 0x00000000000000FFULL) << 56) | (uint64_t)((value & 0x000000000000FF00ULL) << 40) |
          (uint64_t)((value & 0x0000000000FF0000ULL) << 24) | (uint64_t)((value & 0x00000000FF000000ULL) << 8) |
          (uint64_t)((value & 0x000000FF00000000ULL) >> 8) | (uint64_t)((value & 0x0000FF0000000000ULL) >> 24) |
          (uint64_t)((value & 0x00FF000000000000ULL) >> 40) | (uint64_t)((value & 0xFF00000000000000ULL) >> 56));
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
static inline int16_t xi_endianess_swap_16(const int16_t value) {
  return (int16_t)xi_endianess_swap_u16((uint16_t)value);
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
static inline int32_t xi_endianess_swap_32(const int32_t value) {
  return (int32_t)xi_endianess_swap_u32((uint32_t)value);
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
static inline int64_t xi_endianess_swap_64(const int64_t value) {
  return (int64_t)xi_endianess_swap_u64((uint64_t)value);
}

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

namespace Xi {
namespace Endianess {

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
[[nodiscard]] static inline constexpr uint16_t swap(uint16_t value) {
  return ((uint16_t)((value & 0x00FFU) << 8) | (uint16_t)((value & 0xFF00U) >> 8));
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
[[nodiscard]] static inline constexpr uint32_t swap(uint32_t value) {
  return ((uint32_t)((value & 0x000000FFU) << 24) | (uint32_t)((value & 0x0000FF00U) << 8) |
          (uint32_t)((value & 0x00FF0000U) >> 8) | (uint32_t)((value & 0xFF000000U) >> 24));
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
[[nodiscard]] static inline constexpr uint64_t swap(uint64_t value) {
  return ((uint64_t)((value & 0x00000000000000FFULL) << 56) | (uint64_t)((value & 0x000000000000FF00ULL) << 40) |
          (uint64_t)((value & 0x0000000000FF0000ULL) << 24) | (uint64_t)((value & 0x00000000FF000000ULL) << 8) |
          (uint64_t)((value & 0x000000FF00000000ULL) >> 8) | (uint64_t)((value & 0x0000FF0000000000ULL) >> 24) |
          (uint64_t)((value & 0x00FF000000000000ULL) >> 40) | (uint64_t)((value & 0xFF00000000000000ULL) >> 56));
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
[[nodiscard]] static inline constexpr int16_t swap(int16_t value) {
  return static_cast<int16_t>(static_cast<uint16_t>(value));
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
[[nodiscard]] static inline constexpr int32_t swap(int32_t value) {
  return static_cast<int32_t>(static_cast<uint32_t>(value));
}

/*!
 * \brief Swaps the endianess of the given value.
 * \param value The value to convert.
 * \return A value in converted endianess representation.
 */
[[nodiscard]] static inline constexpr int64_t swap(int64_t value) {
  return static_cast<int64_t>(static_cast<uint64_t>(value));
}

}  // namespace Endianess
}  // namespace Xi

#endif
