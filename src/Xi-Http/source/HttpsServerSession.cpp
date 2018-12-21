#include "HttpsServerSession.h"

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/asio/bind_executor.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

Xi::Http::HttpsServerSession::HttpsServerSession(Xi::Http::ServerSession::socket_t socket,
                                                 Xi::Http::ServerSession::buffer_t buffer,
                                                 boost::asio::ssl::context &ctx,
                                                 std::shared_ptr<Xi::Http::RequestHandler> handler,
                                                 Xi::Concurrent::IDispatcher &dispatcher)
    : ServerSession(std::move(socket), std::move(buffer), handler, dispatcher), m_stream{m_socket, ctx} {}

void Xi::Http::HttpsServerSession::run() {
  m_stream.async_handshake(
      boost::asio::ssl::stream_base::server, m_buffer.data(),
      boost::asio::bind_executor(m_strand, std::bind(&HttpsServerSession::onHandshake,
                                                     std::shared_ptr<HttpsServerSession>{shared_from_this(), this},
                                                     std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::HttpsServerSession::onHandshake(boost::beast::error_code ec, std::size_t bytes_used) {
  try {
    checkErrorCode(ec);
    m_buffer.consume(bytes_used);
    readRequest();
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::HttpsServerSession::onShutdown(boost::beast::error_code ec) {
  try {
    checkErrorCode(ec);
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::HttpsServerSession::doReadRequest() {
  m_request = {};
  boost::beast::http::async_read(
      m_stream, m_buffer, m_request,
      boost::asio::bind_executor(m_strand, std::bind(&ServerSession::onRequestRead, shared_from_this(),
                                                     std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::HttpsServerSession::doOnRequestRead() {
  m_convertedRequest = m_conversion(m_request);
  auto _this = shared_from_this();
  m_dispatcher.post([_this]() { _this->writeResponse(_this->m_handler->operator()(_this->m_convertedRequest)); });
}

void Xi::Http::HttpsServerSession::doWriteResponse(Response &&response) {
  m_response = m_conversion(response);
  boost::beast::http::async_write(
      m_stream, m_response,
      boost::asio::bind_executor(m_strand, std::bind(&ServerSession::onResponseWritten, shared_from_this(),
                                                     std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::HttpsServerSession::doOnResponseWritten() { close(); }

void Xi::Http::HttpsServerSession::doClose() {
  m_stream.async_shutdown(boost::asio::bind_executor(
      m_strand, std::bind(&HttpsServerSession::onShutdown,
                          std::shared_ptr<HttpsServerSession>{shared_from_this(), this}, std::placeholders::_1)));
}
