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

#include <Xi/Byte.hh>

#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>
#include <stddef.h>

#define XI_VARINT_DECODE_OUT_OF_MEMORY (SIZE_MAX - 0)
#define XI_VARINT_DECODE_OVERFLOW (SIZE_MAX - 1)
#define XI_VARINT_DECODE_NONE_CANONICAL (SIZE_MAX - 2)
#define XI_VARINT_ENCODE_OUT_OF_MEMORY (SIZE_MAX - 0)

#define XI_VARINT_INT8_MAX_BYTES 2
#define XI_VARINT_UINT8_MAX_BYTES 2
#define XI_VARINT_INT16_MAX_BYTES 3
#define XI_VARINT_UINT16_MAX_BYTES 3
#define XI_VARINT_INT32_MAX_BYTES 5
#define XI_VARINT_UINT32_MAX_BYTES 5
#define XI_VARINT_INT64_MAX_BYTES 10
#define XI_VARINT_UINT64_MAX_BYTES 10

size_t xi_encoding_varint_decode_int8(const xi_byte_t *source, const size_t count, int8_t *out);
size_t xi_encoding_varint_decode_uint8(const xi_byte_t *source, const size_t count, uint8_t *out);
size_t xi_encoding_varint_decode_int16(const xi_byte_t *source, const size_t count, int16_t *out);
size_t xi_encoding_varint_decode_uint16(const xi_byte_t *source, const size_t count, uint16_t *out);
size_t xi_encoding_varint_decode_int32(const xi_byte_t *source, const size_t count, int32_t *out);
size_t xi_encoding_varint_decode_uint32(const xi_byte_t *source, const size_t count, uint32_t *out);
size_t xi_encoding_varint_decode_int64(const xi_byte_t *source, const size_t count, int64_t *out);
size_t xi_encoding_varint_decode_uint64(const xi_byte_t *source, const size_t count, uint64_t *out);

size_t xi_encoding_varint_encode_int8(int8_t value, xi_byte_t *dest, const size_t count);
size_t xi_encoding_varint_encode_uint8(uint8_t value, xi_byte_t *dest, const size_t count);
size_t xi_encoding_varint_encode_int16(int16_t value, xi_byte_t *dest, const size_t count);
size_t xi_encoding_varint_encode_uint16(uint16_t value, xi_byte_t *dest, const size_t count);
size_t xi_encoding_varint_encode_int32(int32_t value, xi_byte_t *dest, const size_t count);
size_t xi_encoding_varint_encode_uint32(uint32_t value, xi_byte_t *dest, const size_t count);
size_t xi_encoding_varint_encode_int64(int64_t value, xi_byte_t *dest, const size_t count);
size_t xi_encoding_varint_encode_uint64(uint64_t value, xi_byte_t *dest, const size_t count);

int xi_encoding_varint_has_successor(xi_byte_t current);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include <cinttypes>

#include <Xi/ErrorCode.hpp>
#include <Xi/Result.h>

namespace Xi {
namespace Encoding {
namespace VarInt {

template <typename _IntT>
inline constexpr size_t maximumEncodingSize();

template <>
inline constexpr size_t maximumEncodingSize<int8_t>() {
  return XI_VARINT_INT8_MAX_BYTES;
}
template <>
inline constexpr size_t maximumEncodingSize<uint8_t>() {
  return XI_VARINT_UINT8_MAX_BYTES;
}
template <>
inline constexpr size_t maximumEncodingSize<int16_t>() {
  return XI_VARINT_INT16_MAX_BYTES;
}
template <>
inline constexpr size_t maximumEncodingSize<uint16_t>() {
  return XI_VARINT_UINT16_MAX_BYTES;
}
template <>
inline constexpr size_t maximumEncodingSize<int32_t>() {
  return XI_VARINT_INT32_MAX_BYTES;
}
template <>
inline constexpr size_t maximumEncodingSize<uint32_t>() {
  return XI_VARINT_UINT32_MAX_BYTES;
}
template <>
inline constexpr size_t maximumEncodingSize<int64_t>() {
  return XI_VARINT_INT64_MAX_BYTES;
}
template <>
inline constexpr size_t maximumEncodingSize<uint64_t>() {
  return XI_VARINT_UINT64_MAX_BYTES;
}

Result<size_t> decode(const ByteSpan source, int8_t &out);
Result<size_t> decode(const ByteSpan source, uint8_t &out);
Result<size_t> decode(const ByteSpan source, int16_t &out);
Result<size_t> decode(const ByteSpan source, uint16_t &out);
Result<size_t> decode(const ByteSpan source, int32_t &out);
Result<size_t> decode(const ByteSpan source, uint32_t &out);
Result<size_t> decode(const ByteSpan source, int64_t &out);
Result<size_t> decode(const ByteSpan source, uint64_t &out);

Result<size_t> encode(int8_t value, ByteSpan dest);
Result<size_t> encode(uint8_t value, ByteSpan dest);
Result<size_t> encode(int16_t value, ByteSpan dest);
Result<size_t> encode(uint16_t value, ByteSpan dest);
Result<size_t> encode(int32_t value, ByteSpan dest);
Result<size_t> encode(uint32_t value, ByteSpan dest);
Result<size_t> encode(int64_t value, ByteSpan dest);
Result<size_t> encode(uint64_t value, ByteSpan dest);

bool hasSuccessor(Byte current);

XI_ERROR_CODE_BEGIN(Decode)
XI_ERROR_CODE_VALUE(OutOfMemory, 0x0001)
XI_ERROR_CODE_VALUE(Overflow, 0x0002)
XI_ERROR_CODE_VALUE(NoneCanonical, 0x0003)
XI_ERROR_CODE_END(Decode, VarIntDecodeError)

XI_ERROR_CODE_BEGIN(Encode)
XI_ERROR_CODE_VALUE(OutOfMemory, 0x0001)
XI_ERROR_CODE_END(Encode, VarIntEncodeError)

};  // namespace VarInt
}  // namespace Encoding
}  // namespace Xi

XI_ERROR_CODE_OVERLOADS(Xi::Encoding::VarInt, Decode)
XI_ERROR_CODE_OVERLOADS(Xi::Encoding::VarInt, Encode)

#endif
