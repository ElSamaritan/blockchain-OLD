#pragma once

#include <memory>
#include <utility>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/asio/ssl.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include "HttpServerSession.h"

namespace Xi {
namespace Http {
class HttpsServerSession : public ServerSession {
 public:
  HttpsServerSession(socket_t socket, buffer_t buffer, boost::asio::ssl::context& ctx,
                     std::shared_ptr<RequestHandler> handler, Concurrent::IDispatcher& dispatcher);
  XI_DEFAULT_MOVE(HttpsServerSession);
  ~HttpsServerSession() override = default;

  void run() override;

  void onHandshake(boost::beast::error_code ec, std::size_t bytes_used);
  void onShutdown(boost::beast::error_code ec);

 protected:
  void doReadRequest() override;
  void doOnRequestRead() override;
  void doWriteResponse(Response&& response) override;
  void doOnResponseWritten() override;
  void doClose() override;

 private:
  boost::asio::ssl::stream<socket_t&> m_stream;
};
}  // namespace Http
}  // namespace Xi
