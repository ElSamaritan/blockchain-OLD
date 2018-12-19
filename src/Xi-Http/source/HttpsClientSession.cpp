#include "HttpsClientSession.h"

Xi::Http::HttpsClientSession::HttpsClientSession(boost::asio::io_context &io, boost::asio::ssl::context &ctx,
                                                 std::shared_ptr<IClientSessionBuilder> builder)
    : ClientSession(io, builder), m_stream{io, ctx} {}

void Xi::Http::HttpsClientSession::doPrepareRun() {
  const auto host = m_request.find(boost::beast::http::field::host);
  if (host == m_request.end()) throw std::runtime_error{"request has no host"};
  if (!SSL_set_tlsext_host_name(m_stream.native_handle(), std::string{host->value()}.c_str())) {
    boost::beast::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
    checkErrorCode(ec);
  }
}

void Xi::Http::HttpsClientSession::doOnHostResolved(resolver_t::results_type results) {
  boost::asio::async_connect(m_stream.next_layer(), results.begin(), results.end(),
                             std::bind(&ClientSession::onConnected, shared_from_this(), std::placeholders::_1));
}

void Xi::Http::HttpsClientSession::doOnConnected() {
  auto _this = shared_from_this();
  m_stream.async_handshake(
      boost::asio::ssl::stream_base::client,
      std::bind(&HttpsClientSession::onHandshake,
                std::shared_ptr<HttpsClientSession>{_this, static_cast<HttpsClientSession *>(_this.get())},
                std::placeholders::_1));
}

void Xi::Http::HttpsClientSession::doOnRequestWritten() {
  boost::beast::http::async_read(
      m_stream, m_buffer, m_response,
      std::bind(&ClientSession::onResponseRecieved, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Xi::Http::HttpsClientSession::doOnResponseRecieved() {
  m_stream.async_shutdown(std::bind(&ClientSession::onShutdown, shared_from_this(), std::placeholders::_1));
}

void Xi::Http::HttpsClientSession::doOnShutdown() {}

void Xi::Http::HttpsClientSession::onHandshake(boost::beast::error_code ec) {
  try {
    checkErrorCode(ec);
    boost::beast::http::async_write(
        m_stream, m_request,
        std::bind(&ClientSession::onRequestWritten, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  } catch (...) {
    fail(std::current_exception());
  }
}
