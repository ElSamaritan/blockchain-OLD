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

#include "crypto/crypto-ops.h"
#include "crypto/Types/Signature.h"

const Crypto::Signature Crypto::Signature::Null{};

Xi::Result<Crypto::Signature> Crypto::Signature::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  Signature reval;
  if (Signature::bytes() * 2 != hex.size()) {
    throw std::runtime_error{"wrong hex size"};
  }
  if (!Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type))) {
    throw std::runtime_error{"invalid hex string"};
  }
  if (!reval.isValid()) {
    throw std::runtime_error{"signature is invalid"};
  }
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Crypto::Signature::Signature() { nullify(); }

Crypto::Signature::Signature(Crypto::Signature::array_type raw) : array_type(std::move(raw)) {}

Crypto::Signature::~Signature() {}

std::string Crypto::Signature::toString() const { return Common::toHex(data(), size() * sizeof(value_type)); }

bool Crypto::Signature::isValid() const { return sc_check(data()) == 0 && sc_check(data() + 32) == 0; }

bool Crypto::Signature::isNull() const { return (*this) == Null; }

Xi::ConstByteSpan Crypto::Signature::span() const { return Xi::ConstByteSpan{data(), bytes()}; }

Xi::ByteSpan Crypto::Signature::span() { return Xi::ByteSpan{data(), bytes()}; }

void Crypto::Signature::nullify() { fill(0); }

bool Crypto::serialize(Crypto::Signature &signature, Common::StringView name, CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(signature.data(), Signature::bytes(), name), false);
  XI_RETURN_EC_IF_NOT(signature.isValid(), false);
  return true;
}
