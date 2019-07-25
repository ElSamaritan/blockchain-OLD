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

#include <stdexcept>
#include <utility>

#include <Common/StringTools.h>

#include "Xi/Crypto/PublicKey.hpp"
#include "crypto/crypto.h"

namespace Xi {
namespace Crypto {

const PublicKey PublicKey::Null{};

Result<PublicKey> PublicKey::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  PublicKey reval;
  if (PublicKey::bytes() * 2 != hex.size()) {
    throw std::runtime_error{"wrong hex size"};
  }
  if (!Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type))) {
    throw std::runtime_error{"invalid hex string"};
  }
  if (!reval.isValid()) {
    throw std::runtime_error{"public key is invalid"};
  }
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

PublicKey::PublicKey() {
  nullify();
}

PublicKey::PublicKey(PublicKey::array_type raw) : array_type(std::move(raw)) {
}

PublicKey::~PublicKey() {
}

std::string PublicKey::toString() const {
  return Common::toHex(data(), size() * sizeof(value_type));
}

bool PublicKey::isNull() const {
  return *this == PublicKey::Null;
}

Xi::ConstByteSpan PublicKey::span() const {
  return Xi::ConstByteSpan{data(), bytes()};
}

Xi::ByteSpan PublicKey::span() {
  return Xi::ByteSpan{data(), bytes()};
}

bool PublicKey::isValid() const {
  return ::Crypto::check_key(*this) && !isNull();
}

void PublicKey::nullify() {
  fill(0);
}

}  // namespace Crypto

std::ostream &Crypto::operator<<(std::ostream &stream, const Crypto::PublicKey &key) {
  return stream << key.toString();
}

bool Crypto::serialize(PublicKey &publicKey, Common::StringView name, CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(publicKey.data(), PublicKey::bytes(), name), false);
  XI_RETURN_SC(true);
}

}  // namespace Xi
