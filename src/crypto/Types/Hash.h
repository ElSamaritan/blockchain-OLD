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

#include <array>
#include <string>
#include <memory>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/Byte.hh>
#include <Xi/Span.hpp>
#include <Xi/Exceptions.hpp>
#include <Xi/Crypto/Hash/FastHash.hh>
#include <Common/VectorOutputStream.h>
#include <Serialization/ISerializer.h>
#include <Serialization/BinaryOutputStreamSerializer.h>
#include <Xi/Algorithm/GenericHash.h>
#include <Xi/Algorithm/GenericComparison.h>

namespace Crypto {

XI_DECLARE_SPANS_STRUCT(Hash)

struct Hash : Xi::ByteArray<XI_HASH_FAST_HASH_SIZE> {
  using array_type = Xi::ByteArray<XI_HASH_FAST_HASH_SIZE>;
  static const Hash Null;
  static inline constexpr size_t bytes() {
    return XI_HASH_FAST_HASH_SIZE;
  }
  static Xi::Result<Hash> fromString(const std::string& hex);

  static Xi::Result<Hash> compute(Xi::ConstByteSpan data);
  static Xi::Result<void> compute(Xi::ConstByteSpan data, Hash& out);
  static Xi::Result<void> compute(Xi::ConstByteSpan data, Xi::ByteSpan out);

  static Xi::Result<Hash> computeMerkleTree(Xi::ConstByteSpan data, size_t count);
  static Xi::Result<void> computeMerkleTree(Xi::ConstByteSpan data, size_t count, Hash& out);

  static Xi::Result<Hash> computeMerkleTree(ConstHashSpan data);
  static Xi::Result<void> computeMerkleTree(ConstHashSpan data, Hash& out);

  template <typename _ValueT>
  static inline Xi::Result<Hash> computeObjectHash(const _ValueT& value) {
    XI_ERROR_TRY();
    Xi::ByteVector blob{};
    Common::VectorOutputStream stream{blob};
    CryptoNote::BinaryOutputStreamSerializer serializer{stream};
    Xi::exceptional_if_not<Xi::RuntimeError>(serializer(const_cast<_ValueT&>(value), ""),
                                             "object serialization failed for hash computation");
    return Hash::compute(blob);
    XI_ERROR_CATCH();
  }

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
};

XI_MAKE_GENERIC_HASH_FUNC(Hash)
XI_MAKE_GENERIC_COMPARISON(Hash)

[[nodiscard]] bool serialize(Hash& hash, Common::StringView name, CryptoNote::ISerializer& serializer);

using HashVector = std::vector<Hash>;

}  // namespace Crypto

XI_MAKE_GENERIC_HASH_OVERLOAD(Crypto, Hash)
