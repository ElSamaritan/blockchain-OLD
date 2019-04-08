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

#include <Xi/Algorithm/String.h>

namespace Xi {
namespace Http {
/*!
 * \brief The Content-Type entity header is used to indicate the media type of the resource.
 *
 * In responses, a Content-Type header tells the client what the content type of the returned content actually is.
 * Browsers will do MIME sniffing in some cases and will not necessarily follow the value of this header; to prevent
 * this behavior, the header X-Content-Type-Options can be set to nosniff.
 *
 * In requests, (such as POST or PUT), the client tells the server what type of data is actually sent.
 *
 * Documentation is taken from https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Type
 */
enum struct ContentType { Html, Plain, Xml, Json, Text, Binary, MultipartFormData };
}  // namespace Http
std::string to_string(Http::ContentType status);

template <>
Xi::Http::ContentType lexical_cast<Xi::Http::ContentType>(const std::string& value);
}  // namespace Xi
