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

#include <cinttypes>
#include <string>
#include <vector>
#include <future>

#include <System/Dispatcher.h>

#include <Xi/Global.h>

#include "Xi/Http/Request.h"
#include "Xi/Http/Response.h"
#include "Xi/Http/SSLConfiguration.h"

namespace Xi {
namespace Http {
class ClientSession;

class Client {
 public:
  /*!
   * \brief Client creates an new http client connected to the given host.
   * \param host The hostname this clients connects to, can be an DNS entry.
   * \param port The port to connect to if necessary, if 0 then the default port is used (HTTP: 80, HTTPS: 443)
   */
  explicit Client(const std::string& host, uint16_t port, SSLConfiguration config);
  XI_DELETE_COPY(Client);
  XI_DEFAULT_MOVE(Client);
  virtual ~Client();

  const std::string host() const;
  uint16_t port() const;

  /*!
   * \brief send sends the request to the server asynchroniously
   *
   * \throws std::runtime_error if the session pool is exhausted
   */
  std::future<Response> send(Request&& request);
  std::future<Response> send(const std::string& url, Method method, ContentType type, const std::string& body = "");

#define MAKE_METHOD_ALIAS(NAME, METHOD)                                                                       \
  inline std::future<Response> NAME(const std::string& url, ContentType type, const std::string& body = "") { \
    return send(url, Method::METHOD, type, body);                                                             \
  }                                                                                                           \
  inline Response NAME##Sync(const std::string& url, ContentType type, const std::string& body = "") {        \
    return send(url, Method::METHOD, type, body).get();                                                       \
  }

  MAKE_METHOD_ALIAS(get, Get)
  MAKE_METHOD_ALIAS(post, Post)
  MAKE_METHOD_ALIAS(del, Delete)
  MAKE_METHOD_ALIAS(put, Put)
  MAKE_METHOD_ALIAS(head, Head)

#undef MAKE_METHOD_ALIAS

 private:
  const std::string m_host;
  const uint16_t m_port;
  SSLConfiguration m_sslConfig;

  struct _Worker;
  std::shared_ptr<_Worker> m_worker;
};
}  // namespace Http
}  // namespace Xi
