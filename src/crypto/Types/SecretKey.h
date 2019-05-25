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

#include <array>
#include <string>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/Byte.hh>
#include <Serialization/ISerializer.h>
#include <Xi/Algorithm/GenericHash.h>
#include <Xi/Algorithm/GenericComparison.h>

namespace Crypto {
struct SecretKey : Xi::ByteArray<32> {
  using array_type = Xi::ByteArray<32>;
  static const SecretKey Null;
  static inline constexpr size_t bytes() { return 32; }
  static Xi::Result<SecretKey> fromString(const std::string& hex);

  SecretKey();
  explicit SecretKey(array_type raw);
  XI_DEFAULT_COPY(SecretKey);
  XI_DEFAULT_MOVE(SecretKey);
  ~SecretKey();

  std::string toString() const;

  Xi::ConstByteSpan span() const;
  Xi::ByteSpan span();

  void nullify();
  void serialize(CryptoNote::ISerializer& serializer);
};

XI_MAKE_GENERIC_HASH_FUNC(SecretKey)
XI_MAKE_GENERIC_COMPARISON(SecretKey)
}  // namespace Crypto

XI_MAKE_GENERIC_HASH_OVERLOAD(Crypto, SecretKey)
