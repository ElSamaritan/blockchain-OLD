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

#include "Xi/Http/Client.h"

#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <Xi/Algorithm/String.h>

#include "Xi/Http/Uri.h"

#include "IClientSessionBuilder.h"
#include "ClientSession.h"
#include "HttpClientSession.h"
#include "HttpsClientSession.h"

struct Xi::Http::Client::_Worker : IClientSessionBuilder, std::enable_shared_from_this<_Worker> {
  std::thread thread;
  std::atomic_bool keepRunning{true};
  boost::asio::io_context io;
  boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23_client};

  _Worker() {}

  void run() { thread = std::thread{std::bind(&_Worker::operator(), shared_from_this())}; }
  void stop() { keepRunning = false; }

  void operator()() {
    while (keepRunning) {
      try {
        boost::system::error_code ec;
        auto tasksHandled = io.run(ec);
        io.reset();
        if (tasksHandled == 0) std::this_thread::sleep_for(std::chrono::milliseconds{20});
      } catch (...) {
        // TODO Logging
      }
    }
  }

  std::shared_ptr<ClientSession> makeHttpSession() override {
    return std::make_shared<HttpClientSession>(io, shared_from_this());
  }
  std::shared_ptr<ClientSession> makeHttpsSession() override {
    return std::make_shared<HttpsClientSession>(io, ctx, shared_from_this());
  }
};

Xi::Http::Client::Client(const std::string &host, uint16_t port, SSLConfiguration config)
    : m_host{host}, m_port{port}, m_sslConfig{config}, m_worker{new _Worker} {
  m_sslConfig.initializeClientContext(m_worker->ctx);
  m_worker->run();
}

Xi::Http::Client::~Client() { m_worker->stop(); }

const std::string Xi::Http::Client::host() const { return m_host; }

uint16_t Xi::Http::Client::port() const { return m_port; }

std::future<Xi::Http::Response> Xi::Http::Client::send(Xi::Http::Request &&request) {
  if (request.host().empty()) request.setHost(host());
  if (request.port() == 0) request.setPort(port());
  request.headers().setAcceptedContentEncodings(
      {ContentEncoding::Gzip, ContentEncoding::Deflate, ContentEncoding::Identity});
  // TODO: request type should be determined on schema not config.
  if (m_sslConfig.enabled()) {
    request.setSSLRequired(true);
    return m_worker->makeHttpsSession()->run(std::move(request));
  } else
    return m_worker->makeHttpSession()->run(std::move(request));
}

std::future<Xi::Http::Response> Xi::Http::Client::send(const std::string &url, Xi::Http::Method method,
                                                       Xi::Http::ContentType type, std::string body) {
  Request request{url, method};
  request.headers().setContentType(type);
  request.setBody(move(body));
  return send(std::move(request));
}
