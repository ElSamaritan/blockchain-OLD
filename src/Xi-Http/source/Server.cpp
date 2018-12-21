#include "Xi/Http/Server.h"

#include <vector>
#include <thread>
#include <stdexcept>
#include <atomic>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/asio.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include "Listener.h"
#include "ServerSession.h"

struct Xi::Http::Server::_Listener : Listener {
  std::vector<std::thread> runner;
  std::shared_ptr<RequestHandler> handler;
  Xi::Concurrent::IDispatcher& dispatcher;
  std::atomic_bool keepRunning{true};

  _Listener(const std::string& address, uint16_t port, std::shared_ptr<RequestHandler> _handler,
            Concurrent::IDispatcher& _dispatcher)
      : Listener(boost::asio::ip::tcp::endpoint{boost::asio::ip::make_address(address), port}),
        handler{_handler},
        dispatcher{_dispatcher} {}

  void run(uint16_t numThreads) {
    Listener::run();
    runner.reserve(numThreads);
    for (std::size_t i = 0; i < numThreads; ++i) {
      runner.emplace_back(std::thread{[&] {
        while (keepRunning) {
          try {
            boost::system::error_code ec;
            io.run(ec);
          } catch (...) {
            // TODO Logging
          }
        }
      }});
    }
  }

  ~_Listener() override {
    try {
      keepRunning = false;
      for (auto& thread : runner) thread.join();
      io.stop();
    } catch (...) {
    }
  }

  void doOnAccept(boost::asio::ip::tcp::socket socket) override {
    std::make_shared<ServerSession>(std::move(socket), handler, dispatcher)->run();
  }
};

void Xi::Http::Server::setHandler(std::shared_ptr<Xi::Http::RequestHandler> handler) { m_handler = handler; }

std::shared_ptr<Xi::Http::RequestHandler> Xi::Http::Server::handler() const { return m_handler; }

void Xi::Http::Server::start(const std::string& address, uint16_t port) {
  if (m_listener.get() != nullptr) throw std::runtime_error{"server is already running, stop it first"};
  if (m_handler.get() == nullptr) throw std::runtime_error{"you must provide a handler in order to start the server"};
  m_listener = std::make_shared<_Listener>(address, port, handler(), *dispatcher());
  m_listener->run(1);
}

void Xi::Http::Server::stop() { m_listener.reset(); }

void Xi::Http::Server::setDispatcher(std::shared_ptr<Xi::Concurrent::IDispatcher> dispatcher) {
  m_dispatcher = dispatcher;
}

std::shared_ptr<Xi::Concurrent::IDispatcher> Xi::Http::Server::dispatcher() const { return m_dispatcher; }
