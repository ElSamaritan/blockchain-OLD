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

#include "Xi/Crypto/KeyImage.hpp"
#include "crypto/crypto-ops.h"

namespace Xi {
namespace Crypto {

const KeyImage KeyImage::Null{};

Result<KeyImage> KeyImage::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  KeyImage reval;
  if (KeyImage::bytes() * 2 != hex.size()) {
    throw std::runtime_error{"wrong hex size"};
  }
  if (!Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type))) {
    throw std::runtime_error{"invalid hex string"};
  }
  if (!reval.isValid()) {
    throw std::runtime_error{"key image is invalid"};
  }
  return success(std::move(reval));
  XI_ERROR_CATCH();
}

KeyImage::KeyImage() {
  nullify();
}

KeyImage::KeyImage(KeyImage::array_type raw) : array_type(std::move(raw)) {
}

KeyImage::~KeyImage() {
}

std::string KeyImage::toString() const {
  return Common::toHex(data(), size() * sizeof(value_type));
}

Xi::ConstByteSpan KeyImage::span() const {
  return Xi::ConstByteSpan{data(), bytes()};
}

Xi::ByteSpan KeyImage::span() {
  return Xi::ByteSpan{data(), bytes()};
}

bool KeyImage::isValid() const {
  ge_p3 point;
  XI_RETURN_EC_IF_NOT(ge_frombytes_vartime(&point, data()) == 0, false);
  ge_dsmp image_dsm;
  ge_dsm_precomp(image_dsm, &point);
  XI_RETURN_EC_IF_NOT(ge_check_subgroup_precomp_vartime(image_dsm) == 0, false);
  XI_RETURN_SC(true);
}

bool KeyImage::isNull() const {
  return (*this) == Null;
}

void KeyImage::nullify() {
  fill(0);
}

std::ostream &operator<<(std::ostream &stream, const KeyImage &rhs) {
  return stream << rhs.toString();
}

}  // namespace Crypto
}  // namespace Xi

bool Xi::Crypto::serialize(KeyImage &keyImage, Common::StringView name, CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(keyImage.data(), KeyImage::bytes(), name), false);
  return true;
}
