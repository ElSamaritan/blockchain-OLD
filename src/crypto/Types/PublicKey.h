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

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/Byte.hh>
#include <Serialization/ISerializer.h>
#include <Xi/Algorithm/GenericHash.h>
#include <Xi/Algorithm/GenericComparison.h>

namespace Crypto {
struct PublicKey : Xi::ByteArray<32> {
  static const PublicKey Null;

  static Xi::Result<PublicKey> fromString(const std::string& hex);

  PublicKey() = default;
  XI_DEFAULT_COPY(PublicKey);
  XI_DEFAULT_MOVE(PublicKey);
  ~PublicKey() = default;

  std::string toString() const;

  /*
   * \brief checks that this is indeed a valid ecc point and has the right order.
   */
  bool isValid() const;

  void nullify();
  void serialize(CryptoNote::ISerializer& serializer);
};

XI_MAKE_GENERIC_HASH_FUNC(PublicKey)
XI_MAKE_GENERIC_COMPARISON(PublicKey)
}  // namespace Crypto

XI_MAKE_GENERIC_HASH_OVERLOAD(Crypto, PublicKey)
