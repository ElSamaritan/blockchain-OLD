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
  using array_type = Xi::ByteArray<32>;
  static const PublicKey Null;
  static inline constexpr size_t bytes() {
    return 32;
  }
  static Xi::Result<PublicKey> fromString(const std::string& hex);

  PublicKey();
  explicit PublicKey(array_type raw);
  XI_DEFAULT_COPY(PublicKey);
  XI_DEFAULT_MOVE(PublicKey);
  ~PublicKey();

  std::string toString() const;
  bool isNull() const;

  Xi::ConstByteSpan span() const;
  Xi::ByteSpan span();

  /*
   * \brief checks that this is indeed a valid ecc point and has the right order.
   */
  bool isValid() const;

  void nullify();
};

XI_MAKE_GENERIC_HASH_FUNC(PublicKey)
XI_MAKE_GENERIC_COMPARISON(PublicKey)

[[nodiscard]] bool serialize(PublicKey& publicKey, Common::StringView name, CryptoNote::ISerializer& serializer);

}  // namespace Crypto

XI_MAKE_GENERIC_HASH_OVERLOAD(Crypto, PublicKey)
