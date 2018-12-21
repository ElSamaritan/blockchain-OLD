#include "Xi/Http/Client.h"

#include <chrono>
#include <thread>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>
#include <utility>

#include <boost/asio.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <Xi/Utils/String.h>

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

  _Worker() {
    // ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    ctx.set_verify_mode(boost::asio::ssl::verify_none);
  }

  void run() { thread = std::thread{std::bind(&_Worker::operator(), shared_from_this())}; }
  void stop() { keepRunning = false; }

  void operator()() {
    while (keepRunning) {
      try {
        boost::system::error_code ec;
        io.run(ec);
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

Xi::Http::Client::Client(const std::string &host, uint16_t port, SSLClientConfiguration config)
    : m_host{host}, m_port{port}, m_sslConfig{config}, m_worker{new _Worker} {
  m_sslConfig.initializeContext(m_worker->ctx);
  m_worker->run();
}

Xi::Http::Client::~Client() { m_worker->stop(); }

const std::string Xi::Http::Client::host() const { return m_host; }

uint16_t Xi::Http::Client::port() const { return m_port; }

std::future<Xi::Http::Response> Xi::Http::Client::send(Xi::Http::Request &&request) {
  if (request.host().empty()) request.setHost(host());
  if (request.port() == 0) request.setPort(port());
  if (!m_sslConfig.Disabled) {
    request.setSSLRequired(true);
    return m_worker->makeHttpsSession()->run(std::move(request));
  } else
    return m_worker->makeHttpSession()->run(std::move(request));
}

std::future<Xi::Http::Response> Xi::Http::Client::send(const std::string &url, Xi::Http::Method method,
                                                       Xi::Http::ContentType type, const std::string &body) {
  Request request{url, method};
  request.headers().setContentType(type);
  request.setBody(body);
  return send(std::move(request));
}
