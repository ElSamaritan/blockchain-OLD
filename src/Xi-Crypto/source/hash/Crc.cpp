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

#include "Xi/Crypto/Hash/Crc.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4701)
#endif

#include <boost/crc.hpp>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <boost/endian/conversion.hpp>

#include <Xi/Exceptions.hpp>

XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Crc::Hash16, 16)
XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Crc::Hash32, 32)

void Xi::Crypto::Hash::Crc::compute(Xi::ConstByteSpan data, Hash16 &out) {
  static_assert(sizeof(boost::crc_16_type::value_type) == Hash16::bytes(), "invalid hash size for boost crc");
  boost::crc_16_type result{};
  result.process_bytes(data.data(), data.size());
  *reinterpret_cast<boost::crc_16_type::value_type *>(out.data()) = boost::endian::native_to_big(result.checksum());
}

void Xi::Crypto::Hash::Crc::compute(Xi::ConstByteSpan data, Hash32 &out) {
  static_assert(sizeof(boost::crc_32_type::value_type) == Hash32::bytes(), "invalid hash size for boost crc");
  boost::crc_32_type result{};
  result.process_bytes(data.data(), data.size());
  *reinterpret_cast<boost::crc_32_type::value_type *>(out.data()) = boost::endian::native_to_big(result.checksum());
}

Xi::Crypto::Hash::Crc::Hash16 Xi::Crypto::Hash::crc16(Xi::ConstByteSpan data) {
  Crc::Hash16 reval;
  compute(data, reval);
  return reval;
}

Xi::Crypto::Hash::Crc::Hash32 Xi::Crypto::Hash::crc32(Xi::ConstByteSpan data) {
  Crc::Hash32 reval;
  compute(data, reval);
  return reval;
}
