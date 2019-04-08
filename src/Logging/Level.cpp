/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include "Logging/Level.h"

#include <string>
#include <cassert>

#include <Xi/Algorithm/String.h>

#define LOG_LEVEL_STRING_CASE(LEVEL)                                                                         \
  if (string == std::to_string(static_cast<uint8_t>(Logging::LEVEL)) || string == put_value(Logging::LEVEL)) \
    return Logging::LEVEL;                                                                                   \
  else

boost::optional<Logging::LevelTranslator::external_type> Logging::LevelTranslator::get_value(
    const Logging::LevelTranslator::internal_type &v) {
  auto string = Xi::to_lower(v);
  LOG_LEVEL_STRING_CASE(NONE)
  LOG_LEVEL_STRING_CASE(FATAL)
  LOG_LEVEL_STRING_CASE(ERROR)
  LOG_LEVEL_STRING_CASE(WARNING)
  LOG_LEVEL_STRING_CASE(INFO)
  LOG_LEVEL_STRING_CASE(DEBUGGING)
  LOG_LEVEL_STRING_CASE(TRACE)
  return boost::none;
}

#undef LOG_LEVEL_STRING_CASE

boost::optional<Logging::LevelTranslator::internal_type> Logging::LevelTranslator::put_value(
    const Logging::LevelTranslator::external_type &v) {
  switch (v) {
    case Logging::NONE:
      return std::string{"none"};
    case Logging::FATAL:
      return std::string{"fatal"};
    case Logging::ERROR:
      return std::string{"error"};
    case Logging::WARNING:
      return std::string{"warning"};
    case Logging::INFO:
      return std::string{"info"};
    case Logging::DEBUGGING:
      return std::string{"debugging"};
    case Logging::TRACE:
      return std::string{"trace"};
    default:
      return boost::none;
  }
}

void Logging::serialize(Logging::Level &level, CryptoNote::ISerializer &serializer) {
  LevelTranslator translator{};
  if (serializer.type() == CryptoNote::ISerializer::INPUT) {
    std::string str;
    serializer(str, "log-level");
    level = translator.get_value(str).get_value_or(Level::NONE);
  } else {
    assert(serializer.type() == CryptoNote::ISerializer::OUTPUT);
    std::string str = translator.put_value(level).get_value_or("none");
    serializer(str, "log-level");
  }
}
