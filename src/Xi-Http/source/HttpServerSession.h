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

#include <memory>
#include <utility>

#include "Stream.h"
#include "ServerSession.h"

namespace Xi {
namespace Http {
class HttpServerSession : public ServerSession {
 public:
  HttpServerSession(socket_t socket, buffer_t buffer, std::shared_ptr<RequestHandler> handler,
                    Concurrent::IDispatcher& dispatcher);
  XI_DEFAULT_MOVE(HttpServerSession);
  ~HttpServerSession() override = default;

 protected:
  void doReadRequest() override;
  void doOnRequestRead() override;
  void doWriteResponse(Response&& response) override;
  void doOnResponseWritten() override;
  void doClose() override;

 private:
  socket_t m_socket;
};
}  // namespace Http
}  // namespace Xi
