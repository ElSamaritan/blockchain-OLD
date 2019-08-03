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

#include "Xi/App/Environment.h"

#include <cstdlib>
#include <string>
#include <type_traits>
#include <algorithm>
#include <fstream>
#include <regex>
#include <vector>

#include <Xi/ExternalIncludePush.h>
#include <boost/algorithm/string.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Version/BuildInfo.h>
#include <Xi/Exceptions.hpp>
#include <Xi/Algorithm/String.h>
#include <Xi/FileSystem.h>

#include <iostream>

namespace Xi {
namespace App {

namespace {

bool toBoolean(std::string str) {
  str = Xi::trim(Xi::to_lower(str));
  if (str == "true" || str == "yes" || str == "1" || str == "on") {
    return true;
  } else if (str == "false" || str == "no" || str == "0" || str == "off") {
    return false;
  } else {
    exceptional<InvalidArgumentError>(std::string{"Unrecognized boolean string representation: "} + str);
  }
}

std::string toString(std::string str) {
  return str;
}

std::vector<std::string> toStringVector(std::string str) {
  return Xi::split(str, ",; ");
}

template <typename _IntegerT>
_IntegerT toInteger(std::string str) {
  if constexpr (std::is_unsigned_v<_IntegerT>) {
    auto value = std::stoull(str);
    if constexpr (!std::is_same_v<_IntegerT, uint64_t>) {
      if (value > std::numeric_limits<_IntegerT>::max()) {
        Xi::exceptional<OutOfRangeError>();
      }
    }
    return static_cast<_IntegerT>(value);
  } else {
    auto value = std::stoll(str);
    if constexpr (!std::is_same_v<_IntegerT, int64_t>) {
      if (value > std::numeric_limits<_IntegerT>::max()) {
        Xi::exceptional<OutOfRangeError>();
      }
      if (value < std::numeric_limits<_IntegerT>::max()) {
        Xi::exceptional<OutOfRangeError>();
      }
    }
    return static_cast<_IntegerT>(value);
  }
}
}  // namespace

#define XI_ENV_CONSIDER_CASE(CONVERT, GETTER)          \
  if (auto envValue = GETTER(id); !envValue.empty()) { \
    value = CONVERT(envValue);                         \
    return *this;                                      \
  }

#define XI_ENV_CONSIDER(CONVERT)           \
  XI_ENV_CONSIDER_CASE(CONVERT, globalApp) \
  XI_ENV_CONSIDER_CASE(CONVERT, localApp)  \
  XI_ENV_CONSIDER_CASE(CONVERT, global)    \
  XI_ENV_CONSIDER_CASE(CONVERT, local)     \
  return *this;

std::string Environment::get(const std::string &key) {
  auto value = std::getenv(key.c_str());
  if (value == nullptr) {
    return std::string{};
  } else {
    return std::string{value};
  }
}

void Environment::set(const std::string &value) {
  auto text = value;
  static const std::regex env_re{R"--(\$\{([^}]+)\})--"};
  std::smatch match;
  while (std::regex_search(text, match, env_re)) {
    auto const from = match[0];
    const std::string var_name{match[1].first, match[1].second};
    text.replace(from.first, from.second, get(var_name));
  }

#if defined(WIN32)
  _putenv(text.c_str());
#else
  std::vector<char> rawText{};
  rawText.resize(text.size() + 1, '\0');
  std::memcpy(rawText.data(), text.data(), text.size());
  putenv(rawText.data());
#endif
}

Environment::Environment(const std::string &globalPrefix, const std::string &appPrefix)
    : m_globalPrefix{globalPrefix}, m_appPrefix{appPrefix} {
  /* */
}

Environment &Environment::operator()(bool &value, const std::string &id) {
  XI_ENV_CONSIDER(toBoolean);
}

Environment &Environment::operator()(int8_t &value, const std::string &id) {
  XI_ENV_CONSIDER(toInteger<int8_t>);
}

Environment &Environment::operator()(uint8_t &value, const std::string &id) {
  XI_ENV_CONSIDER(toInteger<uint8_t>);
}

Environment &Environment::operator()(int16_t &value, const std::string &id) {
  XI_ENV_CONSIDER(toInteger<int16_t>);
}

Environment &Environment::operator()(uint16_t &value, const std::string &id) {
  XI_ENV_CONSIDER(toInteger<uint16_t>);
}

Environment &Environment::operator()(int32_t &value, const std::string &id) {
  XI_ENV_CONSIDER(toInteger<int32_t>);
}

Environment &Environment::operator()(uint32_t &value, const std::string &id) {
  XI_ENV_CONSIDER(toInteger<uint32_t>);
}

Environment &Environment::operator()(int64_t &value, const std::string &id) {
  XI_ENV_CONSIDER(toInteger<int64_t>);
}

Environment &Environment::operator()(uint64_t &value, const std::string &id) {
  XI_ENV_CONSIDER(toInteger<uint64_t>);
}

Environment &Environment::operator()(std::string &value, const std::string &id) {
  XI_ENV_CONSIDER(toString);
}

Environment &Environment::operator()(std::vector<std::string> &value, const std::string &id) {
  XI_ENV_CONSIDER(toStringVector);
}

#undef XI_ENV_CONSIDER
#undef XI_ENV_CONSIDER_CASE

void Environment::load() {
  if (const auto envEnv = get(m_globalPrefix + "_ENV"); !envEnv.empty()) {
    load(envEnv, true);
  }
  load(".env");
}

void Environment::load(const std::string &env, bool userProvided) {
  {
    auto envFiles = split(env, ",");
    if (envFiles.size() > 1) {
      for (const auto &envfile : envFiles) {
        load(envfile);
      }
      return;
    }
  }

  std::string envFile{};
  if (auto maybeEnv = FileSystem::searchFile(env, ".env"); maybeEnv.isError()) {
    if (userProvided) {
      maybeEnv.throwOnError();
    }
    return;
  } else {
    envFile = maybeEnv.take();
  }
  std::ifstream stream{envFile};
  if (!stream.good()) {
    return;
  }

  std::string line{};
  while (std::getline(stream, line)) {
    line = trim(line);
    if (starts_with(line, "#")) {
      continue;
    }
    set(line);
  }
}

std::string Environment::global(const std::string &id) {
  std::string globalAppId = m_globalPrefix + "_" + id;
  return get(globalAppId);
}

std::string Environment::globalApp(const std::string &id) {
  std::string globalId = m_globalPrefix + "_" + m_appPrefix + "_" + id;
  return get(globalId);
}

std::string Environment::local(const std::string &id) {
  return get(id);
}

std::string Environment::localApp(const std::string &id) {
  std::string localAppId = m_appPrefix + "_" + id;
  return get(localAppId);
}
}  // namespace App
}  // namespace Xi
