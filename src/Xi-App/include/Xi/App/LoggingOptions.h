﻿/* ============================================================================================== *
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
#include <optional>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Logging/Level.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/OptionalSerialization.hpp>

#include "Xi/App/IOptions.h"

namespace Xi {
namespace App {
struct LoggingOptions : public IOptions {
  /*!
   * \brief DefaultLogLevel Log level applied if no logger specific level is specified.
   */
  Logging::Level DefaultLogLevel = Logging::Info;

  /*!
   * \brief ConsoleLogLevel Minimum log level for console logging, if not provided the default log level is used.
   */
  std::optional<Logging::Level> ConsoleLogLevel = std::nullopt;

  /*!
   * \brief FileLogLevel Minimum log level for file logging, if not provided the default log level is used.
   */
  std::optional<Logging::Level> FileLogLevel = std::nullopt;

  /*!
   * \brief LogFilePath Path to store the log file, should be application dependent.
   */
  std::string LogFilePath;

  /*!
   * \brief FileLogLevel Minimum log level for file logging, if not provided the default warning level is used.
   */
  std::optional<Logging::Level> DiscordLogLevel = std::nullopt;

  /*!
   * \brief DiscordWebhook Your discord webhook url to send notifications.
   */
  std::string DiscordWebhook;

  /*!
   * \brief DiscordAuthor Is the author to be emplaced into the discord embed (if wanted)
   */
  std::string DiscordAuthor;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(DefaultLogLevel)
  KV_MEMBER(ConsoleLogLevel)
  KV_MEMBER(FileLogLevel)
  KV_MEMBER(LogFilePath)
  KV_MEMBER(DiscordLogLevel)
  KV_MEMBER(DiscordWebhook)
  KV_MEMBER(DiscordAuthor)
  KV_END_SERIALIZATION

  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;
};
}  // namespace App
}  // namespace Xi
