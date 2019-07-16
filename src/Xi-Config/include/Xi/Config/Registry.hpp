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

#include <map>
#include <string>
#include <string_view>

#include <Xi/Concurrent/RecursiveLock.h>

#include "Xi/Config/Configuration.hpp"

namespace Xi {
namespace Config {

class Registry {
 public:
  static int addConfigJson(std::string_view name, std::string_view content);
  static bool addConfigJsonFile(std::string_view name, std::string_view path);
  static const Configuration* searchByName(std::string_view name);

 private:
  static std::map<std::string, Configuration> m_configs;
  static Concurrent::RecursiveLock m_lock;
};

}  // namespace Config
}  // namespace Xi
