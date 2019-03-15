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

#pragma once

#include <array>
#include <string>

#include <Xi/Global.h>
#include <Xi/Result.h>
#include <Xi/Algorithm/GenericHash.h>
#include <Xi/Algorithm/GenericComparison.h>

#include "crypto/Types/Byte.h"

namespace Crypto {
struct Signature : std::array<Byte, 64> {
  static const Signature Null;

  static Xi::Result<Signature> fromString(const std::string& hex);

  Signature() = default;
  XI_DEFAULT_COPY(Signature);
  XI_DEFAULT_MOVE(Signature);
  ~Signature() = default;

  std::string toString() const;
};

XI_MAKE_GENERIC_HASH_FUNC(Signature)
XI_MAKE_GENERIC_COMPARISON(Signature)
}  // namespace Crypto

XI_MAKE_GENERIC_HASH_OVERLOAD(Crypto, Signature)
