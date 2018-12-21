#include "HttpServerSession.h"

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

void Xi::Http::HttpServerSession::doReadRequest() {
  m_request = {};
  boost::beast::http::async_read(
      m_socket, m_buffer, m_request,
      boost::asio::bind_executor(m_strand, std::bind(&ServerSession::onRequestRead, shared_from_this(),
                                                     std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::HttpServerSession::doOnRequestRead() {
  m_convertedRequest = m_conversion(m_request);
  auto _this = shared_from_this();
  m_dispatcher.post([_this]() { _this->writeResponse(_this->m_handler->operator()(_this->m_convertedRequest)); });
}

void Xi::Http::HttpServerSession::doWriteResponse(Response&& response) {
  m_response = m_conversion(response);
  boost::beast::http::async_write(
      m_socket, m_response,
      boost::asio::bind_executor(m_strand, std::bind(&ServerSession::onResponseWritten, shared_from_this(),
                                                     std::placeholders::_1, std::placeholders::_2)));
}

void Xi::Http::HttpServerSession::doOnResponseWritten() { close(); }

void Xi::Http::HttpServerSession::doClose() {
  boost::beast::error_code ec;
  m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
  checkErrorCode(ec);
}
