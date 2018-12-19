#pragma once

#include <memory>
#include <string>

#include "Xi/Http/RequestHandler.h"

namespace Xi {
namespace Http {
/*!
 * \brief The Server class provies a http/https capable server implmentation
 */
class Server {
 public:
  /*!
   * \brief setHandler sets the handle that should be called for resolving requests
   *
   * If you want to provide multiple handlers you may consider a \see Router
   */
  void setHandler(std::shared_ptr<RequestHandler> handler);

  /*!
   * \brief handler the reciever of request, creating responses
   */
  std::shared_ptr<RequestHandler> handler() const;

  /*!
   * \brief start starts the server listening on given address, port (non blocking)
   * \param address the ip this server runs on
   * \param port the port that should be used
   *
   * \exception std::runtime_error If the server is already running
   * \exception std::runtime_error If no handler was set
   */
  void start(const std::string& address, uint16_t port);

  /*!
   * \brief stop gracefully shutdowns the server
   */
  void stop();

 private:
  std::shared_ptr<RequestHandler> m_handler;

  struct _Listener;
  std::shared_ptr<_Listener> m_listener;
};
}  // namespace Http
}  // namespace Xi
