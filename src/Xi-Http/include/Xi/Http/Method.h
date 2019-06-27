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

#include <Xi/Algorithm/String.h>

namespace Xi {
namespace Http {
/*!
 * HTTP defines a set of request methods to indicate the desired action to be performed for a given resource. Although
 * they can also be nouns, these request methods are sometimes referred as HTTP verbs. Each of them implements a
 * different semantic, but some common features are shared by a group of them: e.g. a request method can be safe,
 * idempotent, or cacheable.
 *
 * Description is taken from \link https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods .
 */
enum struct Method {
  Unknown = 0,  ///< The method is unknown or was not specified.
  Delete = 1,   ///< The DELETE method deletes the specified resource.
  Get = 2,      ///< The GET method requests a representation of the specified resource. Requests using GET should only
                ///< retrieve data.
  Head = 3,  ///< The HEAD method asks for a response identical to that of a GET request, but without the response body.
  Post = 4,  ///< The POST method is used to submit an entity to the specified resource, often causing a change in state
             ///< or side effects on the server.
  Put = 5,   ///< The PUT method replaces all current representations of the target resource with the request payload.
  Connect = 6,  ///< The CONNECT method establishes a tunnel to the server identified by the target resource.
  Options = 7,  ///< The OPTIONS method is used to describe the communication options for the target resource.
  Trace = 8,    ///< The TRACE method performs a message loop-back test along the path to the target resource.
  Patch = 29,   ///< The PATCH method is used to apply partial modifications to a resource.
};
}  // namespace Http
std::string to_string(Http::Method method);
}  // namespace Xi
