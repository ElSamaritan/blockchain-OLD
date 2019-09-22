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

#include "Xi/Log/Discord/DiscordLogger.hpp"

#include <sstream>
#include <cassert>
#include <iostream>

#include <Serialization/ConsoleOutputSerializer.hpp>

#include "Xi/Log/Discord/Embed.hpp"

namespace Xi {
namespace Log {
namespace Discord {

DiscordLogger::DiscordLogger(const std::string& webhook, Logging::Level level)
    : CommonLogger(level), m_http{nullptr}, m_webhook{"/api/webhooks/"} {
  const std::string webhookPrefix{"https://discordapp.com/api/webhooks/"};
  if (starts_with(webhook, webhookPrefix)) {
    m_webhook += webhook.substr(webhookPrefix.size());
  } else {
    m_webhook += webhook;
  }

  m_http = std::make_unique<Http::Client>("https://discordapp.com", Http::SSLConfiguration::RootStoreClient);
}

DiscordLogger::~DiscordLogger() {
}

void DiscordLogger::setAuthor(const std::string& author) {
  XI_CONCURRENT_LOCK_WRITE(m_configGuard);
  m_author = author;
}

void DiscordLogger::doLogString(Logging::CommonLogger::LogContext context, const std::string& message) {
  if (message.empty()) {
    return;
  }

  Embed embed{};

  std::string threadId{};
  {
    std::stringstream builder{};
    builder << context.thread;
    threadId = builder.str();
  }

  // clang-format off
  embed
    .title(context.category)
    .description(eraseColorEndcodings(message))
    .timestamp(context.time)
    .color(logLevelColor(context.level))
  ;

  embed.addField()
    .isInline(true)
    .name("thread")
    .value(threadId)
  ;
  embed.addField()
    .isInline(true)
    .name("level")
    .value(toString(context.level))
  ;
  // clang-format on

  if (!m_author.empty()) {
    EmbedAuthor author{};
    author.name(m_author);
    embed.author(author);
  }

  if (auto reqBody = embed.toWebhookBody(); !reqBody.isError()) {
    [[maybe_unused]] const auto response = m_http->postSync(m_webhook, Http::ContentType::Json, reqBody.take());
    assert(isSuccessCode(response.status()));
  }
}

void DiscordLogger::doLogObject(Logging::CommonLogger::LogContext context, Logging::ILogObject& object) {
  Embed embed{};

  std::string threadId{};
  {
    std::stringstream builder{};
    builder << context.thread;
    threadId = builder.str();
  }

  std::string objectString{};
  {
    std::stringstream builder{};
    builder << "```yaml\n";
    {
      CryptoNote::ConsoleOutputSerializer ser{builder};
      object.log(ser);
    }
    builder << "```";
    objectString = builder.str();
  }

  // clang-format off
  embed
    .title(context.category)
    .description(objectString)
    .timestamp(context.time)
    .color(logLevelColor(context.level))
  ;

  embed.addField()
    .isInline(true)
    .name("thread")
    .value(threadId)
  ;
  embed.addField()
    .isInline(true)
    .name("level")
    .value(toString(context.level))
  ;
  // clang-format on

  if (!m_author.empty()) {
    EmbedAuthor author{};
    author.name(m_author);
    embed.author(author);
  }

  if (auto reqBody = embed.toWebhookBody(); !reqBody.isError()) {
    [[maybe_unused]] const auto response = m_http->postSync(m_webhook, Http::ContentType::Json, reqBody.take());
    assert(isSuccessCode(response.status()));
  }
}

std::string DiscordLogger::eraseColorEndcodings(const std::string& message) const {
  std::stringstream builder;
  bool readingText = true;
  for (size_t charPos = 0; charPos < message.size(); ++charPos) {
    if (message[charPos] == ILogger::COLOR_DELIMETER) {
      readingText = !readingText;
    } else if (readingText) {
      builder << message[charPos];
    }
  }

  return builder.str();
}

uint32_t DiscordLogger::logLevelColor(Logging::Level level) const {
  switch (level) {
    case Logging::Fatal:
      return 0xFF0000;
    case Logging::Error:
      return 0xFF8000;
    case Logging::Warning:
      return 0xFFFF00;
    case Logging::Info:
      return 0xFFFFFF;
    case Logging::Debugging:
      return 0x00FF80;
    case Logging::Trace:
      return 0x0084FF;
    default:
      return 0x666666;
  }
}

}  // namespace Discord
}  // namespace Log
}  // namespace Xi
