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

#include <future>
#include <memory>
#include <utility>
#include <sstream>
#include <string>

#include <boost/optional.hpp>

#include <Xi/Global.hh>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>

#include "ClientSession.h"

namespace Xi {
namespace Http {
/*!
 * \brief The HttpClientSession class implements a client session without ssl encryption.
 */
class HttpClientSession : public ClientSession {
 public:
  HttpClientSession(boost::asio::io_context& io, std::shared_ptr<IClientSessionBuilder> builder);
  XI_DELETE_COPY(HttpClientSession);
  XI_DEFAULT_MOVE(HttpClientSession);
  ~HttpClientSession() override = default;

 protected:
  void doPrepareRun() override;
  void doOnHostResolved(resolver_t::iterator begin, resolver_t::iterator end) override;
  void doOnConnected() override;
  void doOnRequestWritten() override;
  void doOnResponseRecieved() override;
  void doOnShutdown() override;

 private:
  boost::asio::ip::tcp::socket m_socket;
};
}  // namespace Http
}  // namespace Xi
