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

#include <type_traits>

#include <Xi/Blob.hpp>
#include <Serialization/ISerializer.h>

namespace Xi {
namespace Blockchain {
namespace Block {

struct Nonce : Xi::enable_blob_from_this<Nonce, 4> {
  using enable_blob_from_this::enable_blob_from_this;
  using integer_type = uint32_t;
  using signed_integer_type = std::make_signed_t<integer_type>;

  static const Nonce Null;

  void advance(integer_type offset);

  void setAsInteger(integer_type value);
  integer_type asInteger() const;
};

std::string toString(const Nonce& nonce);

[[nodiscard]] bool serialize(Nonce& value, Common::StringView name, CryptoNote::ISerializer& serializer);

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi
