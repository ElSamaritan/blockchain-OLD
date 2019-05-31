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
#include <string>
#include <vector>

#include <Xi/Global.hh>

namespace CryptoNote {
class TypeTag {
 public:
  using binary_type = uint64_t;
  using text_type = std::string;

 public:
  static const TypeTag Null;

  static const binary_type NoBinaryTag;
  static const text_type NoTextTag;

 public:
  TypeTag(binary_type binary, text_type text);
  XI_DEFAULT_COPY(TypeTag);
  XI_DEFAULT_MOVE(TypeTag);
  ~TypeTag() = default;

  binary_type binary() const;
  const text_type &text() const;
  bool isNull() const;

  bool operator==(const TypeTag &rhs) const;
  bool operator!=(const TypeTag &rhs) const;

 private:
  binary_type m_binary;
  text_type m_text;
};

using TypeTagVector = std::vector<TypeTag>;
}  // namespace CryptoNote
