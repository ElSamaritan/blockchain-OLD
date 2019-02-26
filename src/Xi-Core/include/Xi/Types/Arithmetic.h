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

#include <type_traits>

#include <Xi/Global.h>

namespace Xi {
namespace Types {

/*!
 * \brief Arithmetic wraps an arithmetic type and is only compatible to its own type.
 *
 * Encoding values for different semantic meaning with the same type can lead to hard errors.
 * Use this class to make an arithmetic type that is only compatible to itself.
 */
template <typename _ValueT, typename _CompType>
struct enable_arithmetic_from_this {
  static_assert(std::is_arithmetic_v<_ValueT>, "Arithmetic can only wrap arithmetic types");

  using value_t = _ValueT;
  using compatible_t = _CompType;

  value_t value;  ///< stores the actual value representation

  compatible_t& this_compatible() { return *static_cast<compatible_t*>(this); }
  const compatible_t& this_compatible() const { return *static_cast<const compatible_t*>(this); }

  explicit enable_arithmetic_from_this() : value{} {}
  explicit enable_arithmetic_from_this(value_t _value) : value{_value} {}
  explicit enable_arithmetic_from_this(const enable_arithmetic_from_this& _value) : value{_value.value} {}
  compatible_t& operator=(compatible_t _value) {
    this->value = _value;
    return this_compatible();
  }

  bool operator==(const compatible_t& rhs) const { return this->value == rhs.value; }
  bool operator!=(const compatible_t& rhs) const { return this->value != rhs.value; }

  bool operator<(const compatible_t& rhs) const { return this->value < rhs.value; }
  bool operator<=(const compatible_t& rhs) const { return this->value <= rhs.value; }
  bool operator>(const compatible_t& rhs) const { return this->value > rhs.value; }
  bool operator>=(const compatible_t& rhs) const { return this->value >= rhs.value; }

  compatible_t operator+(compatible_t rhs) const { return compatible_t{this->value + rhs.value}; }
  compatible_t& operator+=(compatible_t rhs) {
    this->value += rhs.value;
    return this_compatible();
  }

  compatible_t operator-(compatible_t rhs) const { return compatible_t{this->value - rhs.value}; }
  compatible_t& operator-=(compatible_t rhs) {
    this->value -= rhs.value;
    return this_compatible();
  }
};
}  // namespace Types
}  // namespace Xi
