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

#include <type_traits>

#include <Xi/Blob.hpp>
#include <Serialization/ISerializer.h>

namespace CryptoNote {

struct BlockHeaderNonce : Xi::enable_blob_from_this<BlockHeaderNonce, 4> {
  using enable_blob_from_this::enable_blob_from_this;
  using integer_type = uint32_t;
  using signed_integer_type = std::make_signed_t<integer_type>;

  static const BlockHeaderNonce Null;

  void advance(integer_type offset);

  void setAsInteger(integer_type value);
  integer_type asInteger() const;
};

std::string toString(const BlockHeaderNonce& nonce);

[[nodiscard]] bool serialize(BlockHeaderNonce& value, Common::StringView name, ISerializer& serializer);

}  // namespace CryptoNote
