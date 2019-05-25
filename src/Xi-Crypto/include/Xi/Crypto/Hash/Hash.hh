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

#include <Xi/Byte.hh>

#define XI_HASH_128_SIZE 16u
#define XI_HASH_128_BITS (XI_HASHSIZE * 8u)

#define XI_HASH_224_SIZE 28u
#define XI_HASH_224_BITS (XI_HASH_224_BITS * 8u)

#define XI_HASH_256_SIZE 32u
#define XI_HASH_256_BITS (XI_HASH_256_SIZE * 8u)

#define XI_HASH_384_SIZE 48u
#define XI_HASH_384_BITS (XI_HASH_384_BITS * 8u)

#define XI_HASH_512_SIZE 64u
#define XI_HASH_512_BITS (XI_HASH_512_SIZE * 8u)

#define XI_HASH_1600_SIZE 200u
#define XI_HASH_1600_BITS (XI_HASH_1600_SIZE * 8u)

#if defined(__cplusplus)
extern "C" {
#endif

typedef xi_byte_t xi_crypto_hash_128[XI_HASH_128_SIZE];
typedef xi_byte_t xi_crypto_hash_224[XI_HASH_224_SIZE];
typedef xi_byte_t xi_crypto_hash_256[XI_HASH_256_SIZE];
typedef xi_byte_t xi_crypto_hash_384[XI_HASH_384_SIZE];
typedef xi_byte_t xi_crypto_hash_512[XI_HASH_512_SIZE];
typedef xi_byte_t xi_crypto_hash_1600[XI_HASH_1600_SIZE];

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include <cinttypes>
#include <initializer_list>
#include <utility>
#include <string_view>
#include <cstring>
#include <string>

#include <Xi/Global.hh>
#include <Xi/Blob.hpp>

namespace Xi {
namespace Crypto {
namespace Hash {

template <typename _HashT>
_HashT computeHash(ConstByteSpan data) {
  _HashT reval{};
  compute(data, reval);
  return reval;
}

template <typename _T, size_t _Bits>
struct enable_hash_from_this : enable_blob_from_this<_T, _Bits / 8> {
  static_assert((_Bits % 8) == 0, "hash number of bits must be a multiple of 8");

  using value_type = _T;

  static const value_type Null;

  static inline constexpr size_t bits() { return _Bits; }

  static inline value_type compute(ConstByteSpan data) { return computeHash<value_type>(data); }
  static inline value_type compute(std::string_view data) {
    return computeHash<value_type>(
        ConstByteSpan{reinterpret_cast<const ConstByteSpan::const_pointer>(data.data()), data.size()});
  }

  using enable_blob_from_this<_T, _Bits / 8>::enable_blob_from_this;

  inline void nullify() { this->fill(0); }
  inline bool isNull() const { return *this == Null; }
};

#define XI_CRYPTO_HASH_DECLARE_HASH_TYPE(NAME, BITS)                    \
  struct NAME : ::Xi::Crypto::Hash::enable_hash_from_this<NAME, BITS> { \
    using enable_hash_from_this::enable_hash_from_this;                 \
    XI_DEFAULT_COPY(NAME);                                              \
    XI_DEFAULT_MOVE(NAME);                                              \
  }

#define XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(CLASS, BITS) \
  template <>                                                   \
  const CLASS Xi::Crypto::Hash::enable_hash_from_this<CLASS, BITS>::Null{};

}  // namespace Hash
}  // namespace Crypto
}  // namespace Xi

#endif
