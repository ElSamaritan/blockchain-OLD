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

#include <memory>
#include <utility>

#include <Xi/ExternalIncludePush.h>
#include <boost/asio/ssl.hpp>
#include <Xi/ExternalIncludePop.h>

#include "HttpServerSession.h"

namespace Xi {
namespace Http {
class HttpsServerSession : public ServerSession {
 public:
  HttpsServerSession(socket_t socket, buffer_t buffer, boost::asio::ssl::context& ctx,
                     std::shared_ptr<RequestHandler> handler, Concurrent::IDispatcher& dispatcher);
  XI_DEFAULT_MOVE(HttpsServerSession);
  ~HttpsServerSession() override = default;

  void run() override;

  void onHandshake(boost::beast::error_code ec, std::size_t bytes_used);
  void onShutdown(boost::beast::error_code ec);

 protected:
  void doReadRequest() override;
  void doOnRequestRead() override;
  void doWriteResponse(Response&& response) override;
  void doOnResponseWritten() override;
  void doClose() override;

 private:
  boost::asio::ssl::stream<socket_t&> m_stream;
};
}  // namespace Http
}  // namespace Xi
