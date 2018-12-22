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

#include "Xi/Http/Router.h"

#include <stdexcept>

void Xi::Http::Router::addEndpoint(std::shared_ptr<Xi::Http::Endpoint> endpoint, uint32_t priority) {
  m_endpoints.insert(std::make_pair(priority, endpoint));
}

bool Xi::Http::Router::acceptsRequest(const Xi::Http::Request &request) {
  for (auto it = m_endpoints.rbegin(); it != m_endpoints.rend(); ++it) {
    if (it->second->acceptsRequest(request)) return true;
  }
  return false;
}

Xi::Http::Response Xi::Http::Router::doHandleRequest(const Xi::Http::Request &request) {
  for (auto it = m_endpoints.rbegin(); it != m_endpoints.rend(); ++it) {
    if (it->second->acceptsRequest(request)) return it->second->operator()(request);
  }
  return makeNotFound();
}
