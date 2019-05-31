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

#include "crypto/Types/KeyDerivation.h"

const Crypto::KeyDerivation Crypto::KeyDerivation::Null{};

Xi::Result<Crypto::KeyDerivation> Crypto::KeyDerivation::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  KeyDerivation reval;
  if (!Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type))) {
    throw std::runtime_error{"invalid hex string"};
  }
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Crypto::KeyDerivation::KeyDerivation() { nullify(); }

Crypto::KeyDerivation::KeyDerivation(Crypto::KeyDerivation::array_type raw) : array_type(std::move(raw)) {}

Crypto::KeyDerivation::~KeyDerivation() {}

std::string Crypto::KeyDerivation::toString() const { return Common::toHex(data(), size() * sizeof(value_type)); }

Xi::ConstByteSpan Crypto::KeyDerivation::span() const { return Xi::ConstByteSpan{data(), bytes()}; }

Xi::ByteSpan Crypto::KeyDerivation::span() { return Xi::ByteSpan{data(), bytes()}; }

void Crypto::KeyDerivation::nullify() { fill(0); }

bool Crypto::serialize(Crypto::KeyDerivation &keyDerivation, Common::StringView name,
                       CryptoNote::ISerializer &serializer) {
  return serializer.binary(keyDerivation.data(), KeyDerivation::bytes(), name);
}
