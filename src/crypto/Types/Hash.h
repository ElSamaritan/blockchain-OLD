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
#include <Xi/Span.hpp>
#include <Xi/Crypto/Hash/FastHash.hh>
#include <Serialization/ISerializer.h>
#include <Xi/Algorithm/GenericHash.h>
#include <Xi/Algorithm/GenericComparison.h>

namespace Crypto {

XI_DECLARE_SPANS_STRUCT(Hash)

struct Hash : Xi::ByteArray<XI_HASH_FAST_HASH_SIZE> {
  using array_type = Xi::ByteArray<XI_HASH_FAST_HASH_SIZE>;
  static const Hash Null;
  static inline constexpr size_t bytes() { return XI_HASH_FAST_HASH_SIZE; }
  static Xi::Result<Hash> fromString(const std::string& hex);

  static Xi::Result<Hash> compute(Xi::ConstByteSpan data);
  static Xi::Result<void> compute(Xi::ConstByteSpan data, Hash& out);
  static Xi::Result<void> compute(Xi::ConstByteSpan data, Xi::ByteSpan out);

  static Xi::Result<Hash> computeMerkleTree(Xi::ConstByteSpan data, size_t count);
  static Xi::Result<void> computeMerkleTree(Xi::ConstByteSpan data, size_t count, Hash& out);

  static Xi::Result<Hash> computeMerkleTree(ConstHashSpan data);
  static Xi::Result<void> computeMerkleTree(ConstHashSpan data, Hash& out);

  Hash();
  explicit Hash(array_type raw);
  XI_DEFAULT_COPY(Hash);
  XI_DEFAULT_MOVE(Hash);
  ~Hash();

  bool isNull() const;

  std::string toString() const;
  std::string toShortString() const;

  Xi::ConstByteSpan span() const;
  Xi::ByteSpan span();

  void nullify();
  bool serialize(CryptoNote::ISerializer& serializer);
};

XI_MAKE_GENERIC_HASH_FUNC(Hash)
XI_MAKE_GENERIC_COMPARISON(Hash)
}  // namespace Crypto

XI_MAKE_GENERIC_HASH_OVERLOAD(Crypto, Hash)
