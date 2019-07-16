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

#include <optional>
#include <vector>
#include <memory>

#include <Xi/Exceptions.hpp>
#include <Xi/Blockchain/Block/Version.hpp>

namespace Xi {
namespace Config {

template <typename _ConfigT>
class VersionContainer {
 public:
  using config_type = _ConfigT;
  using optional_config_type = const config_type*;

 private:
  std::vector<config_type> m_configs;

 public:
  optional_config_type operator()(Blockchain::Block::Version version) const {
    exceptional_if<InvalidArgumentError>(version.isNull());
    const auto index = static_cast<uint32_t>(version.native() - 1);
    if (index >= m_configs.size()) {
      if (m_configs.empty()) {
        return nullptr;
      } else {
        return std::addressof(this->m_configs.back());
      }
    } else {
      return this->m_configs.data() + index;
    }
  }

  void insert(Blockchain::Block::Version version, const config_type& config) {
    exceptional_if<InvalidArgumentError>(version.isNull());
    if (m_configs.empty()) {
      exceptional_if_not<InvalidIndexError>(version == Blockchain::Block::Version::Genesis);
      m_configs.emplace_back(config);
    } else {
      exceptional_if<InvalidIndexError>(version.native() == m_configs.size());
      m_configs.resize(version.native() - 1, m_configs.back());
      m_configs.emplace_back(config);
    }
  }
};

}  // namespace Config
}  // namespace Xi
