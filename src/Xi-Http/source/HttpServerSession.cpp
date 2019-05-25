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

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#include "HttpServerSession.h"

#include <Xi/ExternalIncludePush.h>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <Xi/ExternalIncludePop.h>

void Xi::Http::HttpServerSession::doReadRequest() {
  m_request = {};
  boost::beast::http::async_read(
      m_socket, m_buffer, m_request,
      boost::asio::bind_executor(m_strand, std::bind(&ServerSession::onRequestRead, shared_from_this(),
                                                     std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::HttpServerSession::doOnRequestRead() {
  m_convertedRequest = m_conversion(m_request);
  auto _this = shared_from_this();
  m_dispatcher.post([_this]() { _this->writeResponse(_this->m_handler->operator()(_this->m_convertedRequest)); });
}

void Xi::Http::HttpServerSession::doWriteResponse(Response&& response) {
  m_response = m_conversion(response);
  boost::beast::http::async_write(
      m_socket, m_response,
      boost::asio::bind_executor(m_strand, std::bind(&ServerSession::onResponseWritten, shared_from_this(),
                                                     std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::HttpServerSession::doOnResponseWritten() { close(); }

void Xi::Http::HttpServerSession::doClose() {
  boost::beast::error_code ec;
  m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
  checkErrorCode(ec);
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
