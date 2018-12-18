#include "Xi/Http/Client.h"

#include <chrono>
#include <thread>
#include <thread>
#include <atomic>
#include <functional>

#include <boost/asio.hpp>
#include <boost/asio/ssl/stream.hpp>

#include "IClientSessionBuilder.h"
#include "ClientSession.h"
#include "HttpClientSession.h"
#include "HttpsClientSession.h"

struct Xi::Http::Client::_Worker : IClientSessionBuilder {
  std::thread thread;
  std::atomic_bool keepRunning{true};
  std::unique_ptr<boost::asio::io_context> io;
  boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23_client};

  _Worker() : thread{std::bind(&_Worker::operator(), this)} {
    // ctx.set_verify_mode(boost::asio::ssl::verify_peer);
    ctx.set_verify_mode(boost::asio::ssl::verify_none);
  }

  ~_Worker() override {
    keepRunning = false;
    thread.join();
  }

  void operator()() {
    io = std::make_unique<boost::asio::io_context>();
    while (keepRunning) {
      try {
        boost::system::error_code ec;
        io->run(ec);
      } catch (...) {
        // TODO Logging
      }
    }
    io.reset();
  }

  std::shared_ptr<ClientSession> makeHttpSession() override {
    if (!io) throw std::runtime_error{"worker is not initialized"};
    return std::make_shared<HttpClientSession>(*io, *this);
  }
  std::shared_ptr<ClientSession> makeHttpsSession() override {
    if (!io) throw std::runtime_error{"worker is not initialized"};
    return std::make_shared<HttpsClientSession>(*io, ctx, *this);
  }
};

Xi::Http::Client::Client(const std::string &host, uint16_t port) : m_host{host}, m_port{port}, m_worker{new _Worker} {}

Xi::Http::Client::~Client() {}

const std::string Xi::Http::Client::host() const { return m_host; }

uint16_t Xi::Http::Client::port() const { return m_port; }

uint16_t Xi::Http::Client::httpPort() const {
  if (port() == 0)
    return 80;
  else
    return port();
}

uint16_t Xi::Http::Client::httpsPort() const {
  if (port() == 0)
    return 443;
  else
    return port();
}

uint16_t Xi::Http::Client::maximumSessions() const { return 32; }

std::future<Xi::Http::Response> Xi::Http::Client::send(Xi::Http::Request &&request) {
  std::string portStr = to_string(static_cast<uint64_t>(m_port));
  return m_worker->makeHttpsSession()->run(m_host.c_str(), portStr.c_str(), std::move(request));
}

std::future<Xi::Http::Response> Xi::Http::Client::send(const std::string &url, Xi::Http::Method method,
                                                       Xi::Http::ContentType type, const std::string &body) {
  Request request{url, method};
  request.headers().setContentType(type);
  request.setBody(body);
  return send(std::move(request));
}
