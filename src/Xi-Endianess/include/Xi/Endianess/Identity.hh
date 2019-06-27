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

#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
static inline uint16_t xi_endianess_identity_u16(const uint16_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
static inline uint32_t xi_endianess_identity_u32(const uint32_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
static inline uint64_t xi_endianess_identity_u64(const uint64_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
static inline int16_t xi_endianess_identity_16(const int16_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
static inline int32_t xi_endianess_identity_32(const int32_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
static inline int64_t xi_endianess_identity_64(const int64_t value) { return value; }

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

namespace Xi {
namespace Endianess {

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
[[nodiscard]] static inline constexpr uint16_t identity(uint16_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
[[nodiscard]] static inline constexpr uint32_t identity(uint32_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
[[nodiscard]] static inline constexpr uint64_t identity(uint64_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
[[nodiscard]] static inline constexpr int16_t identity(int16_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
[[nodiscard]] static inline constexpr int32_t identity(int32_t value) { return value; }

/*!
 * \brief Returns the value given.
 * \param value Value to return.
 * \return The value given.
 */
[[nodiscard]] static inline constexpr int64_t identity(int64_t value) { return value; }

}  // namespace Endianess
}  // namespace Xi

#endif
