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

#include <Xi/Global.hh>

#include "Xi/Http/Method.h"
#include "Xi/Http/Version.h"
#include "Xi/Http/HeaderContainer.h"

namespace Xi {
namespace Http {
/*!
 * \brief The Request class is a wrapper for boost:beast requests with limited functionality to server xi.
 */
class Request final {
  XI_DELETE_COPY(Request);

 public:
  Request();
  Request(const std::string& target, Method method = Method::Get);
  XI_DEFAULT_MOVE(Request);
  ~Request() = default;

  /*!
   * \brief url is the url to query.
   */
  const std::string& target() const;

  /*!
   * \brief setUrl sets the url to query.
   */
  void setTarget(const std::string& target);

  /*!
   * \brief method is the specified method that will be used once emitting the request. Default: Get
   */
  Method method() const;

  /*!
   * \brief setMethod sets the method to be used accoring to the HTTP protocol. Default: Get
   */
  void setMethod(Method method);

  /*!
   * \brief version is the http protocol version that should be used. Default: v1.1
   */
  Version version() const;

  /*!
   * \brief setVersion sets the http protocol version to be used. Default v1.1
   */
  void setVersion(Version version);

  /*!
   * \brief body is the request body that will be send to the enpoint once the request is emitted.
   */
  const std::string& body() const;

  /*! * \brief setBody sets the request body that will be send to the ecpoint once the request is emitted.
   */
  void setBody(const std::string& body);

  /*!
   * \brief setBody sets the request body that will be send to the ecpoint once the request is emitted.
   */
  void setBody(std::string&& body);

  /*!
   * \brief isSSLRequired returns true if ssl is required by the client
   *
   * A request may get redirected during it evaluation and the redirected endpoint may not offer ssl encryption, thus
   * the request must abort a connection once its get redirected to such endpoint if this flag is set.
   */
  bool isSSLRequired() const;

  /*!
   * \brief setSSLRequired sets if ssl is required
   *
   * You may use this field to enforce particular ssl connections even if the client resolving this request does not
   * require it.
   */
  void setSSLRequired(bool isRequired);

  /*!
   * \brief headers A wrapper of all headers set for this request.
   */
  HeaderContainer& headers();

  /*!
   * \brief headers A wrapper of all headers set for this request.
   */
  const HeaderContainer& headers() const;

  /*!
   * \brief host the host to query
   *
   * You may provide a different host than the client has setup. If you dont do so the client will provide the host
   * for you.
   */
  const std::string& host() const;

  /*!
   * \brief setHost sets the host to connect to, empty if the default client host should be used.
   */
  void setHost(const std::string& host);

  /*!
   * \brief port the port to connect to
   *
   * You may provide a different port than the client has setup. If you dont do so the client will provide the port
   * for you.
   */
  uint16_t port() const;

  /*!
   * \brief setPort sets the port to connect to on the host
   * \param port the port to connect to, 0 if no port provided
   */
  void setPort(uint16_t port);

 private:
  friend class Server;

  HeaderContainer m_headers;
  std::string m_body;
  Version m_version;
  Method m_method;
  std::string m_target;
  std::string m_host;
  uint16_t m_port;
  bool m_sslRequired;
};
}  // namespace Http
}  // namespace Xi
