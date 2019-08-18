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

#include "Xi/Crypto/KeyDerivation.hpp"
#include "crypto/crypto-ops.h"

namespace Xi {
namespace Crypto {

const KeyDerivation KeyDerivation::Null{};

Result<Crypto::KeyDerivation> KeyDerivation::fromString(const std::string &hex) {
  XI_ERROR_TRY
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
  XI_ERROR_CATCH
}

KeyDerivation::KeyDerivation() {
  nullify();
}

KeyDerivation::KeyDerivation(KeyDerivation::array_type raw) : array_type(std::move(raw)) {
}

KeyDerivation::~KeyDerivation() {
}

std::string KeyDerivation::toString() const {
  return Common::toHex(data(), size() * sizeof(value_type));
}

bool KeyDerivation::isNull() const {
  return (*this) == Null;
}

bool KeyDerivation::isValid() const {
  ge_p3 point;
  XI_RETURN_EC_IF_NOT(ge_frombytes_vartime(&point, data()) == 0, false);
  ge_dsmp image_dsm;
  ge_dsm_precomp(image_dsm, &point);
  XI_RETURN_EC_IF_NOT(ge_check_subgroup_precomp_vartime(image_dsm) == 0, false);
  XI_RETURN_SC(true);
}

Xi::ConstByteSpan KeyDerivation::span() const {
  return Xi::ConstByteSpan{data(), bytes()};
}

Xi::ByteSpan KeyDerivation::span() {
  return Xi::ByteSpan{data(), bytes()};
}

void KeyDerivation::nullify() {
  fill(0);
}

std::ostream &operator<<(std::ostream &stream, const KeyDerivation &rhs) {
  return stream << rhs.toString();
}

}  // namespace Crypto
}  // namespace Xi

bool Xi::Crypto::serialize(Crypto::KeyDerivation &keyDerivation, Common::StringView name,
                           CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(keyDerivation.data(), KeyDerivation::bytes(), name), false);
  return true;
}
