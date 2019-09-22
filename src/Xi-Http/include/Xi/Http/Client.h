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

#include <cinttypes>
#include <string>
#include <vector>
#include <utility>
#include <future>
#include <optional>
#include <variant>
#include <chrono>
#include <optional>

#include <System/Dispatcher.h>

#include <Xi/Global.hh>
#include <Xi/Concurrent/ReadersWriterLock.h>

#include <Xi/Network/Port.hpp>
#include <Xi/Network/Uri.hpp>
#include <Xi/Network/Protocol.hpp>

#include "Xi/Http/Request.h"
#include "Xi/Http/Response.h"
#include "Xi/Http/SSLConfiguration.h"
#include "Xi/Http/BasicCredentials.h"
#include "Xi/Http/BearerCredentials.h"
#include "Xi/Http/SSLConfiguration.h"

namespace Xi {
namespace Http {
class ClientSession;

/*!
 * \brief The Client class provides basic facilities for building and sending http based web requests.
 *
 * A client may have a default endpoint to connect to which is used to resolve a relative path requested. It is also
 * possible to not provide any host at all. In this case it is up to the caller to ensure all informations required to
 * resolve the endpoint are given on the request.
 */
class Client {
 public:
  /*!
   * \brief Client creates a new http client without a given host.
   * \param config Ssl configuration to use for https validation.
   */

  explicit Client(SSLConfiguration config);
  /*!
   * \brief Client creates an new http client connected to the given host.
   * \param host The hostname this clients connects to, can be an DNS entry.
   * \param config Ssl configuration to use for https validation.
   */
  explicit Client(const std::string& host, SSLConfiguration config);
  XI_DELETE_COPY(Client);
  XI_DEFAULT_MOVE(Client);
  virtual ~Client();

  const std::string host() const;
  uint16_t port() const;
  Network::Protocol protocol() const;

  void useAuthorization(Null);
  void useAuthorization(const BasicCredentials& credentials);
  void useAuthorization(const BearerCredentials& credentials);

  Client& useTimeout(std::chrono::seconds seconds);

  /*!
   * \brief send sends the request to the server asynchroniously
   *
   * \throws std::runtime_error if the session pool is exhausted
   */
  std::future<Response> send(Request&& request);
  std::future<Response> send(const std::string& url, Method method, ContentType type, std::string body = "");

#define MAKE_METHOD_ALIAS(NAME, METHOD)                                                                \
  inline std::future<Response> NAME(const std::string& url, ContentType type, std::string body = "") { \
    return send(url, Method::METHOD, type, move(body));                                                \
  }                                                                                                    \
  inline Response NAME##Sync(const std::string& url, ContentType type, std::string body = "") {        \
    return send(url, Method::METHOD, type, move(body)).get();                                          \
  }

  MAKE_METHOD_ALIAS(get, Get)
  MAKE_METHOD_ALIAS(post, Post)
  MAKE_METHOD_ALIAS(del, Delete)
  MAKE_METHOD_ALIAS(put, Put)
  MAKE_METHOD_ALIAS(head, Head)

#undef MAKE_METHOD_ALIAS

 private:
  std::optional<Network::Uri> m_host;
  SSLConfiguration m_sslConfig;

  using credentials_type = std::variant<Null, BasicCredentials, BearerCredentials>;
  credentials_type m_credentials{null};

  Concurrent::ReadersWriterLock m_credentials_guard{};

  struct _Worker;
  std::shared_ptr<_Worker> m_worker;
};

}  // namespace Http
}  // namespace Xi
