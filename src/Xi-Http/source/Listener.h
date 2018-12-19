#pragma once

#include <memory>
#include <string>

#include <Xi/Global.h>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include "ServerSession.h"

namespace Xi {
namespace Http {
class Listener : public std::enable_shared_from_this<Listener> {
 public:
  Listener(boost::asio::ip::tcp::endpoint endpoint);
  virtual ~Listener() = default;

  virtual void run();
  void doAccept();
  void onAccept(boost::beast::error_code ec);

 protected:
  virtual void onError(boost::beast::error_code ec, const std::string& what);
  virtual void doOnAccept(boost::asio::ip::tcp::socket socket) = 0;

  boost::asio::io_context io;

 private:
  boost::asio::ip::tcp::acceptor m_acceptor;
  boost::asio::ip::tcp::socket m_socket;
};
}  // namespace Http
}  // namespace Xi
