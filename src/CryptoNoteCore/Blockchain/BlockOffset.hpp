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
#include <functional>

#include <Xi/Global.hh>

namespace CryptoNote {

class BlockOffset final {
 public:
  using value_type = int64_t;

 public:
  static BlockOffset fromNative(const value_type value);

 public:
  BlockOffset();
  XI_DEFAULT_COPY(BlockOffset);
  ~BlockOffset() = default;

  int64_t native() const;

  bool operator==(const BlockOffset rhs) const;
  bool operator!=(const BlockOffset rhs) const;
  bool operator<(const BlockOffset rhs) const;
  bool operator<=(const BlockOffset rhs) const;
  bool operator>(const BlockOffset rhs) const;
  bool operator>=(const BlockOffset rhs) const;

 private:
  explicit BlockOffset(value_type value);

 private:
  value_type m_native;
};

BlockOffset operator+(const BlockOffset lhs, const BlockOffset rhs);
BlockOffset& operator+=(BlockOffset& lhs, const BlockOffset rhs);
BlockOffset operator-(const BlockOffset lhs, const BlockOffset rhs);
BlockOffset& operator-=(BlockOffset& lhs, const BlockOffset rhs);

}  // namespace CryptoNote

namespace std {
template <>
struct hash<CryptoNote::BlockOffset> {
  std::size_t operator()(const CryptoNote::BlockOffset offset) const;
};
}  // namespace std
