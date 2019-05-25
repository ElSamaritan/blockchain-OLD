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

#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>

typedef uint8_t xi_byte_t;

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include <cinttypes>
#include <array>
#include <vector>
#include <string>
#include <string_view>

#include "Xi/Span.hpp"

namespace Xi {
using Byte = std::uint8_t;
using ByteVector = std::vector<Byte>;
template <size_t _Size>
using ByteArray = std::array<Byte, _Size>;

XI_DECLARE_SPANS(Byte)

static inline ByteSpan asByteSpan(void* data, size_t size) { return ByteSpan{reinterpret_cast<Byte*>(data), size}; }

static inline ConstByteSpan asByteSpan(const void* data, size_t size) {
  return ConstByteSpan{reinterpret_cast<const Byte*>(data), size};
}

static inline ConstByteSpan asConstByteSpan(void* data, size_t size) {
  return ConstByteSpan{reinterpret_cast<const Byte*>(data), size};
}

static inline ConstByteSpan asConstByteSpan(const void* data, size_t size) {
  return ConstByteSpan{reinterpret_cast<const Byte*>(data), size};
}

static inline ByteSpan asByteSpan(std::string& str) {
  return ByteSpan{reinterpret_cast<Byte*>(str.data()), str.size()};
}
static inline ConstByteSpan asByteSpan(const std::string& str) {
  return ConstByteSpan{reinterpret_cast<const Byte*>(str.data()), str.size()};
}
static inline ConstByteSpan asConstByteSpan(std::string& str) {
  return ConstByteSpan{reinterpret_cast<const Byte*>(str.data()), str.size()};
}
static inline ConstByteSpan asConstByteSpan(const std::string& str) {
  return ConstByteSpan{reinterpret_cast<const Byte*>(str.data()), str.size()};
}

static inline ConstByteSpan asByteSpan(std::string_view str) {
  return ConstByteSpan{reinterpret_cast<const Byte*>(str.data()), str.size()};
}
static inline ConstByteSpan asConstByteSpan(std::string_view str) {
  return ConstByteSpan{reinterpret_cast<const Byte*>(str.data()), str.size()};
}

}  // namespace Xi

static inline constexpr uint64_t operator"" _Bytes(unsigned long long bytes) { return bytes; }
static inline constexpr uint64_t operator"" _kB(unsigned long long kiloBytes) { return kiloBytes * 1024; }
static inline constexpr uint64_t operator"" _MB(unsigned long long megaBytes) { return megaBytes * 1024 * 1024; }

#endif
