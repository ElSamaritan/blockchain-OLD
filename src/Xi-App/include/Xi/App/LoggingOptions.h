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

#pragma once

#include <string>

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Logging/Level.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>

#include "Xi/App/IOptions.h"

namespace Xi {
namespace App {
struct LoggingOptions : public IOptions {
  /*!
   * \brief DefaultLogLevel Log level applied if no logger specific level is specified.
   */
  Logging::Level DefaultLogLevel = Logging::INFO;

  /*!
   * \brief ConsoleLogLevel Minimum log level for console logging, if not provided the default log level is used.
   */
  boost::optional<Logging::Level> ConsoleLogLevel = boost::none;

  /*!
   * \brief FileLogLevel Minimum log level for file logging, if not provided the default log level is used.
   */
  boost::optional<Logging::Level> FileLogLevel = boost::none;

  /*!
   * \brief LogFilePath Path to store the log file, should be application dependent.
   */
  std::string LogFilePath;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(DefaultLogLevel)
  KV_MEMBER(ConsoleLogLevel)
  KV_MEMBER(FileLogLevel)
  KV_MEMBER(LogFilePath)
  KV_END_SERIALIZATION

  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;
};
}  // namespace App
}  // namespace Xi
