#pragma once

#include <memory>
#include <utility>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Global.h>

#include "Xi/Http/RequestHandler.h"

#include "BeastConversion.h"

namespace Xi {
namespace Http {
class ServerSession : public std::enable_shared_from_this<ServerSession> {
  XI_DELETE_COPY(ServerSession);

 public:
  using buffer_t = boost::beast::flat_buffer;
  using request_t = BeastConversion::beast_request_t;
  using response_t = BeastConversion::beast_response_t;
  using socket_t = boost::asio::ip::tcp::socket;
  using strand_t = boost::asio::strand<boost::asio::io_context::executor_type>;

 public:
  ServerSession(socket_t socket, std::shared_ptr<RequestHandler> handler);
  XI_DEFAULT_MOVE(ServerSession);
  ~ServerSession();

  void run();

  void readRequest();
  void onRequestRead(boost::beast::error_code ec, std::size_t bytesTransfered);
  void writeResponse();
  void onResponseWritten(boost::beast::error_code ec, std::size_t bytesTransfered);
  void close();

 protected:
  void checkErrorCode(boost::beast::error_code ec);
  void fail(std::exception_ptr ex);

  void doReadRequest();
  void doOnRequestRead();
  void doWriteResponse();
  void doOnResponseWritten();
  void doClose();

 private:
  socket_t m_socket;
  strand_t m_strand;
  buffer_t m_buffer;
  std::shared_ptr<RequestHandler> m_handler;
  request_t m_request;
  response_t m_response;
  BeastConversion m_conversion;
};
}  // namespace Http
}  // namespace Xi
