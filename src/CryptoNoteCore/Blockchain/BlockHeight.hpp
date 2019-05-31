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
#include <utility>
#include <limits>
#include <type_traits>
#include <string>

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>

#include "CryptoNoteCore/Blockchain/BlockOffset.hpp"

namespace CryptoNote {

class BlockHeight {
 public:
  using value_type = uint32_t;
  using signed_value_type = std::make_signed_t<value_type>;

  static inline constexpr value_type min() { return std::numeric_limits<value_type>::min() + 1; }
  static inline constexpr value_type max() { return std::numeric_limits<value_type>::max() - 1; }

 public:
  static const BlockHeight Null;
  static const BlockHeight Genesis;

  static BlockHeight fromIndex(value_type index);
  static BlockHeight fromNative(value_type native);
  static BlockHeight fromSize(size_t native);

 public:
  BlockHeight();
  XI_DEFAULT_COPY(BlockHeight);
  ~BlockHeight() = default;

  void displace(signed_value_type offset);
  void advance(size_t offset);
  BlockHeight shift(signed_value_type offset) const;
  BlockHeight next(size_t offset) const;

  value_type native() const;
  value_type toIndex() const;
  value_type toSize() const;

  bool isNull() const;

  bool operator==(const BlockHeight rhs) const;
  bool operator!=(const BlockHeight rhs) const;
  bool operator<(const BlockHeight rhs) const;
  bool operator<=(const BlockHeight rhs) const;
  bool operator>(const BlockHeight rhs) const;
  bool operator>=(const BlockHeight rhs) const;

 private:
  explicit BlockHeight(value_type height);

 private:
  uint32_t m_height;
};

BlockOffset operator-(const BlockHeight lhs, const BlockHeight rhs);
BlockHeight operator-(const BlockHeight& lhs, const BlockOffset rhs);
BlockHeight& operator-=(BlockHeight& lhs, const BlockOffset rhs);
BlockHeight operator+(const BlockHeight& lhs, const BlockOffset rhs);
BlockHeight& operator+=(BlockHeight& lhs, const BlockOffset rhs);

std::string toString(const BlockHeight height);
[[nodiscard]] bool serialize(BlockHeight& height, Common::StringView name, ISerializer& serializer);

}  // namespace CryptoNote

namespace std {
template <>
struct hash<CryptoNote::BlockHeight> {
  std::size_t operator()(const CryptoNote::BlockHeight offset) const;
};
}  // namespace std
