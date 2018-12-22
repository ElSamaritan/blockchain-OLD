/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include <Xi/Global.h>

#include "Xi/Http/Endpoint.h"
#include "Xi/Http/Method.h"

namespace Xi {
namespace Http {
/*!
 * \brief The BasicEndpoint class conditionally accepts requests based on their path/method and forwards it to a handler
 */
class BasicEndpoint : public Endpoint {
 public:
  /*!
   * \brief BasicEndpoint construct a basic endpoint
   * \param target required target this endpoint represents
   * \param method the method this enpoint can handle
   * \param handler the handler to forward the request to, can be nullptr
   */
  BasicEndpoint(const std::string& target, Method method, std::shared_ptr<RequestHandler> handler);
  XI_DEFAULT_COPY(BasicEndpoint);
  XI_DEFAULT_MOVE(BasicEndpoint);
  virtual ~BasicEndpoint() override = default;

  /*!
   * \brief acceptsRequest checks if the request is applicable for this endpoint.
   * \param request the request to check
   * \return true if the target and method of this enpoint and the request are equal otherwise false
   */
  bool acceptsRequest(const Request& request) override;

 protected:
  /*!
   * \brief doHandleRequest handles the given request by forwarding it to the provided handler
   * \return notImplemented response if the request is not accepted or no handler is provided, otherwise the result of
   * the handler provided
   */
  Response doHandleRequest(const Request& request) override;

 private:
  std::string m_expectedTarget;
  Method m_expectedMethod;
  std::shared_ptr<RequestHandler> m_handler;
};
}  // namespace Http
}  // namespace Xi
