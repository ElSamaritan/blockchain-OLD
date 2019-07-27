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

#include "Xi/Log/Discord/Embed.hpp"

#include <sstream>

#include <Serialization/JsonOutputStreamSerializer.h>
#include <Serialization/SerializationTools.h>

namespace Xi {
namespace Log {
namespace Discord {

Xi::Log::Discord::Embed &Xi::Log::Discord::Embed::timestamp(boost::posix_time::ptime time) {
  return timestamp(boost::posix_time::to_iso_extended_string(time));
}

EmbedField &Embed::addField() {
  fields().emplace_back();
  return fields().back();
}

Result<std::string> Embed::toJson() const {
  XI_ERROR_TRY();
  return success(CryptoNote::storeToJson(*this));
  XI_ERROR_CATCH();
}

Result<std::string> Embed::toWebhookBody() const {
  XI_ERROR_TRY();
  auto embedBody = toJson().takeOrThrow();
  std::stringstream builder{};
  builder << "{\"embeds\": [" << embedBody << "]}";
  return success(std::string{builder.str()});
  XI_ERROR_CATCH();
}

}  // namespace Discord
}  // namespace Log
}  // namespace Xi
