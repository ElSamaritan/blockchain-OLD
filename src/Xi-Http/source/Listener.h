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

#include <Xi/Global.hh>

#include <Xi/ExternalIncludePush.h>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <Xi/ExternalIncludePop.h>

#include "ServerSession.h"

namespace Xi {
namespace Http {
/*!
 * \brief The Listener class is an abstract class to implement custom logic for tcp connection listener
 */
class Listener : public std::enable_shared_from_this<Listener> {
 public:
  /*!
   * \brief Listener creates a new bounded listener
   * \param endpoint the endpoint this listener is attached to
   */
  Listener(boost::asio::ip::tcp::endpoint endpoint);
  virtual ~Listener() = default;

  /*!
   * \brief run starts emitting async accept requests for incoming connections
   */
  virtual void run();

  /*!
   * \brief doAccept queries an incoming connection and forwards it to onAccept
   */
  void doAccept();

  /*!
   * \brief onAccept checks for errors and forward to the connection to doOnAccept
   */
  void onAccept(boost::beast::error_code ec);

 protected:
  /*!
   * \brief onError called whenever an internal error occured
   * \param ec the error code that occured
   * \param what a description of the step performed when the error occured
   */
  virtual void onError(boost::beast::error_code ec, const std::string& what);

  /*!
   * \brief onError called whenever any exception was thrown but were caught to prevent server shutdown
   * \param ex the exception thrown
   */
  virtual void onError(std::exception_ptr ex);

  /*!
   * \brief doOnAccept handles a newly established connection
   *
   * The implemenation should create a server session out of it, or check if its banned
   */
  virtual void doOnAccept(boost::asio::ip::tcp::socket socket) = 0;

  boost::asio::io_context io;

 private:
  boost::asio::ip::tcp::acceptor m_acceptor;
  boost::asio::ip::tcp::socket m_socket;
};
}  // namespace Http
}  // namespace Xi
