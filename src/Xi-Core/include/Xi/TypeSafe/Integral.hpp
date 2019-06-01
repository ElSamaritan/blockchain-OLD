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

#include <type_traits>

namespace Xi {
namespace TypeSafe {

template <typename _ValueT, typename _CompType>
struct EnableIntegralFromThis {
 public:
  static_assert(std::is_arithmetic_v<_ValueT>, "Arithmetic can only wrap arithmetic types");

  using value_type = _ValueT;
  using compatible_t = _CompType;

 protected:
  value_type value;  ///< stores the actual value representation

 private:
  constexpr compatible_t &this_compatible() { return *static_cast<compatible_t *>(this); }
  constexpr const compatible_t &this_compatible() const { return *static_cast<const compatible_t *>(this); }

 public:
  explicit constexpr EnableIntegralFromThis() : value{} {}
  explicit constexpr EnableIntegralFromThis(value_type _value) : value{_value} {}
  explicit constexpr EnableIntegralFromThis(const EnableIntegralFromThis &_value) : value{_value.value} {}
  compatible_t &operator=(compatible_t _value) {
    this->value = _value;
    return this_compatible();
  }

  constexpr bool operator==(const compatible_t &rhs) const { return this->value == rhs.value; }
  constexpr bool operator!=(const compatible_t &rhs) const { return this->value != rhs.value; }

  constexpr bool operator<(const compatible_t &rhs) const { return this->value < rhs.value; }
  constexpr bool operator<=(const compatible_t &rhs) const { return this->value <= rhs.value; }
  constexpr bool operator>(const compatible_t &rhs) const { return this->value > rhs.value; }
  constexpr bool operator>=(const compatible_t &rhs) const { return this->value >= rhs.value; }
};

}  // namespace TypeSafe
}  // namespace Xi
