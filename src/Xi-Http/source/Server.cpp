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

#include "Xi/Http/Server.h"

#include <vector>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <atomic>

#include <Xi/ExternalIncludePush.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <Xi/ExternalIncludePop.h>

#include "Listener.h"
#include "ServerSession.h"
#include "ServerSessionDetector.h"
#include "HttpServerSession.h"
#include "HttpsServerSession.h"

namespace {

struct LoopGuard : public boost::asio::io_context::work {
  using work::work;
};

}  // namespace

struct Xi::Http::Server::_Listener : Listener, IServerSessionBuilder {
  std::shared_ptr<LoopGuard> loopGuard;  ///< Ensures the io context keeps idling.

  std::vector<std::thread> runner;
  boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23};
  std::shared_ptr<RequestHandler> handler;
  Xi::Concurrent::IDispatcher& dispatcher;

  _Listener(const std::string& address, uint16_t port, std::shared_ptr<RequestHandler> _handler,
            Concurrent::IDispatcher& _dispatcher)
      : Listener(boost::asio::ip::tcp::endpoint{boost::asio::ip::make_address(address), port}),
        handler{_handler},
        dispatcher{_dispatcher} {
  }

  void run(uint16_t numThreads) {
    loopGuard = std::make_shared<LoopGuard>(io);
    runner.reserve(numThreads);
    for (std::size_t i = 0; i < numThreads; ++i) {
      runner.emplace_back(std::thread{[&] {
        try {
          boost::system::error_code ec;
          io.run(ec);
          // TODO Logging
        } catch (...) {
          // TODO Logging
        }
      }});
    }
    Listener::run();
  }

  void stop() override {
    try {
      loopGuard.reset();
      Listener::stop();
      io.stop();
      for (auto& thread : runner) {
        thread.join();
      }
    } catch (...) {
      /* */
    }
  }

  void doOnAccept(ServerStream::socket_type socket) override {
    if (!loopGuard) {
      return;
    } else {
      try {
        std::make_shared<ServerSessionDetector>(std::move(socket), ctx,
                                                std::shared_ptr<IServerSessionBuilder>{shared_from_this(), this})
            ->run();
      } catch (...) {
        /* */
      }
    }
  }

  std::shared_ptr<ServerSession> makeHttpServerSession(ServerSession::socket_t socket,
                                                       ServerSession::buffer_t buffer) override {
    return std::make_shared<HttpServerSession>(std::move(socket), std::move(buffer), handler, dispatcher);
  }
  std::shared_ptr<ServerSession> makeHttpsServerSession(ServerSession::socket_t socket,
                                                        ServerSession::buffer_t buffer) override {
    return std::make_shared<HttpsServerSession>(std::move(socket), std::move(buffer), ctx, handler, dispatcher);
  }
};

void Xi::Http::Server::setHandler(std::shared_ptr<Xi::Http::RequestHandler> handler) {
  m_handler = handler;
}

std::shared_ptr<Xi::Http::RequestHandler> Xi::Http::Server::handler() const {
  return m_handler;
}

void Xi::Http::Server::start(const std::string& address, uint16_t port) {
  if (m_listener.get() != nullptr)
    throw std::runtime_error{"server is already running, stop it first"};
  if (m_handler.get() == nullptr)
    throw std::runtime_error{"you must provide a handler in order to start the server"};
  m_listener = std::make_shared<_Listener>(address, port, handler(), *dispatcher());

  m_sslConfig.initializeServerContext(m_listener->ctx);
  m_host = address;

  m_listener->run(1);
}

void Xi::Http::Server::stop() {
  if (m_listener) {
    m_listener->stop();
    m_listener.reset();
  }
}

const std::string& Xi::Http::Server::host() const {
  return m_host;
}

void Xi::Http::Server::setDispatcher(std::shared_ptr<Xi::Concurrent::IDispatcher> dispatcher) {
  m_dispatcher = dispatcher;
}

std::shared_ptr<Xi::Concurrent::IDispatcher> Xi::Http::Server::dispatcher() const {
  return m_dispatcher;
}

Xi::Http::SSLConfiguration Xi::Http::Server::sslConfiguration() const {
  return m_sslConfig;
}

void Xi::Http::Server::setSSLConfiguration(SSLConfiguration config) {
  m_sslConfig = config;
}
