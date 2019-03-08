// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <string>
#include <system_error>

#include <Xi/Http/Server.h>
#include <Xi/Http/Request.h>
#include <Xi/Http/Response.h>
#include <Xi/Http/RequestHandler.h>

#include <System/Dispatcher.h>
#include <System/Event.h>
#include "Logging/ILogger.h"
#include "Logging/LoggerRef.h"

namespace CryptoNote {
class HttpResponse;
class HttpRequest;
}  // namespace CryptoNote

namespace Common {
class JsonValue;
}

namespace System {
class TcpConnection;
}

namespace PaymentService {
class ConfigurationManager;
}

namespace CryptoNote {

class JsonRpcServer : public Xi::Http::Server, public Xi::Http::RequestHandler {
 public:
  using JsonValue = Common::JsonValue;

 public:
  JsonRpcServer(System::Dispatcher& sys, System::Event& stopEvent, Logging::ILogger& loggerGroup,
                Xi::Http::SSLConfiguration sslConfig);
  JsonRpcServer(const JsonRpcServer&) = delete;

  void setCorsHeader(std::string cors);
  const std::string& corsHeader();

  void start(const std::string& bindAddress, uint16_t bindPort);

 protected:
  static void makeErrorResponse(const std::error_code& ec, Common::JsonValue& resp);
  static void makeMethodNotFoundResponse(Common::JsonValue& resp);
  static void makeInvalidPasswordResponse(Common::JsonValue& resp);
  static void makeGenericErrorReponse(Common::JsonValue& resp, const char* what, int errorCode = -32001);
  static void fillJsonResponse(const Common::JsonValue& v, Common::JsonValue& resp);
  static void prepareJsonResponse(const Common::JsonValue& req, Common::JsonValue& resp);
  static void makeJsonParsingErrorResponse(Common::JsonValue& resp);

  virtual void processJsonRpcRequest(const Common::JsonValue& req, Common::JsonValue& resp) = 0;

 private:
  // HttpServer
  virtual Xi::Http::Response doHandleRequest(const Xi::Http::Request& request) override;

  void emplaceDefaultHeaders(Xi::Http::Response& response) const;

  System::Dispatcher& system;
  System::Event& stopEvent;
  Logging::LoggerRef logger;
  std::string cors;
};

}  // namespace CryptoNote
