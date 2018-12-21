#pragma once

#include <memory>
#include <utility>

#include "ServerSession.h"

namespace Xi {
namespace Http {
class IServerSessionBuilder {
 public:
  virtual ~IServerSessionBuilder() = default;

  virtual std::shared_ptr<ServerSession> makeHttpServerSession(ServerSession::socket_t socket,
                                                               ServerSession::buffer_t buffer) = 0;
  virtual std::shared_ptr<ServerSession> makeHttpsServerSession(ServerSession::socket_t socket,
                                                                ServerSession::buffer_t buffer) = 0;
};
}  // namespace Http
}  // namespace Xi
