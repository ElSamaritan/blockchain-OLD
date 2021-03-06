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
#include <unordered_set>
#include <string>
#include <unordered_set>
#include <vector>
#include <ostream>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/Byte.hh>
#include <Xi/Span.hpp>
#include <Serialization/ISerializer.h>
#include <Xi/Algorithm/GenericHash.h>
#include <Xi/Algorithm/GenericComparison.h>

namespace Xi {
namespace Crypto {
struct KeyImage : Xi::ByteArray<32> {
  using array_type = Xi::ByteArray<32>;
  static const KeyImage Null;
  static inline constexpr size_t bytes() {
    return 32;
  }
  static Xi::Result<KeyImage> fromString(const std::string& hex);

  KeyImage();
  explicit KeyImage(array_type raw);
  XI_DEFAULT_COPY(KeyImage);
  XI_DEFAULT_MOVE(KeyImage);
  ~KeyImage();

  std::string toString() const;

  Xi::ConstByteSpan span() const;
  Xi::ByteSpan span();

  /*
   * \brief isValid checks the domain of the image
   *
   * \return true if the key image is in the correct domain, otherwise false.
   *
   */
  bool isValid() const;
  bool isNull() const;

  void nullify();
};

using KeyImageSet = std::unordered_set<KeyImage>;
XI_DECLARE_SPANS(KeyImage)

XI_MAKE_GENERIC_HASH_FUNC(KeyImage)
XI_MAKE_GENERIC_COMPARISON(KeyImage)

[[nodiscard]] bool serialize(KeyImage& keyImage, Common::StringView name, CryptoNote::ISerializer& serializer);

std::ostream& operator<<(std::ostream& stream, const KeyImage& rhs);

}  // namespace Crypto
}  // namespace Xi

XI_MAKE_GENERIC_HASH_OVERLOAD(Xi::Crypto, KeyImage)

namespace Crypto {

using KeyImage = Xi::Crypto::KeyImage;
using KeyImageSet = std::unordered_set<KeyImage>;
using KeyImageVector = std::vector<KeyImage>;
XI_DECLARE_SPANS(KeyImage)

}  // namespace Crypto
