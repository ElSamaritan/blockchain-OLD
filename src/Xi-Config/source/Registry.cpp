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

#include "Xi/Config/Registry.hpp"

#include <iostream>
#include <fstream>
#include <utility>

#include <Xi/Exceptions.hpp>
#include <Serialization/SerializationTools.h>

std::map<std::string, Xi::Config::Configuration> Xi::Config::Registry::m_configs{};
Xi::Concurrent::RecursiveLock Xi::Config::Registry::m_lock{};

bool Xi::Config::Registry::addConfigJson(std::string_view name, std::string_view content) {
  XI_CONCURRENT_RLOCK(m_lock);

  XI_RETURN_EC_IF(searchByName(name) != nullptr, false);
  Configuration config{};
  std::string scontent{content};
  XI_RETURN_EC_IF_NOT(CryptoNote::loadFromJson(config, scontent), false);
  m_configs[std::string{name}] = std::move(config);
  XI_RETURN_SC(true);
}

bool Xi::Config::Registry::addConfigJsonFile(std::string_view name, std::string_view path) {
  XI_CONCURRENT_RLOCK(m_lock);

  std::string spath{path};
  std::ifstream stream{spath, std::ios::in};
  XI_RETURN_EC_IF_NOT(stream.good(), false);
  std::string scontent((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
  XI_RETURN_EC_IF_NOT(addConfigJson(name, scontent), false);
  XI_RETURN_SC(true);
}

const Xi::Config::Configuration *Xi::Config::Registry::searchByName(std::string_view name) {
  XI_CONCURRENT_RLOCK(m_lock);

  std::string sname{name};
  if (auto search = m_configs.find(sname); search != m_configs.end()) {
    return std::addressof(search->second);
  } else {
    return nullptr;
  }
}
