﻿/* ============================================================================================== *
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
#include <vector>
#include <ostream>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/Byte.hh>
#include <Xi/Span.hpp>
#include <Serialization/ISerializer.h>
#include <Xi/Algorithm/GenericHash.h>
#include <Xi/Algorithm/GenericComparison.h>

#include "Xi/Crypto/PublicKey.hpp"

namespace Xi {
namespace Crypto {
struct SecretKey : Xi::ByteArray<32> {
  using array_type = Xi::ByteArray<32>;
  static const SecretKey Null;
  static inline constexpr size_t bytes() {
    return 32;
  }
  static Xi::Result<SecretKey> fromString(const std::string& hex);

  SecretKey();
  explicit SecretKey(array_type raw);
  XI_DEFAULT_COPY(SecretKey);
  XI_DEFAULT_MOVE(SecretKey);
  ~SecretKey();

  /*!
   * \brief toPublicKey converts the secret key to its corresponding public key.
   * \return the public key counter part for this secret key.
   * \throws InvalidArgumentError iff this secret key is invalid.
   */
  PublicKey toPublicKey() const;

  std::string toString() const;
  bool isNull() const;

  bool isValid() const;

  Xi::ConstByteSpan span() const;
  Xi::ByteSpan span();

  void nullify();
};

XI_MAKE_GENERIC_HASH_FUNC(SecretKey)
XI_MAKE_GENERIC_COMPARISON(SecretKey)

[[nodiscard]] bool serialize(SecretKey& secretKey, Common::StringView name, CryptoNote::ISerializer& serializer);

std::ostream& operator<<(std::ostream& stream, const SecretKey& rhs);

}  // namespace Crypto
}  // namespace Xi

XI_MAKE_GENERIC_HASH_OVERLOAD(Xi::Crypto, SecretKey)

namespace Crypto {

using SecretKey = Xi::Crypto::SecretKey;
using SecretKeyVector = std::vector<SecretKey>;
XI_DECLARE_SPANS(SecretKey)

}  // namespace Crypto
