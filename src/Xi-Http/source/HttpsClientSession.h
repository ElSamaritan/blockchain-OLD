#pragma once

#include <future>
#include <memory>
#include <utility>
#include <sstream>
#include <string>

#include <boost/optional.hpp>

#include <Xi/Global.h>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

#include "ClientSession.h"

namespace Xi {
namespace Http {
/*!
 * \brief The HttpsClientSession class implements a client session requiring ssl encryption.
 */
class HttpsClientSession : public ClientSession {
 public:
  HttpsClientSession(boost::asio::io_context& io, boost::asio::ssl::context& ctx,
                     std::shared_ptr<IClientSessionBuilder> builder);
  XI_DELETE_COPY(HttpsClientSession);
  XI_DEFAULT_MOVE(HttpsClientSession);
  ~HttpsClientSession() override = default;

 protected:
  void doPrepareRun() override;
  void doOnHostResolved(resolver_t::results_type results) override;
  void doOnConnected() override;
  void doOnRequestWritten() override;
  void doOnResponseRecieved() override;
  void doOnShutdown() override;

 private:
  /*!
   * \brief onHandshake called after initial handshake with the server
   */
  void onHandshake(boost::beast::error_code ec);

 private:
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_stream;
};
}  // namespace Http
}  // namespace Xi
