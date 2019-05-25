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

#include <cstring>

#include "Xi/Byte.hh"

namespace Xi {

template <typename _T, size_t _Bytes>
struct enable_blob_from_this : protected ByteArray<_Bytes> {
  using value_type = _T;
  using array_type = ByteArray<_Bytes>;

  static inline constexpr size_t bytes() { return _Bytes; }

  enable_blob_from_this() { this->fill(0); }
  explicit enable_blob_from_this(array_type raw) : array_type(std::move(raw)) {}

  using array_type::data;
  using array_type::size;
  using array_type::operator[];

  ByteSpan span() { return ByteSpan{this->data(), this->size()}; }
  ConstByteSpan span() const { return ConstByteSpan{this->data(), this->size()}; }

  inline bool operator==(const value_type &rhs) { return ::std::memcmp(this->data(), rhs.data(), bytes()) == 0; }
  inline bool operator!=(const value_type &rhs) { return ::std::memcmp(this->data(), rhs.data(), bytes()) != 0; }
};

}  // namespace Xi
