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

#include "Xi/Resource/Container.hpp"

#include <Xi/Algorithm/String.h>

#include "Xi/Resource/Error.hpp"

namespace Xi {
namespace Resource {

Container::Container() {
  /* */
}

Container::~Container() {
  /* */
}

Result<Entity> Container::get(const std::string &name, std::optional<Type> expectedType) const {
  XI_CONCURRENT_LOCK_READ(m_entitiesGuard);
  const auto search = m_entities.find(name);
  if (search == m_entities.end()) {
    return makeError(ResourceError::NotFound);
  } else {
    if (expectedType && *expectedType != search->second.type()) {
      return makeError(ResourceError::InvalidType);
    }
    return success(search->second);
  }
}

bool Container::contains(const std::string &name) const {
  XI_CONCURRENT_LOCK_READ(m_entitiesGuard);
  return m_entities.find(name) != m_entities.end();
}

Result<void> Container::set(const std::string &name, const Entity &entity) {
  XI_CONCURRENT_LOCK_PREPARE_WRITE(m_entitiesGuard);
  if (contains(name)) {
    return makeError(ResourceError::AlreadyExists);
  } else {
    XI_CONCURRENT_LOCK_ACQUIRE_WRITE(m_entitiesGuard);
    m_entities[name] = entity;
    return success();
  }
}

Result<void> Container::set(const std::string &name, const ByteVector &binary) {
  return set(name, Entity{binary});
}

Result<void> Container::set(const std::string &name, const std::string &text) {
  return set(name, Entity{text});
}

Result<Entity> Container::operator()(const std::string &name, std::optional<Type> expectedType) const {
  return get(name, expectedType);
}

std::string resourcePrefix() {
  return "xrc://";
}

bool isResourcePath(const std::string &path) {
  return starts_with(path, resourcePrefix());
}

std::string resourcePath(const std::string &path) {
  return resourcePrefix() + path;
}

}  // namespace Resource
}  // namespace Xi
