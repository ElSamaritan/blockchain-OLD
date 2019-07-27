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
#include <optional>
#include <cinttypes>

#include <Xi/ExternalIncludePush.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/OptionalSerialization.hpp>

namespace Xi {
namespace Log {
namespace Discord {

class EmbedField {
 public:
  XI_PROPERTY(std::string, name, "")
  XI_PROPERTY(std::string, value, "")
  XI_PROPERTY(bool, isInline, false)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(name(), name)
  KV_MEMBER_RENAME(value(), value)
  KV_MEMBER_RENAME(isInline(), inline)
  KV_END_SERIALIZATION
};

class EmbedUrl {
 public:
  XI_PROPERTY(std::string, url, "")

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(url(), url)
  KV_END_SERIALIZATION
};

class EmbedAuthor {
 public:
  XI_PROPERTY(std::string, name, "")
  XI_PROPERTY(std::optional<std::string>, url, std::nullopt)
  XI_PROPERTY(std::optional<std::string>, icon, std::nullopt)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(name(), name)
  KV_MEMBER_RENAME(url(), url)
  KV_MEMBER_RENAME(icon(), icon_url)
  KV_END_SERIALIZATION
};

class Embed {
 public:
  XI_PROPERTY(std::string, title, "")
  XI_PROPERTY(std::string, description, "")
  XI_PROPERTY(std::optional<std::string>, url, "")
  XI_PROPERTY(uint32_t, color, 0)
  XI_PROPERTY(std::optional<std::string>, timestamp, std::nullopt)
  XI_PROPERTY(std::optional<EmbedUrl>, thumbnail, std::nullopt)
  XI_PROPERTY(std::optional<EmbedUrl>, image, std::nullopt)
  XI_PROPERTY(std::optional<EmbedAuthor>, author, std::nullopt)
  XI_PROPERTY(std::vector<EmbedField>, fields, std::vector<EmbedField>{})

  Embed& timestamp(boost::posix_time::ptime time);
  EmbedField& addField();

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(title(), title)
  KV_MEMBER_RENAME(description(), description)
  KV_MEMBER_RENAME(url(), url)
  KV_MEMBER_RENAME(color(), color)
  KV_MEMBER_RENAME(timestamp(), timestamp)
  KV_MEMBER_RENAME(thumbnail(), thumbnail)
  KV_MEMBER_RENAME(image(), image)
  KV_MEMBER_RENAME(author(), author)
  KV_MEMBER_RENAME(fields(), fields)
  KV_END_SERIALIZATION

  Result<std::string> toJson() const;
  Result<std::string> toWebhookBody() const;
};

}  // namespace Discord
}  // namespace Log
}  // namespace Xi
