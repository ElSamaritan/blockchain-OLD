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

#include "Listener.h"

Xi::Http::Listener::Listener(boost::asio::ip::tcp::endpoint endpoint) : io{}, m_acceptor(io), m_socket(io) {
  boost::beast::error_code ec;

  // Open the acceptor
  m_acceptor.open(endpoint.protocol(), ec);
  if (ec) {
    onError(ec, "open");
    return;
  }

  // Allow address reuse
  m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    onError(ec, "set_option");
    return;
  }

  // Bind to the server address
  m_acceptor.bind(endpoint, ec);
  if (ec) {
    onError(ec, "bind");
    return;
  }

  // Start listening for connections
  m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    onError(ec, "listen");
    return;
  }
}

void Xi::Http::Listener::run() {
  if (!m_acceptor.is_open()) return;
  doAccept();
}

void Xi::Http::Listener::doAccept() {
  m_acceptor.async_accept(m_socket, std::bind(&Listener::onAccept, shared_from_this(), std::placeholders::_1));
}

void Xi::Http::Listener::onAccept(boost::beast::error_code ec) {
  try {
    if (ec) {
      onError(ec, "accept");
    } else {
      doOnAccept(std::move(m_socket));
    }
  } catch (...) {
    onError(std::current_exception());
  }

  // Accept another connection
  doAccept();
}

void Xi::Http::Listener::onError(boost::beast::error_code ec, const std::string &what) { XI_UNUSED(ec, what); }

void Xi::Http::Listener::onError(std::exception_ptr ex) { XI_UNUSED(ex); }
