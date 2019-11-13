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

#include "Xi/App/LoggingOptions.h"

#include <Xi/Global.hh>
#include <Xi/FileSystem.h>

void Xi::App::LoggingOptions::loadEnvironment(Xi::App::Environment &env) {
  std::string defaultLogLevel{};
  std::string consoleLogLevel{};
  std::string discordLogLevel{};
  std::string fileLogLevel{};
  // clang-format off
  env
    (defaultLogLevel, "LOG_LEVEL")
    (consoleLogLevel, "LOG_CONSOLE_LEVEL")
    (discordLogLevel, "LOG_DISCORD_LEVEL")
    (fileLogLevel, "LOG_FILE_LEVEL")
    (LogFilePath, "LOG_FILE_PATH")
    (DiscordWebhook, "LOG_DISCORD_WEBHOOK")
    (DiscordAuthor, "LOG_DISCORD_AUTHOR")
  ;
  // clang-format on
  Logging::LevelTranslator trans{};
  if (!defaultLogLevel.empty()) {
    DefaultLogLevel = trans.get_value(defaultLogLevel).value_or(DefaultLogLevel);
  }
  if (!consoleLogLevel.empty()) {
    if (auto level = trans.get_value(consoleLogLevel)) {
      ConsoleLogLevel = *level;
    }
  }
  if (!discordLogLevel.empty()) {
    if (auto level = trans.get_value(discordLogLevel)) {
      DiscordLogLevel = *level;
    }
  }
  if (!fileLogLevel.empty()) {
    if (auto level = trans.get_value(fileLogLevel)) {
      FileLogLevel = *level;
    }
  }
}

void Xi::App::LoggingOptions::emplaceOptions(cxxopts::Options &options) {
  Logging::LevelTranslator trans{};
  const std::string acceptedLevels{"none|fatal|error|warning|info|debbuging|trace"};
  // clang-format off
  options.add_options("logging")
    ("log-level", "minimum log level for all loggers applied by default",
        cxxopts::value<std::string>()->default_value(*trans.put_value(DefaultLogLevel)), acceptedLevels)
    ("log-console-level", "minimum log level for console logging", cxxopts::value<std::string>(), acceptedLevels)
    ("log-file-level", "minimum log level for file logging", cxxopts::value<std::string>(), acceptedLevels)
    ("log-file", "file path writing logs to", cxxopts::value<std::string>(LogFilePath)->default_value(LogFilePath), "<filepath>")
    ("log-discord-level", "minimum log level for discord logging", cxxopts::value<std::string>(), acceptedLevels)
    ("log-discord-webhook", "discord webhook url", cxxopts::value<std::string>(DiscordWebhook)->default_value(DiscordWebhook), "<url>")
    ("log-discord-author", "author name for the discord posts", cxxopts::value<std::string>(DiscordAuthor)->default_value(DiscordAuthor), "<name>")
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
      throw cxxopts::invalid_option_format_error{result["log-level"].as<std::string>() + " not recognized."};
    } else {
      DefaultLogLevel = *logLevel;
    }
  }
  if (result.count("log-console-level")) {
    auto logLevel = trans.get_value(result["log-console-level"].as<std::string>());
    if (logLevel == boost::none) {
      throw cxxopts::invalid_option_format_error{result["log-console-level"].as<std::string>() + " not recognized."};
    } else {
      ConsoleLogLevel = *logLevel;
    }
  }
  if (result.count("log-file-level")) {
    auto logLevel = trans.get_value(result["log-file-level"].as<std::string>());
    if (logLevel == boost::none) {
      throw cxxopts::invalid_option_format_error{result["log-file-level"].as<std::string>() + " not recognized."};
    } else {
      FileLogLevel = *logLevel;
    }
  }
  if (result.count("log-discord-level")) {
    auto logLevel = trans.get_value(result["log-discord-level"].as<std::string>());
    if (logLevel == boost::none) {
      throw cxxopts::invalid_option_format_error{result["log-discord-level"].as<std::string>() + " not recognized."};
    } else {
      DiscordLogLevel = *logLevel;
    }
  }
  if (!LogFilePath.empty()) {
    auto directory = FileSystem::fileDirectory(LogFilePath);
    if (!directory.isError() && !directory->empty()) {
      FileSystem::ensureDirectoryExists(*directory).throwOnError();
    }
  }
  return false;
}
