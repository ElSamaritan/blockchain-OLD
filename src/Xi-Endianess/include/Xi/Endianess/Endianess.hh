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
#include "Xi/Endianess/Swap.hh"
#include "Xi/Endianess/Type.hh"
#include "Xi/Endianess/Big.hh"
#include "Xi/Endianess/Little.hh"

#if defined(__cplusplus)

namespace Xi {
namespace Endianess {

/*!
 * \brief Convertes endianess of a value with a given endianness type at runtime.
 * \param value The value to convert.
 * \param target The endianess conversion target.
 * \return The value in target endianness format.
 *
 * \note If you receive or send a value in a target endiannes you can apply this conversion in both
 * cases. Ie. you receive a little endian value and want it in big endian representation you may
 * call this function with little endian again to perform the swap.
 */
[[nodiscard]] static inline constexpr uint16_t convert(uint16_t value, const Type target) {
  switch (target) {
    case Type::Native:
      return identity(value);
    case Type::Little:
      return little(value);
    case Type::Big:
      return big(value);
  }
  return identity(value);
}

/*!
 * \brief Convertes endianess of a value with a given endianness type at runtime.
 * \param value The value to convert.
 * \param target The endianess conversion target.
 * \return The value in target endianness format.
 *
 * \note If you receive or send a value in a target endiannes you can apply this conversion in both
 * cases. Ie. you receive a little endian value and want it in big endian representation you may
 * call this function with little endian again to perform the swap.
 */
[[nodiscard]] static inline constexpr uint32_t convert(uint32_t value, const Type target) {
  switch (target) {
    case Type::Native:
      return identity(value);
    case Type::Little:
      return little(value);
    case Type::Big:
      return big(value);
  }
  return identity(value);
}

/*!
 * \brief Convertes endianess of a value with a given endianness type at runtime.
 * \param value The value to convert.
 * \param target The endianess conversion target.
 * \return The value in target endianness format.
 *
 * \note If you receive or send a value in a target endiannes you can apply this conversion in both
 * cases. Ie. you receive a little endian value and want it in big endian representation you may
 * call this function with little endian again to perform the swap.
 */
[[nodiscard]] static inline constexpr uint64_t convert(uint64_t value, const Type target) {
  switch (target) {
    case Type::Native:
      return identity(value);
    case Type::Little:
      return little(value);
    case Type::Big:
      return big(value);
  }
  return identity(value);
}

/*!
 * \brief Convertes endianess of a value with a given endianness type at runtime.
 * \param value The value to convert.
 * \param target The endianess conversion target.
 * \return The value in target endianness format.
 *
 * \note If you receive or send a value in a target endiannes you can apply this conversion in both
 * cases. Ie. you receive a little endian value and want it in big endian representation you may
 * call this function with little endian again to perform the swap.
 */
[[nodiscard]] static inline constexpr int16_t convert(int16_t value, const Type target) {
  switch (target) {
    case Type::Native:
      return identity(value);
    case Type::Little:
      return little(value);
    case Type::Big:
      return big(value);
  }
  return identity(value);
}

/*!
 * \brief Convertes endianess of a value with a given endianness type at runtime.
 * \param value The value to convert.
 * \param target The endianess conversion target.
 * \return The value in target endianness format.
 *
 * \note If you receive or send a value in a target endiannes you can apply this conversion in both
 * cases. Ie. you receive a little endian value and want it in big endian representation you may
 * call this function with little endian again to perform the swap.
 */
[[nodiscard]] static inline constexpr int32_t convert(int32_t value, const Type target) {
  switch (target) {
    case Type::Native:
      return identity(value);
    case Type::Little:
      return little(value);
    case Type::Big:
      return big(value);
  }
  return identity(value);
}

/*!
 * \brief Convertes endianess of a value with a given endianness type at runtime.
 * \param value The value to convert.
 * \param target The endianess conversion target.
 * \return The value in target endianness format.
 *
 * \note If you receive or send a value in a target endiannes you can apply this conversion in both
 * cases. Ie. you receive a little endian value and want it in big endian representation you may
 * call this function with little endian again to perform the swap.
 */
[[nodiscard]] static inline constexpr int64_t convert(int64_t value, const Type target) {
  switch (target) {
    case Type::Native:
      return identity(value);
    case Type::Little:
      return little(value);
    case Type::Big:
      return big(value);
  }
  return identity(value);
}

}  // namespace Endianess
}  // namespace Xi

#endif
