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

#include "crypto/Types/KeyImage.h"
#include "crypto/crypto.h"

const Crypto::KeyImage Crypto::KeyImage::Null{};

Xi::Result<Crypto::KeyImage> Crypto::KeyImage::fromString(const std::string &hex) {
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

Crypto::KeyImage::KeyImage() { nullify(); }

Crypto::KeyImage::KeyImage(Crypto::KeyImage::array_type raw) : array_type(std::move(raw)) {}

Crypto::KeyImage::~KeyImage() {}

std::string Crypto::KeyImage::toString() const { return Common::toHex(data(), size() * sizeof(value_type)); }

Xi::ConstByteSpan Crypto::KeyImage::span() const { return Xi::ConstByteSpan{data(), bytes()}; }

Xi::ByteSpan Crypto::KeyImage::span() { return Xi::ByteSpan{data(), bytes()}; }

bool Crypto::KeyImage::isValid() const {
  static const Crypto::KeyImage I{{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
  static const Crypto::KeyImage L{{0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7,
                                   0xa2, 0xde, 0xf9, 0xde, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10}};
  return scalarmultKey(*this, L) == I;
}

bool Crypto::KeyImage::isNull() const { return (*this) == Null; }

void Crypto::KeyImage::nullify() { fill(0); }

bool Crypto::serialize(Crypto::KeyImage &keyImage, Common::StringView name, CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(keyImage.data(), KeyImage::bytes(), name), false);
  XI_RETURN_EC_IF_NOT(keyImage.isValid(), false);
  return true;
}
