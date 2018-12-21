#include "ServerSession.h"

#include <stdexcept>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

Xi::Http::ServerSession::ServerSession(socket_t socket, buffer_t buffer, std::shared_ptr<RequestHandler> handler,
                                       Concurrent::IDispatcher& dispatcher)
    : m_socket{std::move(socket)},
      m_strand{m_socket.get_executor()},
      m_buffer{std::move(buffer)},
      m_handler{handler},
      m_dispatcher{dispatcher} {
  if (m_handler.get() == nullptr) throw std::invalid_argument{"a server session requires a non null request handler"};
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

void Xi::Http::ServerSession::writeResponse(Response&& response) {
  try {
    doWriteResponse(std::move(response));
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
