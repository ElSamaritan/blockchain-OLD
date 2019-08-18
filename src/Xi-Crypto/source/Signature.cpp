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

#include "Xi/Crypto/Signature.hpp"

namespace Xi {
namespace Crypto {

const Signature Signature::Null{};

Result<Signature> Signature::fromString(const std::string &hex) {
  XI_ERROR_TRY
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
  XI_ERROR_CATCH
}

Signature::Signature() {
  nullify();
}

Signature::Signature(Signature::array_type raw) : array_type(std::move(raw)) {
}

Signature::~Signature() {
}

std::string Signature::toString() const {
  return Common::toHex(data(), size() * sizeof(value_type));
}

bool Signature::isValid() const {
  return sc_check(data()) == 0 && sc_check(data() + 32) == 0;
}

bool Signature::isNull() const {
  return (*this) == Null;
}

ConstByteSpan Signature::span() const {
  return ConstByteSpan{data(), bytes()};
}

ByteSpan Signature::span() {
  return ByteSpan{data(), bytes()};
}

void Signature::nullify() {
  fill(0);
}

std::ostream &operator<<(std::ostream &stream, const Signature &rhs) {
  return stream << rhs.toString();
}

}  // namespace Crypto
}  // namespace Xi

bool Xi::Crypto::serialize(Signature &signature, Common::StringView name, CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(signature.data(), Signature::bytes(), name), false);
  return true;
}
