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

#include "AllConfigs.hpp"

std::map<std::string, Xi::Config::Configuration> Xi::Config::Registry::m_configs{};
Xi::Concurrent::RecursiveLock Xi::Config::Registry::m_lock{};

int Xi::Config::Registry::addConfigJson(std::string_view name, std::string_view content) {
  XI_CONCURRENT_RLOCK(m_lock);
  XiEmbeddedConfigs::init();

  if (searchByName(name) != nullptr) {
    std::cerr << "Duplicate configuration for: " << name << std::endl;
    exceptional<RuntimeError>();
  }
  Configuration config{};
  std::string scontent{content};
  if (!CryptoNote::loadFromJson(config, scontent)) {
    std::cerr << "Loading configuration failed for: " << name << std::endl;
    exceptional<RuntimeError>();
  }
  m_configs[std::string{name}] = std::move(config);
  return 0;
}

bool Xi::Config::Registry::addConfigJsonFile(std::string_view name, std::string_view path) {
  XI_CONCURRENT_RLOCK(m_lock);
  XiEmbeddedConfigs::init();

  std::string spath{path};
  std::ifstream stream{spath, std::ios::in};
  XI_RETURN_EC_IF_NOT(stream.good(), false);
  std::string scontent((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
  try {
    addConfigJson(name, scontent);
    XI_RETURN_SC(true);
  } catch (...) {
    XI_RETURN_EC(false);
  }
}

const Xi::Config::Configuration *Xi::Config::Registry::searchByName(std::string_view name) {
  XI_CONCURRENT_RLOCK(m_lock);
  XiEmbeddedConfigs::init();

  std::string sname{name};
  if (auto search = m_configs.find(sname); search != m_configs.end()) {
    return std::addressof(search->second);
  } else {
    return nullptr;
  }
}
