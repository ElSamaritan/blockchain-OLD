﻿/* ============================================================================================== *
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

#include <Xi/ExternalIncludePush.h>
#include <boost/logic/tribool.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <Xi/ExternalIncludePop.h>

#include "Stream.h"
#include "IServerSessionBuilder.h"

namespace Xi {
namespace Http {
/*!
 * \brief The ServerSessionDetector class detects whether a client tries to use ssl encryption or not
 *
 * This initial session step enables the server to serve http and https on the same port.
 */
class ServerSessionDetector : public std::enable_shared_from_this<ServerSessionDetector> {
 public:
  ServerSessionDetector(ServerStream::socket_type socket, boost::asio::ssl::context& ctx,
                        std::shared_ptr<IServerSessionBuilder> builder);
  ~ServerSessionDetector() = default;

  void run();

  void onDetect(boost::beast::error_code ec, boost::tribool result);

 private:
  ServerStream::socket_type m_socket;
  boost::asio::ssl::context& m_ctx;
  boost::asio::strand<ServerStream::executor_type> m_strand;
  boost::beast::flat_buffer m_buffer;
  std::shared_ptr<IServerSessionBuilder> m_builder;
};
}  // namespace Http
}  // namespace Xi
