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

#include "Xi/Http/ContentType.h"

#include <stdexcept>

std::string Xi::to_string(Xi::Http::ContentType status) {
  switch (status) {
    case Http::ContentType::Html:
      return "text/html";
    case Http::ContentType::Plain:
      return "text/plain";
    case Http::ContentType::Xml:
      return "application/xml";
    case Http::ContentType::Json:
      return "application/json";
    case Http::ContentType::Text:
      return "application/text";
    case Http::ContentType::Binary:
      return "application/octet-stream";
    case Http::ContentType::MultipartFormData:
      return "multipart/form-data";
    default:
      throw std::runtime_error{"unknow content type could not be parsed into a string representation"};
  }
}

namespace Xi {
template <>
Xi::Http::ContentType lexical_cast<Xi::Http::ContentType>(const std::string &value) {
  if (value == to_string(Http::ContentType::Html))
    return Http::ContentType::Html;
  else if (value == to_string(Http::ContentType::Plain))
    return Http::ContentType::Plain;
  else if (value == to_string(Http::ContentType::Xml))
    return Http::ContentType::Xml;
  else if (value == to_string(Http::ContentType::Json))
    return Http::ContentType::Json;
  else if (value == to_string(Http::ContentType::Text))
    return Http::ContentType::Text;
  else if (value == to_string(Http::ContentType::Binary))
    return Http::ContentType::Binary;
  else if (value == to_string(Http::ContentType::MultipartFormData))
    return Http::ContentType::MultipartFormData;
  else
    throw std::runtime_error{"unable to find a corresponding content type for the provided string"};
}
}  // namespace Xi
