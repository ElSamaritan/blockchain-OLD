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

#include "Xi/Resource/Registry.hpp"

#include <memory>
#include <fstream>

#include <Xi/FileSystem.h>

namespace Xi {
namespace Resource {

Registry::Registry() {
  /* */
}

const Registry &Registry::singleton() {
  static std::unique_ptr<Registry> __singleton{new Registry{}};
  return *__singleton;
}

Registry::~Registry() {
  /* */
}

const Container &Registry::embedded() const {
  return m_embedded;
}
const Registry &registry() {
  return Registry::singleton();
}

const Container &embedded() {
  return registry().embedded();
}

Result<Entity> embedded(const std::string &name, std::optional<Type> expectedType) {
  return embedded()(name, expectedType);
}

Result<std::string> loadText(const std::string &name, const std::string &fileEnding, const std::string &root) {
  XI_ERROR_TRY

  if (isResourcePath(name)) {
    auto res = embedded(name, Type::Text);
    XI_ERROR_PROPAGATE(res)
    return success(res->text());
  } else {
    const auto search = FileSystem::searchFile(name, fileEnding, root);
    XI_ERROR_PROPAGATE(search)
    auto text = FileSystem::readTextFile(*search);
    XI_ERROR_PROPAGATE(text)
    return success(text.take());
  }

  XI_ERROR_CATCH
}

Result<ByteVector> loadBinary(const std::string &name, const std::string &fileEnding, const std::string &root) {
  XI_ERROR_TRY

  if (isResourcePath(name)) {
    auto res = embedded(name, Type::Binary);
    XI_ERROR_PROPAGATE(res)
    return success(res->binary());
  } else {
    const auto search = FileSystem::searchFile(name, fileEnding, root);
    XI_ERROR_PROPAGATE(search)
    auto bin = FileSystem::readBinaryFile(*search);
    XI_ERROR_PROPAGATE(bin)
    return success(bin.take());
  }

  XI_ERROR_CATCH
}

}  // namespace Resource
}  // namespace Xi
