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

#include <stdexcept>
#include <utility>

#include <Common/StringTools.h>

#include "crypto/Types/SecretKey.h"

const Crypto::SecretKey Crypto::SecretKey::Null{};

Xi::Result<Crypto::SecretKey> Crypto::SecretKey::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  SecretKey reval;
  if (!Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type))) {
    throw std::runtime_error{"invalid hex string"};
  }
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Crypto::SecretKey::SecretKey() { nullify(); }

Crypto::SecretKey::SecretKey(Crypto::SecretKey::array_type raw) : array_type(std::move(raw)) {}

Crypto::SecretKey::~SecretKey() {}

std::string Crypto::SecretKey::toString() const { return Common::toHex(data(), size() * sizeof(value_type)); }

bool Crypto::SecretKey::isNull() const { return *this == SecretKey::Null; }

Xi::ConstByteSpan Crypto::SecretKey::span() const { return Xi::ConstByteSpan{data(), bytes()}; }

Xi::ByteSpan Crypto::SecretKey::span() { return Xi::ByteSpan{data(), bytes()}; }

void Crypto::SecretKey::nullify() { fill(0); }

bool Crypto::serialize(Crypto::SecretKey &secretKey, Common::StringView name, CryptoNote::ISerializer &serializer) {
  return serializer.binary(secretKey.data(), SecretKey::bytes(), name);
}
