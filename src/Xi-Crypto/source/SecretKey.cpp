﻿/* ============================================================================================== *
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

#include <Xi/Exceptions.hpp>
#include <Xi/Memory/Clear.hh>
#include <Common/StringTools.h>
#include <crypto/crypto-ops.h>
#include <crypto/crypto.h>

#include "Xi/Crypto/SecretKey.hpp"

namespace Xi {
namespace Crypto {

const SecretKey SecretKey::Null{};

Result<SecretKey> SecretKey::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  SecretKey reval;
  if (SecretKey::bytes() * 2 != hex.size()) {
    throw std::runtime_error{"wrong hex size"};
  }
  if (!Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type))) {
    throw std::runtime_error{"invalid hex string"};
  }
  if (!reval.isValid()) {
    throw std::runtime_error{"secret key is invalid"};
  }
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

SecretKey::SecretKey() {
  nullify();
}

SecretKey::SecretKey(SecretKey::array_type raw) : array_type(std::move(raw)) {
  Memory::secureClear(raw);
}

SecretKey::~SecretKey() {
  Memory::secureClear(span());
}

PublicKey SecretKey::toPublicKey() const {
  PublicKey reval{};
  exceptional_if_not<InvalidArgumentError>(::Crypto::secret_key_to_public_key(*this, reval), "invalid secret key");
  exceptional_if_not<InvalidArgumentError>(reval.isValid(), "invalid secret key");
  return reval;
}

std::string SecretKey::toString() const {
  return Common::toHex(data(), size() * sizeof(value_type));
}

bool SecretKey::isNull() const {
  return *this == SecretKey::Null;
}

bool SecretKey::isValid() const {
  return sc_check(data()) == 0;
}

ConstByteSpan SecretKey::span() const {
  return ConstByteSpan{data(), bytes()};
}

ByteSpan SecretKey::span() {
  return ByteSpan{data(), bytes()};
}

void SecretKey::nullify() {
  fill(0);
}

std::ostream &operator<<(std::ostream &stream, const SecretKey &rhs) {
  return stream << rhs.toString();
}

}  // namespace Crypto
}  // namespace Xi

bool Xi::Crypto::serialize(SecretKey &secretKey, Common::StringView name, CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(secretKey.data(), SecretKey::bytes(), name), false);
  return true;
}
