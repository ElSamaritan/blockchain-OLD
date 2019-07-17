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

#include "crypto/Types/PublicKey.h"
#include "crypto/crypto.h"

const Crypto::PublicKey Crypto::PublicKey::Null{};

Xi::Result<Crypto::PublicKey> Crypto::PublicKey::fromString(const std::string &hex) {
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

Crypto::PublicKey::PublicKey() {
  nullify();
}

Crypto::PublicKey::PublicKey(Crypto::PublicKey::array_type raw) : array_type(std::move(raw)) {
}

Crypto::PublicKey::~PublicKey() {
}

std::string Crypto::PublicKey::toString() const {
  return Common::toHex(data(), size() * sizeof(value_type));
}

bool Crypto::PublicKey::isNull() const {
  return *this == PublicKey::Null;
}

Xi::ConstByteSpan Crypto::PublicKey::span() const {
  return Xi::ConstByteSpan{data(), bytes()};
}

Xi::ByteSpan Crypto::PublicKey::span() {
  return Xi::ByteSpan{data(), bytes()};
}

bool Crypto::PublicKey::isValid() const {
  return check_key(*this) && !isNull();
}

void Crypto::PublicKey::nullify() {
  fill(0);
}

bool Crypto::serialize(Crypto::PublicKey &publicKey, Common::StringView name, CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(publicKey.data(), PublicKey::bytes(), name), false);
  XI_RETURN_SC(true);
}
