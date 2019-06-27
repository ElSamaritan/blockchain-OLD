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

#include <Xi/Global.hh>

#include "Serialization/ISerializer.h"

namespace CryptoNote {

/*!
 * \section Serialization
 *
 * \attention Binary blobs should be serialized using ISerializer::binary, otherwise thiis serialization method may make
 * an array for a blob in its representation. Ie. for Json [ 122, 10 ].
 */

template <typename _ValueT, size_t _SizeV>
[[nodiscard]] bool serialize(std::array<_ValueT, _SizeV>& value, Common::StringView name, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer.beginStaticArray(_SizeV, name), false);
  for (size_t i = 0; i < _SizeV; ++i) {
    XI_RETURN_EC_IF_NOT(serializer(value[i], ""), false);
  }
  XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
}

template <typename _ValueT, size_t _SizeV>
[[nodiscard]] bool serialize(_ValueT value[_SizeV], Common::StringView name, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer.beginStaticArray(_SizeV, name), false);
  for (size_t i = 0; i < _SizeV; ++i) {
    XI_RETURN_EC_IF_NOT(serializer(value[i], ""), false);
  }
  XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
}

}  // namespace CryptoNote
