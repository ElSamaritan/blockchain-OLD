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
#include <functional>

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>

namespace Xi {
namespace Blockchain {
namespace Block {

class Offset final {
 public:
  using value_type = int64_t;

 public:
  static Offset fromNative(const value_type value);

 public:
  Offset();
  XI_DEFAULT_COPY(Offset);
  ~Offset() = default;

  int64_t native() const;

  bool operator==(const Offset rhs) const;
  bool operator!=(const Offset rhs) const;
  bool operator<(const Offset rhs) const;
  bool operator<=(const Offset rhs) const;
  bool operator>(const Offset rhs) const;
  bool operator>=(const Offset rhs) const;

 private:
  explicit Offset(value_type value);

  friend bool serialize(Offset&, Common::StringView, CryptoNote::ISerializer&);

 private:
  value_type m_native;
};

Offset operator+(const Offset lhs, const Offset rhs);
Offset& operator+=(Offset& lhs, const Offset rhs);
Offset operator-(const Offset lhs, const Offset rhs);
Offset& operator-=(Offset& lhs, const Offset rhs);

[[nodiscard]] bool serialize(Offset& value, Common::StringView name, CryptoNote::ISerializer& serializer);

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi

namespace std {
template <>
struct hash<Xi::Blockchain::Block::Offset> {
  std::size_t operator()(const Xi::Blockchain::Block::Offset offset) const;
};
}  // namespace std
