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

#include "ClientSession.h"

namespace Xi {
namespace Http {
/*!
 * \brief The HttpClientSession class implements a client session without ssl encryption.
 */
class HttpClientSession : public ClientSession {
 public:
  HttpClientSession(boost::asio::io_context& io, std::shared_ptr<IClientSessionBuilder> builder);
  XI_DELETE_COPY(HttpClientSession);
  XI_DEFAULT_MOVE(HttpClientSession);
  ~HttpClientSession() override = default;

 protected:
  void doPrepareRun() override;
  void doOnHostResolved(resolver_t::results_type results) override;
  void doOnConnected() override;
  void doOnRequestWritten() override;
  void doOnResponseRecieved() override;
  void doOnShutdown() override;

 private:
  boost::asio::ip::tcp::socket m_socket;
};
}  // namespace Http
}  // namespace Xi
