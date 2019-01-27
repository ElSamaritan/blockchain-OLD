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

#include <map>
#include <string>

#include <Xi/Global.h>

#include "Xi/Http/RequestHandler.h"

namespace Xi {
namespace Http {
/*!
 * \brief The Endpoint class conditionally handles requests
 *
 * An endpoint is a request handler that only acts to applicable request, thus a router can determine which endpoint
 * to choose based on this interface.
 */
class Endpoint : public RequestHandler {
 public:
  Endpoint() = default;
  XI_DEFAULT_COPY(Endpoint);
  XI_DEFAULT_MOVE(Endpoint);
  virtual ~Endpoint() override = default;

  /*!
   * \brief acceptsRequest check if the request can be handled by this endpoint
   * \param request the request to check
   * \return true if the endpoint is willing to handle this request otherwise false
   */
  virtual bool acceptsRequest(const Request& request) = 0;
};
}  // namespace Http
}  // namespace Xi
