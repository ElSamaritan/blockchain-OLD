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

#include "CryptoNoteCore/Blockchain/BlockHeaderNonce.hpp"

#include <Common/StringTools.h>

#include <boost/endian/conversion.hpp>

const CryptoNote::BlockHeaderNonce CryptoNote::BlockHeaderNonce::Null{{0, 0, 0, 0}};

bool CryptoNote::serialize(CryptoNote::BlockHeaderNonce &value, Common::StringView name,
                           CryptoNote::ISerializer &serializer) {
  return serializer.binary(value.data(), value.size(), name);
}

void CryptoNote::BlockHeaderNonce::advance(CryptoNote::BlockHeaderNonce::integer_type offset) {
  setAsInteger(asInteger() + offset);
}

void CryptoNote::BlockHeaderNonce::setAsInteger(CryptoNote::BlockHeaderNonce::integer_type value) {
  *reinterpret_cast<integer_type *>(data()) = boost::endian::native_to_little(value);
}

CryptoNote::BlockHeaderNonce::integer_type CryptoNote::BlockHeaderNonce::asInteger() const {
  return boost::endian::little_to_native(*reinterpret_cast<const integer_type *>(data()));
}

std::string CryptoNote::toString(const CryptoNote::BlockHeaderNonce &nonce) {
  return Common::toHex(nonce.data(), nonce.size());
}
