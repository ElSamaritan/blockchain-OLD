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

#include <sstream>
#include <memory>

#include <Xi/ExternalIncludePush.h>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/predef.h>
#include <Xi/ExternalIncludePop.h>

#include "Xi/Http/Request.h"
#include "Xi/Http/Response.h"

#include "IClientSessionBuilder.h"
#include "BeastConversion.h"

namespace Xi {
namespace Http {
/*!
 * \brief The ClientSession class coordinates a single http request as a session
 *
 * This is an abstract class the actual handling of the raw communication is implmented by \see HttpClientSession and
 * \see HttpsClientSession
 */
class ClientSession : public std::enable_shared_from_this<ClientSession> {
 public:
  using future_t = std::future<Response>;
  using promise_t = std::promise<Response>;
  using resolver_t = boost::asio::ip::tcp::resolver;
  using request_t = boost::beast::http::request<boost::beast::http::string_body>;
  using response_t = boost::beast::http::response<boost::beast::http::string_body>;
  using buffer_t = boost::beast::flat_buffer;

  /*!
   * \brief ClientSession constructs a new request session
   * \param io the async coordinator that will be executed by workers
   * \param builder the builder used to create new session if a redirection occurs
   *
   * The builder is required because at some point a http connection will maybe forwarded to a https connection.
   */
  ClientSession(boost::asio::io_context& io, std::shared_ptr<IClientSessionBuilder> builder);
  XI_DELETE_COPY(ClientSession);
  XI_DEFAULT_MOVE(ClientSession);
  virtual ~ClientSession() = default;

  /*!
   * \brief run starts this session for the given request
   * \param request the request to send, nocopy
   * \return a future that will be ready once the session finishes or an error occured
   */
  future_t run(Request&& request);

  /*!
   * \brief onHostResolved called when the resolver finished
   */
  void onHostResolved(boost::beast::error_code ec, resolver_t::results_type results);

  /*!
   * \brief onConnected called when the initial connection was established
   */
  void onConnected(boost::beast::error_code ec);

  /*!
   * \brief onRequestWritten called when the raw request has been written to the server
   */
  void onRequestWritten(boost::beast::error_code ec, std::size_t bytesTransfered);

  /*!
   * \brief onResponseRecieved called once the response from the server has been recieved
   */
  void onResponseRecieved(boost::beast::error_code ec, std::size_t bytesTransfered);

  /*!
   * \brief onShutdown called when the session completed and we need to shutdown the connection
   */
  void onShutdown(boost::beast::error_code ec);

 protected:
  /*!
   * This methods implement the NoneVirtualInterface pattern, never call them in one of the base classes. The previous
   * wrapper are designed to be exception save and handle exceptions as expected, forwarding it to the promise.
   *
   * Therefore you are free to throw any exception within these methods.
   */

  virtual void doPrepareRun() = 0;
  virtual void doOnHostResolved(resolver_t::iterator begin, resolver_t::iterator end) = 0;
  virtual void doOnConnected() = 0;
  virtual void doOnRequestWritten() = 0;
  virtual void doOnResponseRecieved() = 0;
  virtual void doOnShutdown() = 0;

  /*!
   * \brief checkErrorCode checks if the error_code encodes an error and throws if so
   */
  void checkErrorCode(const boost::beast::error_code& ec);

  /*!
   * \brief fail indicates that the session has failed with an error and sets the promise to the exception
   */
  void fail(const std::exception_ptr& ex);

 protected:
  request_t m_request;
  response_t m_response;
  buffer_t m_buffer;

 private:
  /*!
   * \brief run starts the request internally after initial data setup
   */
  void run();

  /*!
   * \brief redirect performs a redirection of the request to a new host
   */
  void redirect(boost::optional<std::string> location);

  /*!
   * \brief swallowErrorCode checks for an error but does not throw it
   *
   * This is used if sth. happend when the result was already recieved but the socket could not be shutdowned
   * gracefully.
   */
  void swallowErrorCode(const boost::beast::error_code& ec);

 private:
  boost::asio::io_context& m_io;
  boost::asio::ip::tcp::resolver m_resolver;
  BeastConversion m_conversion;
  uint16_t m_redirectionCounter;

  promise_t m_promise;
  std::string m_port;
  bool m_sslRequired;

  std::shared_ptr<IClientSessionBuilder> m_builder;
};
}  // namespace Http
}  // namespace Xi
