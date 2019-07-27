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
#include <memory>

#include <Xi/Global.hh>
#include <Xi/Http/Client.h>
#include <Logging/CommonLogger.h>

namespace Xi {
namespace Log {
namespace Discord {

class DiscordLogger final : public Logging::CommonLogger {
 public:
  DiscordLogger(const std::string& webhook, Logging::Level level = Logging::Level::None);
  ~DiscordLogger() override;

  void setAuthor(const std::string& author);

 protected:
  void doLogContext(LogContext context) override;

 private:
  std::string eraseColorEndcodings(const std::string& message) const;
  uint32_t logLevelColor(Logging::Level level) const;

 private:
  std::unique_ptr<Http::Client> m_http;
  std::string m_webhook;
  std::string m_author{};
};

}  // namespace Discord
}  // namespace Log
}  // namespace Xi
