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

#include "Xi/Http/ContentEncoding.h"

#include <stdexcept>

std::string Xi::to_string(Xi::Http::ContentEncoding encoding) {
  switch (encoding) {
    case Http::ContentEncoding::Gzip:
      return "gzip";
    case Http::ContentEncoding::Compress:
      return "compress";
    case Http::ContentEncoding::Deflate:
      return "deflate";
    case Http::ContentEncoding::Identity:
      return "identity";
    case Http::ContentEncoding::Brotli:
      return "br";
    default:
      throw std::runtime_error{"unknown content encoding passed to to_string"};
  }
}

namespace Xi {
template <>
Http::ContentEncoding lexical_cast<Http::ContentEncoding>(const std::string& str) {
  if (str == to_string(Http::ContentEncoding::Gzip))
    return Http::ContentEncoding::Gzip;
  else if (str == to_string(Http::ContentEncoding::Compress))
    return Http::ContentEncoding::Compress;
  else if (str == to_string(Http::ContentEncoding::Deflate))
    return Http::ContentEncoding::Deflate;
  else if (str == to_string(Http::ContentEncoding::Identity))
    return Http::ContentEncoding::Identity;
  else if (str == to_string(Http::ContentEncoding::Brotli))
    return Http::ContentEncoding::Brotli;
  else
    throw std::runtime_error{"unexpected string representation of content encoding"};
}
}  // namespace Xi
