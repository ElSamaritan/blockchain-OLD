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

#include <Xi/Global.hh>
#include <Serialization/EnumSerialization.hpp>

namespace Xi {
namespace Resource {

/// The type of a resource embedded.
enum struct Type {
  /// The file was embedded using a binary representation.
  Binary = 1,
  /// The file was embedded as a text file.
  Text,
};

XI_SERIALIZATION_ENUM(Type)

}  // namespace Resource
}  // namespace Xi

XI_SERIALIZATION_ENUM_RANGE(Xi::Resource::Type, Binary, Text)
XI_SERIALIZATION_ENUM_TAG(Xi::Resource::Type, Binary, "binary")
XI_SERIALIZATION_ENUM_TAG(Xi::Resource::Type, Text, "text")
