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

#include <cinttypes>
#include <utility>
#include <limits>
#include <type_traits>
#include <string>
#include <vector>

#include <Xi/Global.hh>
#include <Xi/Span.hpp>
#include <Serialization/ISerializer.h>

#include "Xi/Blockchain/Block/Offset.hpp"

namespace Xi {
namespace Blockchain {
namespace Block {

class Height {
 public:
  using value_type = uint32_t;
  using signed_value_type = std::make_signed_t<value_type>;

  static Height min();
  static Height max();

 public:
  static const Height Null;
  static const Height Genesis;

  static Height fromIndex(value_type index);
  static Height fromNative(value_type native);
  static Height fromSize(size_t native);

 public:
  Height();
  XI_DEFAULT_COPY(Height);
  ~Height() = default;

  void displace(signed_value_type offset);
  void advance(size_t offset);
  Height shift(signed_value_type offset) const;
  Height next(size_t offset) const;

  value_type native() const;
  value_type toIndex() const;
  value_type toSize() const;

  bool isNull() const;

  bool operator==(const Height rhs) const;
  bool operator!=(const Height rhs) const;
  bool operator<(const Height rhs) const;
  bool operator<=(const Height rhs) const;
  bool operator>(const Height rhs) const;
  bool operator>=(const Height rhs) const;

 private:
  explicit Height(value_type height);

 private:
  uint32_t m_height;
};

using HeightVector = std::vector<Height>;
XI_DECLARE_SPANS(Height)

Offset operator-(const Height lhs, const Height rhs);
Height operator-(const Height& lhs, const Offset rhs);
Height& operator-=(Height& lhs, const Offset rhs);
Height operator+(const Height& lhs, const Offset rhs);
Height& operator+=(Height& lhs, const Offset rhs);

std::string toString(const Height height);
[[nodiscard]] bool serialize(Height& height, Common::StringView name, CryptoNote::ISerializer& serializer);

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi

namespace std {
template <>
struct hash<Xi::Blockchain::Block::Height> {
  std::size_t operator()(const Xi::Blockchain::Block::Height offset) const;
};
}  // namespace std
