/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include <cstdint>
#include <cstring>
#include <utility>
#include <functional>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/functional/hash.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

namespace Crypto {

struct Hash {
  uint8_t data[32];
};

struct PublicKey {
  uint8_t data[32];
};

struct SecretKey {
  uint8_t data[32];
};

struct KeyDerivation {
  uint8_t data[32];
};

struct KeyImage {
  uint8_t data[32];
};

struct Signature {
  uint8_t data[64];
};

}  // namespace Crypto

#define MAKE_GENERIC_OPS(_T)                                                                \
  namespace Crypto {                                                                        \
  static inline bool operator==(const _T& lhs, const _T& rhs) {                             \
    return ::std::memcmp(lhs.data, rhs.data, sizeof(_T)) == 0;                              \
  }                                                                                         \
  static inline bool operator!=(const _T& lhs, const _T& rhs) {                             \
    return ::std::memcmp(lhs.data, rhs.data, sizeof(_T)) != 0;                              \
  }                                                                                         \
  static inline bool operator<(const _T& lhs, const _T& rhs) {                              \
    return ::std::memcmp(lhs.data, rhs.data, sizeof(_T)) < 0;                               \
  }                                                                                         \
  static inline bool operator<=(const _T& lhs, const _T& rhs) {                             \
    return ::std::memcmp(lhs.data, rhs.data, sizeof(_T)) < 1;                               \
  }                                                                                         \
  static inline bool operator>(const _T& lhs, const _T& rhs) {                              \
    return ::std::memcmp(lhs.data, rhs.data, sizeof(_T)) > 0;                               \
  }                                                                                         \
  static inline bool operator>=(const _T& lhs, const _T& rhs) {                             \
    return ::std::memcmp(lhs.data, rhs.data, sizeof(_T)) > -1;                              \
  }                                                                                         \
  static inline std::size_t hash_value(const _T& value) {                                   \
    const uint64_t m = UINT64_C(0xc6a4a7935bd1e995);                                        \
    const int r = 47;                                                                       \
    std::size_t seed = 0;                                                                   \
    for (std::size_t i = 0; i < sizeof(_T) / sizeof(std::size_t); ++i) {                    \
      std::size_t k = *reinterpret_cast<std::size_t*>(value.data[i * sizeof(std::size_t)]); \
      k *= m;                                                                               \
      k ^= k >> r;                                                                          \
      k *= m;                                                                               \
      seed ^= k;                                                                            \
      seed *= m;                                                                            \
      seed += 0xe6546b64;                                                                   \
    }                                                                                       \
    return seed;                                                                            \
  }                                                                                         \
  }                                                                                         \
  namespace std {                                                                           \
  template <>                                                                               \
  struct hash<_T> {                                                                         \
    size_t operator()(const _T& value) const { return Crypto::hash_value(value); }          \
  };                                                                                        \
  }  // namespace std

MAKE_GENERIC_OPS(::Crypto::Hash)
MAKE_GENERIC_OPS(::Crypto::PublicKey)
MAKE_GENERIC_OPS(::Crypto::SecretKey)
MAKE_GENERIC_OPS(::Crypto::KeyDerivation)
MAKE_GENERIC_OPS(::Crypto::KeyImage)
MAKE_GENERIC_OPS(::Crypto::Signature)

#undef MAKE_GENERIC_OPS
