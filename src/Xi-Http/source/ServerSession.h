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

#include <memory>
#include <utility>

#include <Xi/ExternalIncludePush.h>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.h>
#include <Xi/Concurrent/IDispatcher.h>

#include "Xi/Http/RequestHandler.h"

#include "BeastConversion.h"

namespace Xi {
namespace Http {
/*!
 * \brief The ServerSession class servers an accepted http connection
 *
 * This class will be created from the listener once a connection was established and will manage the http transfer and
 * high level api calls in order to handle the request.
 */
class ServerSession : public std::enable_shared_from_this<ServerSession> {
  XI_DELETE_COPY(ServerSession);

 public:
  using buffer_t = boost::beast::flat_buffer;
  using request_t = BeastConversion::beast_request_t;
  using response_t = BeastConversion::beast_response_t;
  using socket_t = boost::asio::ip::tcp::socket;
  using strand_t = boost::asio::strand<boost::asio::io_context::executor_type>;

 public:
  /*!
   * \brief ServerSession creates a session handling a client request
   * \param socket the socket created for communicating with the client
   * \param handler the high level api handler that will server the request
   */
  ServerSession(socket_t socket, buffer_t buffer, std::shared_ptr<RequestHandler> handler,
                Concurrent::IDispatcher& dispatcher);
  XI_DEFAULT_MOVE(ServerSession);
  virtual ~ServerSession();

  /*!
   * \brief run starts handling the session
   */
  virtual void run();

  /*!
   * \brief readRequest called on startup an will read the requests data/headers...
   */
  void readRequest();

  /*!
   * \brief onRequestRead called when the request is ready and fully read
   * \param ec An error that may occured while reading
   * \param bytesTransfered the amount of bytes read
   */
  void onRequestRead(boost::beast::error_code ec, std::size_t bytesTransfered);

  /*!
   * \brief writeResponse called once the high level api handler returned a response that will be send back
   */
  void writeResponse(Response&& response);

  /*!
   * \brief onResponseWritten called once the response was written to the client
   * \param ec An error that may occured while writing
   * \param bytesTransfered the amount of bytes writte
   */
  void onResponseWritten(boost::beast::error_code ec, std::size_t bytesTransfered);

  /*!
   * \brief close closes the connection
   */
  void close();

  /*!
   * \brief checkErrorCode check for an potential error and throw if an error occured
   */
  void checkErrorCode(boost::beast::error_code ec);

  /*!
   * \brief fail marks this session as failed
   */
  void fail(std::exception_ptr ex);

  /*!
   * The follwing methods implement the NoneVirtualInterface patter. Never call them directly they are wrapped
   * by their corresponding public methods that will prevent any exception.
   *
   * Thus you are free to throw any exception you like within these methods.
   */

  virtual void doReadRequest() = 0;
  virtual void doOnRequestRead() = 0;
  virtual void doWriteResponse(Response&& response) = 0;
  virtual void doOnResponseWritten() = 0;
  virtual void doClose() = 0;

  socket_t m_socket;
  strand_t m_strand;
  buffer_t m_buffer;
  std::shared_ptr<RequestHandler> m_handler;
  Concurrent::IDispatcher& m_dispatcher;
  request_t m_request;
  Request m_convertedRequest;
  response_t m_response;
  BeastConversion m_conversion;
};
}  // namespace Http
}  // namespace Xi
