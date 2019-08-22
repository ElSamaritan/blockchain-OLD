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

#include <unordered_map>
#include <string>
#include <optional>

#include <Xi/Global.hh>
#include <Xi/Byte.hh>
#include <Xi/Result.h>
#include <Xi/Concurrent/ReadersWriterLock.h>

#include "Xi/Resource/Entity.hpp"

namespace Xi {
namespace Resource {

/// Manages resource entity emplacement and queries.
class Container {
 public:
  Container();
  XI_DELETE_COPY(Container);
  XI_DEFAULT_MOVE(Container);
  ~Container();

  /*!
   * \brief get Queries a resource.
   * \param name The full name of the resource.
   * \param expectedType The resource type expected, if set returns an error if types do not match.
   * \return An error if the resource was not found otherwise the resource entity.
   */
  Result<Entity> get(const std::string& name, std::optional<Type> expectedType = std::nullopt) const;

  /*!
   * \brief contains Checks whether a resource exists.
   * \param name The full name of the resource.
   * \return True if the resource exists, otherwise false.
   */
  [[nodiscard]] bool contains(const std::string& name) const;

  /*!
   * \brief set Sets a resource for a gaven full name.
   * \param name The resource name.
   * \param entity The resource entity.
   * \return An error if the resource already exists.
   */
  Result<void> set(const std::string& name, const Entity& entity);

  /*!
   * \brief set Sets a resource for a gaven full name.
   * \param name The resource name.
   * \param binary The resource binary representation.
   * \return An error if the resource already exists.
   */
  Result<void> set(const std::string& name, const ByteVector& binary);

  /*!
   * \brief set Sets a resource for a gaven full name.
   * \param name The resource name.
   * \param text The resource text representation.
   * \return An error if the resource already exists.
   */
  Result<void> set(const std::string& name, const std::string& text);

  /*!
   * \brief operator () Queries a resource.
   * \param name The full name of the resource.
   * \param expectedType The resource type expected, if set returns an error if types do not match.
   * \return An error if the resource was not found otwerise the resource entity.
   */
  Result<Entity> operator()(const std::string& name, std::optional<Type> expectedType = std::nullopt) const;

 private:
  Concurrent::ReadersWriterLock m_entitiesGuard{};
  std::unordered_map<std::string, Entity> m_entities{};
};

/// Prefix of every name used to identify it as a resource.
std::string resourcePrefix();

/// Prepends the resource prefix to the path.
std::string resourcePath(const std::string& path);

/// Checks whether the path starts with the resource prefix.
bool isResourcePath(const std::string& path);

}  // namespace Resource
}  // namespace Xi
