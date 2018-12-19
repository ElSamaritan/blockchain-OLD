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
/*!
 * \brief The Listener class is an abstract class to implement custom logic for tcp connection listener
 */
class Listener : public std::enable_shared_from_this<Listener> {
 public:
  /*!
   * \brief Listener creates a new bounded listener
   * \param endpoint the endpoint this listener is attached to
   */
  Listener(boost::asio::ip::tcp::endpoint endpoint);
  virtual ~Listener() = default;

  /*!
   * \brief run starts emitting async accept requests for incoming connections
   */
  virtual void run();

  /*!
   * \brief doAccept queries an incoming connection and forwards it to onAccept
   */
  void doAccept();

  /*!
   * \brief onAccept checks for errors and forward to the connection to doOnAccept
   */
  void onAccept(boost::beast::error_code ec);

 protected:
  /*!
   * \brief onError called whenever an internal error occured
   * \param ec the error code that occured
   * \param what a description of the step performed when the error occured
   */
  virtual void onError(boost::beast::error_code ec, const std::string& what);

  /*!
   * \brief onError called whenever any exception was thrown but were caught to prevent server shutdown
   * \param ex the exception thrown
   */
  virtual void onError(std::exception_ptr ex);

  /*!
   * \brief doOnAccept handles a newly established connection
   *
   * The implemenation should create a server session out of it, or check if its banned
   */
  virtual void doOnAccept(boost::asio::ip::tcp::socket socket) = 0;

  boost::asio::io_context io;

 private:
  boost::asio::ip::tcp::acceptor m_acceptor;
  boost::asio::ip::tcp::socket m_socket;
};
}  // namespace Http
}  // namespace Xi
