#include "HttpClientSession.h"

Xi::Http::HttpClientSession::HttpClientSession(boost::asio::io_context& io,
                                               std::shared_ptr<IClientSessionBuilder> builder)
    : ClientSession(io, builder), m_socket{io} {}

void Xi::Http::HttpClientSession::doPrepareRun() {}

void Xi::Http::HttpClientSession::doOnHostResolved(resolver_t::results_type results) {
  boost::asio::async_connect(m_socket, results.begin(), results.end(),
                             std::bind(&ClientSession::onConnected, shared_from_this(), std::placeholders::_1));
}

void Xi::Http::HttpClientSession::doOnConnected() {
  boost::beast::http::async_write(
      m_socket, m_request,
      std::bind(&ClientSession::onRequestWritten, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Xi::Http::HttpClientSession::doOnRequestWritten() {
  boost::beast::http::async_read(
      m_socket, m_buffer, m_response,
      std::bind(&ClientSession::onResponseRecieved, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Xi::Http::HttpClientSession::doOnResponseRecieved() {
  boost::beast::error_code ec;
  m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  onShutdown(ec);
}

void Xi::Http::HttpClientSession::doOnShutdown() {}
