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
#include <string>
#include <utility>

#include <Xi/Concurrent/IDispatcher.h>

#include "Xi/Http/RequestHandler.h"
#include "Xi/Http/SSLConfiguration.h"

namespace Xi {
namespace Http {
/*!
 * \brief The Server class provies a http/https capable server implmentation
 */
class Server : std::enable_shared_from_this<Server> {
 public:
  /*!
   * \brief setHandler sets the handle that should be called for resolving requests
   *
   * If you want to provide multiple handlers you may consider a \see Router
   */
  void setHandler(std::shared_ptr<RequestHandler> handler);

  /*!
   * \brief handler the reciever of request, creating responses
   */
  std::shared_ptr<RequestHandler> handler() const;

  /*!
   * \brief start starts the server listening on given address, port (non blocking)
   * \param address the ip this server runs on
   * \param port the port that should be used
   *
   * \exception std::runtime_error If the server is already running
   * \exception std::runtime_error If no handler was set
   */
  void start(const std::string& address, uint16_t port);

  /*!
   * \brief stop gracefully shutdowns the server
   */
  void stop();

  /*!
   * \brief host The host address this server was started with, \see start
   */
  const std::string& host() const;

  void setDispatcher(std::shared_ptr<Concurrent::IDispatcher> dispatcher);
  std::shared_ptr<Concurrent::IDispatcher> dispatcher() const;

  SSLConfiguration sslConfiguration() const;
  void setSSLConfiguration(SSLConfiguration config);

 private:
  std::shared_ptr<RequestHandler> m_handler;
  std::shared_ptr<Concurrent::IDispatcher> m_dispatcher;
  SSLConfiguration m_sslConfig;
  std::string m_host;

  struct _Listener;
  std::shared_ptr<_Listener> m_listener;
};
}  // namespace Http
}  // namespace Xi
