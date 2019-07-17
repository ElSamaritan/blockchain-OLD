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

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#include "HttpClientSession.h"

Xi::Http::HttpClientSession::HttpClientSession(boost::asio::io_context& io,
                                               std::shared_ptr<IClientSessionBuilder> builder)
    : ClientSession(io, builder), m_socket{io} {
}

void Xi::Http::HttpClientSession::doPrepareRun() {
}

void Xi::Http::HttpClientSession::doOnHostResolved(resolver_t::iterator begin, resolver_t::iterator end) {
  boost::asio::async_connect(m_socket, begin, end,
                             std::bind(&ClientSession::onConnected, shared_from_this(), std::placeholders::_1));
}

void Xi::Http::HttpClientSession::doOnConnected() {
  boost::beast::http::async_write(
      m_socket, m_request,
      std::bind(&ClientSession::onRequestWritten, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Xi::Http::HttpClientSession::doOnRequestWritten() {
  boost::beast::http::async_read(
      m_socket, m_buffer, m_response,
      std::bind(&ClientSession::onResponseRecieved, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Xi::Http::HttpClientSession::doOnResponseRecieved() {
  boost::beast::error_code ec;
  m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  onShutdown(ec);
}

void Xi::Http::HttpClientSession::doOnShutdown() {
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
