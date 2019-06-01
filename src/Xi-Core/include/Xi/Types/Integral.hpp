/* ------------------------------------------------------------------------------------------------------------------ *
 *                                                                                                                    *
 *                                                   MIT Licenese                                                     *
 *                                                                                                                    *
 * ------------------------------------------------------------------------------------------------------------------ *
 *                                                                                                                    *
 * Copyright 2019 Michael Herwig <michael.herwig@hotmail.de>                                                          *
 *                                                                                                                    *
 * ------------------------------------------------------------------------------------------------------------------ *
 *                                                                                                                    *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated       *
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation    *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and   *
 * to permit persons to whom the Software is furnished to do so, subject to the following conditions:                 *
 *                                                                                                                    *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of   *
 * the Software.                                                                                                      *
 *                                                                                                                    *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO   *
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF          *
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS  *
 * IN THE SOFTWARE.                                                                                                   *
 *                                                                                                                    *
 * ------------------------------------------------------------------------------------------------------------------ */

#pragma once

#include <type_traits>

namespace Xi {

template <typename _ValueT, typename _CompType>
struct enable_integral_from_this {
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
  explicit constexpr enable_integral_from_this() : value{} {}
  explicit constexpr enable_integral_from_this(value_type _value) : value{_value} {}
  explicit constexpr enable_integral_from_this(const enable_integral_from_this &_value) : value{_value.value} {}
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

}  // namespace Xi
