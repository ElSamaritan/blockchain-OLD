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

#include <Common/StringTools.h>

#include "crypto/Types/Signature.h"

const Crypto::Signature Crypto::Signature::Null{};

Xi::Result<Crypto::Signature> Crypto::Signature::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  Signature reval;
  if (!Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type))) {
    throw std::runtime_error{"invalid hex string"};
  }
  return reval;
  XI_ERROR_CATCH();
}

std::string Crypto::Signature::toString() const { return Common::toHex(data(), size() * sizeof(value_type)); }

void Crypto::Signature::nullify() { fill(0); }

void Crypto::Signature::serialize(CryptoNote::ISerializer &serializer) {
  serializer.binary(data(), size() * sizeof(value_type), "blob");
}
