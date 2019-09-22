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

#include <variant>
#include <string>

#include <Xi/Global.hh>
#include <Xi/Byte.hh>

#include "Xi/Resource/Type.hpp"

namespace Xi {
namespace Resource {

/// Wraps an embedded data storage as variant text or binary type.
class Entity {
 public:
  explicit Entity();
  XI_DEFAULT_COPY(Entity);

  /// Queries the data type.
  Type type() const;
  /// Returns a reference to the embedded binary data.
  const ByteVector& binary() const;
  /// Returns a reference to the embedded text data.
  const std::string& text() const;

 private:
  explicit Entity(const ByteVector& binary);
  explicit Entity(const std::string& text);

  friend class Container;

 private:
  using embedded_binary_storage = const ByteVector*;
  using embedded_text_storage = const std::string*;

  using null_storage = std::nullptr_t;
  std::variant<embedded_binary_storage, embedded_text_storage, null_storage> m_data;
};

}  // namespace Resource
}  // namespace Xi
