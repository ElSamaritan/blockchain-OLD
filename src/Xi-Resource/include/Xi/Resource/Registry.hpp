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

#include <string>
#include <optional>

#include <Xi/Global.hh>
#include <Xi/Byte.hh>

#include "Xi/Resource/Container.hpp"

namespace Xi {
namespace Resource {

class Registry {
 private:
  Registry();

 public:
  static const Registry& singleton();

 public:
  ~Registry();

  const Container& embedded() const;

 private:
  Container m_embedded{};
};

/// Singleton resource registry.
const Registry& registry();

/// Singleton embedded container.
const Container& embedded();

/*!
 * \brief embedded Queries a resource in the embedded resource container.
 * \param name The full resrouce name.
 * \param expectedType The resource type expected, if set returns an error if types do not match.
 * \return An error if not found, otherwise the resource entity.
 */
Result<Entity> embedded(const std::string& name, std::optional<Type> expectedType = std::nullopt);

/*!
 * \brief load Queries a text resource from ebedded resources or the file system.
 * \param name The resource name or path.
 * \param fileEnding An option file ending to consider if searched in the file system.
 * \param root An optional directory path considered for searching.
 * \return The loaded resource on success otherwise an error.
 *
 * \attention Will not query web reources.
 */
Result<std::string> loadText(const std::string& name, const std::string& fileEnding = "", const std::string& root = "");

/*!
 * \brief load Queries a binary resource from ebedded resources or the file system.
 * \param name The resource name or path.
 * \param fileEnding An option file ending to consider if searched in the file system.
 * \param root An optional directory path considered for searching.
 * \return The loaded resource on success otherwise an error.
 *
 * \attention Will not query web reources.
 */
Result<ByteVector> loadBinary(const std::string& name, const std::string& fileEnding = "",
                              const std::string& root = "");

}  // namespace Resource
}  // namespace Xi
