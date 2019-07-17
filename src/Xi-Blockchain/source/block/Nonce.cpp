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

#include "Xi/Blockchain/Block/Nonce.hpp"

#include <Common/StringTools.h>

#include <boost/endian/conversion.hpp>

const Xi::Blockchain::Block::Nonce Xi::Blockchain::Block::Nonce::Null{{0, 0, 0, 0}};

bool Xi::Blockchain::Block::serialize(Xi::Blockchain::Block::Nonce &value, Common::StringView name,
                                      CryptoNote::ISerializer &serializer) {
  return serializer.binary(value.data(), value.size(), name);
}

void Xi::Blockchain::Block::Nonce::advance(Xi::Blockchain::Block::Nonce::integer_type offset) {
  setAsInteger(asInteger() + offset);
}

void Xi::Blockchain::Block::Nonce::setAsInteger(Xi::Blockchain::Block::Nonce::integer_type value) {
  *reinterpret_cast<integer_type *>(data()) = boost::endian::native_to_little(value);
}

Xi::Blockchain::Block::Nonce::integer_type Xi::Blockchain::Block::Nonce::asInteger() const {
  return boost::endian::little_to_native(*reinterpret_cast<const integer_type *>(data()));
}

std::string Xi::Blockchain::Block::toString(const Xi::Blockchain::Block::Nonce &nonce) {
  return Common::toHex(nonce.data(), nonce.size());
}
