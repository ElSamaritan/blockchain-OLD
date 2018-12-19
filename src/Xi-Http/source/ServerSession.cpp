#include "ServerSession.h"

#include <stdexcept>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

Xi::Http::ServerSession::ServerSession(socket_t socket, std::shared_ptr<RequestHandler> handler)
    : m_socket{std::move(socket)}, m_strand{m_socket.get_executor()}, m_handler{handler} {
  if (!handler) throw std::invalid_argument{"a server session required a non null request handler"};
}

Xi::Http::ServerSession::~ServerSession() {}

void Xi::Http::ServerSession::run() { readRequest(); }

void Xi::Http::ServerSession::readRequest() {
  try {
    doReadRequest();
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::ServerSession::onRequestRead(boost::beast::error_code ec, std::size_t bytesTransfered) {
  boost::ignore_unused(bytesTransfered);
  try {
    checkErrorCode(ec);
    doOnRequestRead();
  } catch (...) {
    fail(std::current_exception());
  }
  if (ec == boost::beast::http::error::end_of_stream)
    return close();
  else
    checkErrorCode(std::move(ec));
}

void Xi::Http::ServerSession::writeResponse() {
  try {
    doWriteResponse();
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::ServerSession::onResponseWritten(boost::beast::error_code ec, std::size_t bytesTransfered) {
  try {
    boost::ignore_unused(bytesTransfered);
    checkErrorCode(ec);
    doOnResponseWritten();
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::ServerSession::close() {
  try {
    doClose();
  } catch (...) {
    fail(std::current_exception());
  }
}

void Xi::Http::ServerSession::checkErrorCode(boost::beast::error_code ec) {
  if (ec) {
    throw std::runtime_error{ec.message()};
  }
}

void Xi::Http::ServerSession::fail(std::exception_ptr ex) {
  XI_UNUSED(ex);  // Logging
}

void Xi::Http::ServerSession::doReadRequest() {
  m_request = {};
  boost::beast::http::async_read(
      m_socket, m_buffer, m_request,
      boost::asio::bind_executor(m_strand, std::bind(&ServerSession::onRequestRead, shared_from_this(),
                                                     std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::ServerSession::doOnRequestRead() {
  const auto request = m_conversion(m_request);
  Response response = m_handler->operator()(request);
  m_response = m_conversion(response);
  writeResponse();
}

void Xi::Http::ServerSession::doWriteResponse() {
  boost::beast::http::async_write(
      m_socket, m_response,
      boost::asio::bind_executor(m_strand, std::bind(&ServerSession::onResponseWritten, shared_from_this(),
                                                     std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::ServerSession::doOnResponseWritten() { close(); }

void Xi::Http::ServerSession::doClose() {
  boost::beast::error_code ec;
  m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
  checkErrorCode(ec);
}
