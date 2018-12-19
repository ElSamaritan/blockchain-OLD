#pragma once

#include <memory>
#include <string>

#include "Xi/Http/RequestHandler.h"

namespace Xi {
namespace Http {
class Server {
 public:
  void setHandler(std::shared_ptr<RequestHandler> handler);
  std::shared_ptr<RequestHandler> handler() const;

  void start(const std::string& address, uint16_t port);
  void stop();

 private:
  std::shared_ptr<RequestHandler> m_handler;

  struct _Listener;
  std::shared_ptr<_Listener> m_listener;
};
}  // namespace Http
}  // namespace Xi
