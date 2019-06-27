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

#include "ServerSessionDetector.h"

#include <stdexcept>

#include "HttpServerSession.h"
#include "HttpsServerSession.h"

Xi::Http::ServerSessionDetector::ServerSessionDetector(boost::asio::ip::tcp::socket socket,
                                                       boost::asio::ssl::context &ctx,
                                                       std::shared_ptr<IServerSessionBuilder> builder)
    : m_socket{std::move(socket)}, m_ctx{ctx}, m_strand{m_socket.get_executor()}, m_builder{builder} {
  if (m_builder.get() == nullptr) throw std::invalid_argument{"builder cannot be a nullptr"};
}

void Xi::Http::ServerSessionDetector::run() {
  async_detect_ssl(m_socket, m_buffer,
                   boost::asio::bind_executor(m_strand, std::bind(&ServerSessionDetector::onDetect, shared_from_this(),
                                                                  std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::ServerSessionDetector::onDetect(boost::beast::error_code ec, boost::logic::tribool result) {
  if (ec) {
    // TODO Logging
  }

  if (result) {
    return m_builder->makeHttpsServerSession(std::move(m_socket), std::move(m_buffer))->run();
  } else {
    return m_builder->makeHttpServerSession(std::move(m_socket), std::move(m_buffer))->run();
  }
}
