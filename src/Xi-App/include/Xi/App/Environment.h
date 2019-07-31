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
#include <cinttypes>
#include <vector>
#include <map>

#include <Xi/Global.hh>

namespace Xi {
namespace App {

class Environment {
 public:
  static std::string get(const std::string& key);
  static void set(const std::string& value);

 public:
  explicit Environment(const std::string prefix);

 public:
  Environment& operator()(bool& value, const std::string& id);

  Environment& operator()(int8_t& value, const std::string& id);
  Environment& operator()(uint8_t& value, const std::string& id);
  Environment& operator()(int16_t& value, const std::string& id);
  Environment& operator()(uint16_t& value, const std::string& id);
  Environment& operator()(int32_t& value, const std::string& id);
  Environment& operator()(uint32_t& value, const std::string& id);
  Environment& operator()(int64_t& value, const std::string& id);
  Environment& operator()(uint64_t& value, const std::string& id);

  Environment& operator()(std::string& value, const std::string& id);
  Environment& operator()(std::vector<std::string>& value, const std::string& id);

  void load();
  void load(const std::string& filename, bool userProvided = false);

 private:
  std::string global(const std::string& id);
  std::string local(const std::string& id);

 private:
  std::string m_globalPrefix;
  std::map<std::string, std::string> m_importMap;
};

}  // namespace App
}  // namespace Xi
