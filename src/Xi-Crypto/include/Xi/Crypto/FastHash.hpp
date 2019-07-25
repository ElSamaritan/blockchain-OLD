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
#include <set>
#include <vector>
#include <ostream>

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

namespace Xi {
namespace Crypto {

XI_DECLARE_SPANS_STRUCT(FastHash)

struct FastHash : Xi::ByteArray<XI_HASH_FAST_HASH_SIZE> {
  using array_type = Xi::ByteArray<XI_HASH_FAST_HASH_SIZE>;
  static const FastHash Null;
  static inline constexpr size_t bytes() {
    return XI_HASH_FAST_HASH_SIZE;
  }
  static Xi::Result<FastHash> fromString(const std::string& hex);

  static Xi::Result<FastHash> compute(Xi::ConstByteSpan data);
  static Xi::Result<void> compute(Xi::ConstByteSpan data, FastHash& out);
  static Xi::Result<void> compute(Xi::ConstByteSpan data, Xi::ByteSpan out);

  static Xi::Result<FastHash> computeMerkleTree(Xi::ConstByteSpan data, size_t count);
  static Xi::Result<void> computeMerkleTree(Xi::ConstByteSpan data, size_t count, FastHash& out);

  static Xi::Result<FastHash> computeMerkleTree(ConstFastHashSpan data);
  static Xi::Result<void> computeMerkleTree(ConstFastHashSpan data, FastHash& out);

  template <typename _ValueT>
  static inline Xi::Result<FastHash> computeObjectHash(const _ValueT& value) {
    XI_ERROR_TRY();
    Xi::ByteVector blob{};
    Common::VectorOutputStream stream{blob};
    CryptoNote::BinaryOutputStreamSerializer serializer{stream};
    Xi::exceptional_if_not<Xi::RuntimeError>(serializer(const_cast<_ValueT&>(value), ""),
                                             "object serialization failed for hash computation");
    return FastHash::compute(blob);
    XI_ERROR_CATCH();
  }

  FastHash();
  explicit FastHash(array_type raw);
  XI_DEFAULT_COPY(FastHash);
  XI_DEFAULT_MOVE(FastHash);
  ~FastHash();

  bool isNull() const;

  std::string toString() const;
  std::string toShortString() const;

  Xi::ConstByteSpan span() const;
  Xi::ByteSpan span();

  void nullify();
};

XI_MAKE_GENERIC_HASH_FUNC(FastHash)
XI_MAKE_GENERIC_COMPARISON(FastHash)

[[nodiscard]] bool serialize(FastHash& hash, Common::StringView name, CryptoNote::ISerializer& serializer);

using FastHashHashVector = std::vector<FastHash>;
using FastHashHashSet = std::set<FastHash>;

std::ostream& operator<<(std::ostream& stream, const FastHash& hash);

}  // namespace Crypto
}  // namespace Xi

XI_MAKE_GENERIC_HASH_OVERLOAD(Xi::Crypto, FastHash)

namespace Crypto {

using Hash = Xi::Crypto::FastHash;
using HashSet = std::set<Hash>;
using HashVector = std::vector<Hash>;
XI_DECLARE_SPANS(Hash)

}  // namespace Crypto
