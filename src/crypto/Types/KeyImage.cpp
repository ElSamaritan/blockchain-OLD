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

#include "crypto/Types/KeyImage.h"

const Crypto::KeyImage Crypto::KeyImage::Null{};

Xi::Result<Crypto::KeyImage> Crypto::KeyImage::fromString(const std::string &hex) {
  XI_ERROR_TRY();
  KeyImage reval;
  if (!Common::fromHex(hex, reval.data(), reval.size() * sizeof(value_type))) {
    throw std::runtime_error{"invalid hex string"};
  }
  return reval;
  XI_ERROR_CATCH();
}

std::string Crypto::KeyImage::toString() const { return Common::toHex(data(), size() * sizeof(value_type)); }
