#pragma once

#include <sstream>
#include <memory>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "Xi/Http/Request.h"
#include "Xi/Http/Response.h"

#include "IClientSessionBuilder.h"

namespace Xi {
namespace Http {
class ClientSession : public std::enable_shared_from_this<ClientSession> {
 public:
  using future_t = std::future<Response>;
  using promise_t = std::promise<Response>;
  using resolver_t = boost::asio::ip::tcp::resolver;
  using request_t = boost::beast::http::request<boost::beast::http::string_body>;
  using response_t = boost::beast::http::response<boost::beast::http::string_body>;
  using buffer_t = boost::beast::flat_buffer;

  ClientSession(boost::asio::io_context& io, IClientSessionBuilder& builder);
  XI_DELETE_COPY(ClientSession);
  XI_DEFAULT_MOVE(ClientSession);
  virtual ~ClientSession() = default;

  future_t run(char const* host, char const* port, Request&& request);

  void onHostResolved(boost::beast::error_code ec, resolver_t::results_type results);
  void onConnected(boost::beast::error_code ec);
  void onRequestWritten(boost::beast::error_code ec, std::size_t bytesTransfered);
  void onResponseRecieved(boost::beast::error_code ec, std::size_t bytesTransfered);
  void onShutdown(boost::beast::error_code ec);

 protected:
  virtual void doPrepareRun() = 0;
  virtual void doOnHostResolved(resolver_t::results_type results) = 0;
  virtual void doOnConnected() = 0;
  virtual void doOnRequestWritten() = 0;
  virtual void doOnResponseRecieved() = 0;
  virtual void doOnShutdown() = 0;

  const char* host();
  void checkErrorCode(const boost::beast::error_code& ec);
  void fail(const std::exception_ptr& ex);

 protected:
  request_t m_request;
  response_t m_response;
  buffer_t m_buffer;

 private:
  void decodeResponse();
  void run();
  void redirect();
  void finalize();
  void swallowErrorCode(const boost::beast::error_code& ec);

  void copyHeader(HeaderContainer::Header header);
  void copyHeaders();
  void copyHeaders(const Request& request);

 private:
  boost::asio::io_context& m_io;
  boost::asio::ip::tcp::resolver m_resolver;

  IClientSessionBuilder& m_builder;
  std::stringstream m_bodyDecodingBuffer;
  promise_t m_promise;
  Request m_originalRequest;
  Response m_responseToEmit;
  std::string m_host;
  std::string m_port;
  bool m_isRedirected;

  std::shared_ptr<ClientSession> m_selfHoldingPointer;
};
}  // namespace Http
}  // namespace Xi
