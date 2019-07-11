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

#include "Xi/Encoding/VarInt.hh"

#include "Xi/Global.hh"

XI_ERROR_CODE_CATEGORY_BEGIN(Xi::Encoding::VarInt, Decode)
XI_ERROR_CODE_DESC(Overflow, "decoding resulted in an overflow")
XI_ERROR_CODE_DESC(OutOfMemory, "decoding expected an successing byte, but reached end of stream")
XI_ERROR_CODE_DESC(NoneCanonical, "a trailing zero byte was encoded")
XI_ERROR_CODE_CATEGORY_END()

XI_ERROR_CODE_CATEGORY_BEGIN(Xi::Encoding::VarInt, Encode)
XI_ERROR_CODE_DESC(OutOfMemory, "end of stream reached writing encoded bytes")
XI_ERROR_CODE_CATEGORY_END()

Xi::Result<size_t> Xi::Encoding::VarInt::decode(const Xi::ByteSpan source, int8_t &out) {
  auto reval = xi_encoding_varint_decode_int8(source.data(), static_cast<size_t>(source.size()), &out);
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OVERFLOW, makeError(DecodeError::Overflow));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_NONE_CANONICAL, makeError(DecodeError::NoneCanonical));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OUT_OF_MEMORY, makeError(DecodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::decode(const Xi::ByteSpan source, uint8_t &out) {
  auto reval = xi_encoding_varint_decode_uint8(source.data(), static_cast<size_t>(source.size()), &out);
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OVERFLOW, makeError(DecodeError::Overflow));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_NONE_CANONICAL, makeError(DecodeError::NoneCanonical));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OUT_OF_MEMORY, makeError(DecodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::decode(const Xi::ByteSpan source, int16_t &out) {
  auto reval = xi_encoding_varint_decode_int16(source.data(), static_cast<size_t>(source.size()), &out);
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OVERFLOW, makeError(DecodeError::Overflow));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_NONE_CANONICAL, makeError(DecodeError::NoneCanonical));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OUT_OF_MEMORY, makeError(DecodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::decode(const Xi::ByteSpan source, uint16_t &out) {
  auto reval = xi_encoding_varint_decode_uint16(source.data(), static_cast<size_t>(source.size()), &out);
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OVERFLOW, makeError(DecodeError::Overflow));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_NONE_CANONICAL, makeError(DecodeError::NoneCanonical));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OUT_OF_MEMORY, makeError(DecodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::decode(const Xi::ByteSpan source, int32_t &out) {
  auto reval = xi_encoding_varint_decode_int32(source.data(), static_cast<size_t>(source.size()), &out);
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OVERFLOW, makeError(DecodeError::Overflow));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_NONE_CANONICAL, makeError(DecodeError::NoneCanonical));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OUT_OF_MEMORY, makeError(DecodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::decode(const Xi::ByteSpan source, uint32_t &out) {
  auto reval = xi_encoding_varint_decode_uint32(source.data(), static_cast<size_t>(source.size()), &out);
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OVERFLOW, makeError(DecodeError::Overflow));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_NONE_CANONICAL, makeError(DecodeError::NoneCanonical));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OUT_OF_MEMORY, makeError(DecodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::decode(const Xi::ByteSpan source, int64_t &out) {
  auto reval = xi_encoding_varint_decode_int64(source.data(), static_cast<size_t>(source.size()), &out);
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OVERFLOW, makeError(DecodeError::Overflow));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_NONE_CANONICAL, makeError(DecodeError::NoneCanonical));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OUT_OF_MEMORY, makeError(DecodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::decode(const Xi::ByteSpan source, uint64_t &out) {
  auto reval = xi_encoding_varint_decode_uint64(source.data(), static_cast<size_t>(source.size()), &out);
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OVERFLOW, makeError(DecodeError::Overflow));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_NONE_CANONICAL, makeError(DecodeError::NoneCanonical));
  XI_RETURN_EC_IF(reval == XI_VARINT_DECODE_OUT_OF_MEMORY, makeError(DecodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::encode(int8_t value, Xi::ByteSpan dest) {
  auto reval = xi_encoding_varint_encode_int8(value, dest.data(), static_cast<size_t>(dest.size()));
  XI_RETURN_EC_IF(reval == XI_VARINT_ENCODE_OUT_OF_MEMORY, makeError(EncodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::encode(uint8_t value, Xi::ByteSpan dest) {
  auto reval = xi_encoding_varint_encode_uint8(value, dest.data(), static_cast<size_t>(dest.size()));
  XI_RETURN_EC_IF(reval == XI_VARINT_ENCODE_OUT_OF_MEMORY, makeError(EncodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::encode(int16_t value, Xi::ByteSpan dest) {
  auto reval = xi_encoding_varint_encode_int16(value, dest.data(), static_cast<size_t>(dest.size()));
  XI_RETURN_EC_IF(reval == XI_VARINT_ENCODE_OUT_OF_MEMORY, makeError(EncodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::encode(uint16_t value, Xi::ByteSpan dest) {
  auto reval = xi_encoding_varint_encode_uint16(value, dest.data(), static_cast<size_t>(dest.size()));
  XI_RETURN_EC_IF(reval == XI_VARINT_ENCODE_OUT_OF_MEMORY, makeError(EncodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::encode(int32_t value, Xi::ByteSpan dest) {
  auto reval = xi_encoding_varint_encode_int32(value, dest.data(), static_cast<size_t>(dest.size()));
  XI_RETURN_EC_IF(reval == XI_VARINT_ENCODE_OUT_OF_MEMORY, makeError(EncodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::encode(uint32_t value, Xi::ByteSpan dest) {
  auto reval = xi_encoding_varint_encode_uint32(value, dest.data(), static_cast<size_t>(dest.size()));
  XI_RETURN_EC_IF(reval == XI_VARINT_ENCODE_OUT_OF_MEMORY, makeError(EncodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::encode(int64_t value, Xi::ByteSpan dest) {
  auto reval = xi_encoding_varint_encode_int64(value, dest.data(), static_cast<size_t>(dest.size()));
  XI_RETURN_EC_IF(reval == XI_VARINT_ENCODE_OUT_OF_MEMORY, makeError(EncodeError::OutOfMemory));
  return success(reval);
}

Xi::Result<size_t> Xi::Encoding::VarInt::encode(uint64_t value, Xi::ByteSpan dest) {
  auto reval = xi_encoding_varint_encode_uint64(value, dest.data(), static_cast<size_t>(dest.size()));
  XI_RETURN_EC_IF(reval == XI_VARINT_ENCODE_OUT_OF_MEMORY, makeError(EncodeError::OutOfMemory));
  return success(reval);
}

bool Xi::Encoding::VarInt::hasSuccessor(Xi::Byte current) { return xi_encoding_varint_has_successor(current); }
