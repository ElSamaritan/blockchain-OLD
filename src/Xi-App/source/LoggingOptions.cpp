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

#include "Xi/App/LoggingOptions.h"

#include <Xi/Global.hh>

void Xi::App::LoggingOptions::emplaceOptions(cxxopts::Options &options) {
  Logging::LevelTranslator trans{};
  const std::string acceptedLevels{"none|fatal|error|warning|info|debbuging|trace"};
  // clang-format off
  options.add_options("logging")
    ("log-level", "minimum log level for all loggers applied by default",
        cxxopts::value<std::string>()->default_value(*trans.put_value(DefaultLogLevel)), acceptedLevels)
    ("log-console-level", "minimum log level for console logging", cxxopts::value<std::string>(), acceptedLevels)
    ("log-file-level", "minimum log level for file logging", cxxopts::value<std::string>(), acceptedLevels)
    ("log-file", "file path writing logs to", cxxopts::value<std::string>(LogFilePath)->default_value(LogFilePath), "filepath")
  ;
  // clang-format on
}

bool Xi::App::LoggingOptions::evaluateParsedOptions(const cxxopts::Options &options,
                                                    const cxxopts::ParseResult &result) {
  XI_UNUSED(options);
  Logging::LevelTranslator trans{};
  if (result.count("log-level")) {
    auto logLevel = trans.get_value(result["log-level"].as<std::string>());
    if (logLevel == boost::none) {
      throw cxxopts::invalid_option_format_error{result["log-level"].as<std::string>() + " no recognized."};
    } else {
      DefaultLogLevel = *logLevel;
    }
  }
  if (result.count("log-console-level")) {
    auto logLevel = trans.get_value(result["log-console-level"].as<std::string>());
    if (logLevel == boost::none) {
      throw cxxopts::invalid_option_format_error{result["log-console-level"].as<std::string>() + " no recognized."};
    } else {
      ConsoleLogLevel = *logLevel;
    }
  }
  if (result.count("log-file-level")) {
    auto logLevel = trans.get_value(result["log-file-level"].as<std::string>());
    if (logLevel == boost::none) {
      throw cxxopts::invalid_option_format_error{result["log-file-level"].as<std::string>() + " no recognized."};
    } else {
      FileLogLevel = *logLevel;
    }
  }
  return false;
}
