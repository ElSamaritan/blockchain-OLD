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
#include <cstring>
#include <utility>

#include <Xi/FileSystem.h>
#include <Xi/Http/Client.h>
#include <Xi/Encoding/Base64.h>

namespace {
Xi::Http::Response loadWebResource(const std::string url) {
  auto http = std::make_unique<Xi::Http::Client>(Xi::Http::SSLConfiguration::RootStoreClient);
  auto res = http->getSync(url, Xi::Http::ContentType::Text, "");
  Xi::exceptional_if_not<Xi::RuntimeError>(isSuccessCode(res.status()),
                                           "Resolving web resource '{}' yielded none success code {}.", url,
                                           toString(res.status()));
  return res;
}
}  // namespace

namespace Xi {
namespace Resource {

struct Registry::_Impl {
  Container embedded{};
};

Registry::Registry() : m_impl{new _Impl} {
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
  return m_impl->embedded;
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
    if (const auto uri = Network::Uri::fromString(name); uri.isValue()) {
      const auto protocol = uri->protocol();
      if (protocol.isValue() && isHttpBased(*protocol)) {
        return success(loadWebResource(name).takeBody());
      }
    }

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
    if (const auto uri = Network::Uri::fromString(name); uri.isValue()) {
      const auto protocol = uri->protocol();
      if (protocol.isValue() && isHttp(*protocol)) {
        auto httpRes = loadWebResource(name);
        auto decoded = Base64::decode(httpRes.body());
        ByteVector reval{};
        reval.resize(decoded.size());
        std::memcpy(reval.data(), decoded.data(), reval.size());
        return success(std::move(reval));
      }
    }

    const auto search = FileSystem::searchFile(name, fileEnding, root);
    XI_ERROR_PROPAGATE(search)
    auto binary = FileSystem::readBinaryFile(*search);
    XI_ERROR_PROPAGATE(binary)
    return success(binary.take());
  }

  XI_ERROR_CATCH
}

}  // namespace Resource
}  // namespace Xi
