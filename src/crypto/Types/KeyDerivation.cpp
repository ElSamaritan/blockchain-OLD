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

#include "crypto/Types/KeyDerivation.h"
#include "crypto/crypto-ops.h"

const Crypto::KeyDerivation Crypto::KeyDerivation::Null{};

Xi::Result<Crypto::KeyDerivation> Crypto::KeyDerivation::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  KeyDerivation reval;
  if (KeyDerivation::bytes() * 2 != hex.size()) {
    throw std::runtime_error{"wrong hex size"};
  }
  if (!Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type))) {
    throw std::runtime_error{"invalid hex string"};
  }
  if (!reval.isValid()) {
    throw std::runtime_error{"key invalid"};
  }

  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Crypto::KeyDerivation::KeyDerivation() { nullify(); }

Crypto::KeyDerivation::KeyDerivation(Crypto::KeyDerivation::array_type raw) : array_type(std::move(raw)) {}

Crypto::KeyDerivation::~KeyDerivation() {}

std::string Crypto::KeyDerivation::toString() const { return Common::toHex(data(), size() * sizeof(value_type)); }

bool Crypto::KeyDerivation::isNull() const { return (*this) == Null; }

bool Crypto::KeyDerivation::isValid() const {
  ge_p3 p;
  return ge_frombytes_vartime(&p, data()) == 0;
}

Xi::ConstByteSpan Crypto::KeyDerivation::span() const { return Xi::ConstByteSpan{data(), bytes()}; }

Xi::ByteSpan Crypto::KeyDerivation::span() { return Xi::ByteSpan{data(), bytes()}; }

void Crypto::KeyDerivation::nullify() { fill(0); }

bool Crypto::serialize(Crypto::KeyDerivation &keyDerivation, Common::StringView name,
                       CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(keyDerivation.data(), KeyDerivation::bytes(), name), false);
  XI_RETURN_EC_IF_NOT(keyDerivation.isValid(), false);
  return true;
}
