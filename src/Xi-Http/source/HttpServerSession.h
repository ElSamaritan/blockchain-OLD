#pragma once

#include <memory>
#include <utility>

#include "ServerSession.h"

namespace Xi {
namespace Http {
class HttpServerSession : public ServerSession {
 public:
  using ServerSession::ServerSession;
  XI_DEFAULT_MOVE(HttpServerSession);
  ~HttpServerSession() override = default;

 protected:
  void doReadRequest() override;
  void doOnRequestRead() override;
  void doWriteResponse(Response&& response) override;
  void doOnResponseWritten() override;
  void doClose() override;

 private:
};
}  // namespace Http
}  // namespace Xi
