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

#include <optional>
#include <type_traits>

#include <Xi/ExternalIncludePush.h>
#include <boost/utility/value_init.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>

#include "Serialization/ISerializer.h"

namespace CryptoNote {

template <typename _ValueT>
bool serialize(std::optional<_ValueT> &value, Common::StringView name, ISerializer &serializer) {
  using native_t = typename std::remove_cv_t<_ValueT>;
  static_assert(std::is_default_constructible_v<native_t>,
                "optional serialization expects default constructible types");

  XI_RETURN_EC_IF_NOT(serializer.beginObject(name), false);
  bool hasValue = value.has_value();
  XI_RETURN_EC_IF_NOT(serializer(hasValue, "has_value"), false);
  if (serializer.type() == ISerializer::INPUT) {
    if (hasValue) {
      value.emplace();
      XI_RETURN_EC_IF_NOT(serializer(*value, "value"), false);
    } else {
      value = std::nullopt;
    }
  } else if (hasValue) {
    XI_RETURN_EC_IF_NOT(serializer(*value, "value"), false);
  }

  serializer.endObject();
  return true;
}

}  // namespace CryptoNote
