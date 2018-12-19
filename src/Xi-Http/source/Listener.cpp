#include "Listener.h"

Xi::Http::Listener::Listener(boost::asio::ip::tcp::endpoint endpoint) : io{}, m_acceptor(io), m_socket(io) {
  boost::beast::error_code ec;

  // Open the acceptor
  m_acceptor.open(endpoint.protocol(), ec);
  if (ec) {
    onError(ec, "open");
    return;
  }

  // Allow address reuse
  m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    onError(ec, "set_option");
    return;
  }

  // Bind to the server address
  m_acceptor.bind(endpoint, ec);
  if (ec) {
    onError(ec, "bind");
    return;
  }

  // Start listening for connections
  m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    onError(ec, "listen");
    return;
  }
}

void Xi::Http::Listener::run() {
  if (!m_acceptor.is_open()) return;
  doAccept();
}

void Xi::Http::Listener::doAccept() {
  m_acceptor.async_accept(m_socket, std::bind(&Listener::onAccept, shared_from_this(), std::placeholders::_1));
}

void Xi::Http::Listener::onAccept(boost::beast::error_code ec) {
  if (ec) {
    onError(ec, "accept");
  } else {
    doOnAccept(std::move(m_socket));
  }

  // Accept another connection
  doAccept();
}

void Xi::Http::Listener::onError(boost::beast::error_code ec, const std::string &what) { XI_UNUSED(ec, what); }
