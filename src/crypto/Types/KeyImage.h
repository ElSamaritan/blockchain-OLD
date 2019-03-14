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
#include <unordered_set>

#include <Xi/Global.h>
#include <Xi/Algorithm/GenericHash.h>
#include <Xi/Algorithm/GenericComparison.h>

#include "crypto/Types/Byte.h"

namespace Crypto {
struct KeyImage : std::array<Byte, 32> {
  static const KeyImage Null;

  KeyImage() = default;
  XI_DEFAULT_COPY(KeyImage);
  XI_DEFAULT_MOVE(KeyImage);
  ~KeyImage() = default;
};

using KeyImageSet = std::unordered_set<KeyImage>;

XI_MAKE_GENERIC_HASH_FUNC(KeyImage)
XI_MAKE_GENERIC_COMPARISON(KeyImage)
}  // namespace Crypto

XI_MAKE_GENERIC_HASH_OVERLOAD(Crypto, KeyImage)