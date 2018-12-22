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

#pragma once

#include <memory>
#include <utility>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/logic/tribool.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

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
  ServerSessionDetector(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context& ctx,
                        std::shared_ptr<IServerSessionBuilder> builder);
  ~ServerSessionDetector() = default;

  void run();

  void onDetect(boost::beast::error_code ec, boost::tribool result);

 private:
  boost::asio::ip::tcp::socket m_socket;
  boost::asio::ssl::context& m_ctx;
  boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
  boost::beast::flat_buffer m_buffer;
  std::shared_ptr<IServerSessionBuilder> m_builder;
};
}  // namespace Http
}  // namespace Xi
