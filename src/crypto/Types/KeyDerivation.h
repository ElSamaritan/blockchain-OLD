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
struct KeyDerivation : Xi::ByteArray<32> {
  using array_type = Xi::ByteArray<32>;
  static const KeyDerivation Null;
  static inline constexpr size_t bytes() { return 32; }
  static Xi::Result<KeyDerivation> fromString(const std::string& hex);

  KeyDerivation();
  explicit KeyDerivation(array_type raw);
  XI_DEFAULT_COPY(KeyDerivation);
  XI_DEFAULT_MOVE(KeyDerivation);
  ~KeyDerivation();

  std::string toString() const;

  bool isNull() const;
  bool isValid() const;

  Xi::ConstByteSpan span() const;
  Xi::ByteSpan span();

  void nullify();
};

XI_MAKE_GENERIC_HASH_FUNC(KeyDerivation)
XI_MAKE_GENERIC_COMPARISON(KeyDerivation)

[[nodiscard]] bool serialize(KeyDerivation& keyDerivation, Common::StringView name,
                             CryptoNote::ISerializer& serializer);

}  // namespace Crypto

XI_MAKE_GENERIC_HASH_OVERLOAD(Crypto, KeyDerivation)
