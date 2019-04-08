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

#include "Xi/Http/BasicEndpoint.h"

#include <stdexcept>

#include <Xi/Algorithm/String.h>

Xi::Http::BasicEndpoint::BasicEndpoint(const std::string &target, Xi::Http::Method method,
                                       std::shared_ptr<Xi::Http::RequestHandler> handler)
    : m_expectedTarget{target}, m_expectedMethod{method}, m_handler{handler} {}

bool Xi::Http::BasicEndpoint::acceptsRequest(const Xi::Http::Request &request) {
  return request.method() == m_expectedMethod && request.target() == m_expectedTarget;
}

Xi::Http::Response Xi::Http::BasicEndpoint::doHandleRequest(const Xi::Http::Request &request) {
  if (!acceptsRequest(request) || m_handler.get() == nullptr) return makeNotImplemented();
  return m_handler->operator()(request);
}
