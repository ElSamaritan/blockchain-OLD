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
#include <cstdint>
#include <utility>

#define XI_MAKE_GENERIC_HASH_FUNC(_T)                                                                       \
  static inline std::size_t hash_value(const _T& value) {                                                   \
    static_assert(sizeof(_T) % sizeof(size_t) == 0, "Value types for generic hash must align with size_t"); \
    const uint64_t m = UINT64_C(0xc6a4a7935bd1e995);                                                        \
    const int r = 47;                                                                                       \
    std::size_t seed = 0;                                                                                   \
    for (std::size_t i = 0; i < sizeof(_T) / sizeof(std::size_t); ++i) {                                    \
      std::size_t k = reinterpret_cast<const std::size_t*>(&value)[i];                                      \
      k *= m;                                                                                               \
      k ^= k >> r;                                                                                          \
      k *= m;                                                                                               \
      seed ^= k;                                                                                            \
      seed *= m;                                                                                            \
      seed += 0xe6546b64;                                                                                   \
    }                                                                                                       \
    return seed;                                                                                            \
  }

#define XI_MAKE_GENERIC_HASH_OVERLOAD(_NS, _T)                                       \
  namespace std {                                                                    \
  template <>                                                                        \
  struct hash<_NS::_T> {                                                             \
    size_t operator()(const _NS::_T& value) const { return _NS::hash_value(value); } \
  };                                                                                 \
  }
