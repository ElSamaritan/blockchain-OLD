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

#include <future>
#include <memory>
#include <utility>
#include <sstream>
#include <string>

#include <boost/optional.hpp>

#include <Xi/Global.h>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

#include "ClientSession.h"

namespace Xi {
namespace Http {
/*!
 * \brief The HttpsClientSession class implements a client session requiring ssl encryption.
 */
class HttpsClientSession : public ClientSession {
 public:
  HttpsClientSession(boost::asio::io_context& io, boost::asio::ssl::context& ctx,
                     std::shared_ptr<IClientSessionBuilder> builder);
  XI_DELETE_COPY(HttpsClientSession);
  XI_DEFAULT_MOVE(HttpsClientSession);
  ~HttpsClientSession() override = default;

 protected:
  void doPrepareRun() override;
  void doOnHostResolved(resolver_t::iterator begin, resolver_t::iterator end) override;
  void doOnConnected() override;
  void doOnRequestWritten() override;
  void doOnResponseRecieved() override;
  void doOnShutdown() override;

 private:
  /*!
   * \brief onHandshake called after initial handshake with the server
   */
  void onHandshake(boost::beast::error_code ec);

 private:
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_stream;
};
}  // namespace Http
}  // namespace Xi
