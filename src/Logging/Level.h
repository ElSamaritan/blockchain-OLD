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

#include <Xi/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Algorithm/String.h>

#include <Serialization/ISerializer.h>

namespace Logging {
enum Level {
  None = 0,
  Fatal = 1,
  Error = 2,
  Warning = 3,
  Info = 4,
  Debugging = 5,
  Trace = 6,
};

class LevelTranslator {
 public:
  using internal_type = std::string;
  using external_type = Level;

  boost::optional<external_type> get_value(internal_type const& v);
  boost::optional<internal_type> put_value(external_type const& v);
};

[[nodiscard]] bool serialize(Level& level, CryptoNote::ISerializer& serializer);

std::string toString(Level level);
}  // namespace Logging
