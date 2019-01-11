// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "JsonRpcServer.h"

#include <fstream>
#include <future>
#include <system_error>
#include <memory>
#include <sstream>

#include <Xi/Concurrent/SystemDispatcher.h>

#include <System/TcpConnection.h>
#include <System/TcpListener.h>
#include <System/TcpStream.h>
#include <System/Ipv4Address.h>
#include "Rpc/JsonRpc.h"

#include "Common/JsonValue.h"
#include "Serialization/JsonInputValueSerializer.h"
#include "Serialization/JsonOutputStreamSerializer.h"

namespace CryptoNote {

JsonRpcServer::JsonRpcServer(System::Dispatcher& sys, System::Event& stopEvent, Logging::ILogger& loggerGroup,
                             PaymentService::ConfigurationManager& config)
    : Server(), system(sys), stopEvent(stopEvent), logger(loggerGroup, "JsonRpcServer"), config(config) {
  setDispatcher(std::make_shared<Xi::Concurrent::SystemDispatcher>(sys));
  setSSLConfiguration(config.serviceConfig.sslServer);
}

void JsonRpcServer::start(const std::string& bindAddress, uint16_t bindPort) {
  Server::start(bindAddress, bindPort);
  stopEvent.wait();
  Server::stop();
}

Xi::Http::Response JsonRpcServer::doHandleRequest(const Xi::Http::Request& request) {
  try {
    logger(Logging::TRACE) << "HTTP request came: \n" << request.body();

    if (request.target() == "/json_rpc") {
      if (request.method() == Xi::Http::Method::Options) {
        Xi::Http::Response response;
        emplaceDefaultHeaders(response);
        return response;
      } else if (request.method() != Xi::Http::Method::Post) {
        return makeBadRequest("Only POST and OPTIONS methods are allowed.");
      }

      std::istringstream jsonInputStream(request.body());
      Common::JsonValue jsonRpcRequest;
      Common::JsonValue jsonRpcResponse(Common::JsonValue::OBJECT);

      try {
        jsonInputStream >> jsonRpcRequest;
      } catch (std::runtime_error&) {
        logger(Logging::DEBUGGING) << "Couldn't parse request: \"" << request.body() << "\"";
        makeJsonParsingErrorResponse(jsonRpcResponse);
        return Xi::Http::Response{Xi::Http::StatusCode::Ok, jsonRpcResponse.toString(), Xi::Http::ContentType::Json};
      }

      processJsonRpcRequest(jsonRpcRequest, jsonRpcResponse);

      std::ostringstream jsonOutputStream;
      jsonOutputStream << jsonRpcResponse;

      Xi::Http::Response resp;
      emplaceDefaultHeaders(resp);

      resp.setStatus(Xi::Http::StatusCode::Ok);
      resp.setBody(jsonOutputStream.str());

      return resp;
    } else {
      logger(Logging::WARNING) << "Requested url \"" << request.target() << "\" is not found";
      return makeNotFound();
    }
  } catch (std::exception& e) {
    logger(Logging::WARNING) << "Error while processing http request: " << e.what();
    return makeInternalServerError();
  }
}

void JsonRpcServer::emplaceDefaultHeaders(Xi::Http::Response& response) const {
  if (!config.serviceConfig.corsHeader.empty()) {
    response.headers().set(Xi::Http::HeaderContainer::AccessControlAllowOrigin, config.serviceConfig.corsHeader);
    response.headers().setAccessControlRequestMethods({Xi::Http::Method::Post, Xi::Http::Method::Options});
  }
  response.headers().setAllow({Xi::Http::Method::Post, Xi::Http::Method::Options});
  response.headers().setContentType(Xi::Http::ContentType::Json);
}

void JsonRpcServer::prepareJsonResponse(const Common::JsonValue& req, Common::JsonValue& resp) {
  using Common::JsonValue;

  if (req.contains("id")) {
    resp.insert("id", req("id"));
  }

  resp.insert("jsonrpc", "2.0");
}

void JsonRpcServer::makeErrorResponse(const std::error_code& ec, Common::JsonValue& resp) {
  using Common::JsonValue;

  JsonValue error(JsonValue::OBJECT);

  JsonValue code;
  code = static_cast<int64_t>(CryptoNote::JsonRpc::errParseError);  // Application specific error code

  JsonValue message;
  message = ec.message();

  JsonValue data(JsonValue::OBJECT);
  JsonValue appCode;
  appCode = static_cast<int64_t>(ec.value());
  data.insert("application_code", appCode);

  error.insert("code", code);
  error.insert("message", message);
  error.insert("data", data);

  resp.insert("error", error);
}

void JsonRpcServer::makeGenericErrorReponse(Common::JsonValue& resp, const char* what, int errorCode) {
  using Common::JsonValue;

  JsonValue error(JsonValue::OBJECT);

  JsonValue code;
  code = static_cast<int64_t>(errorCode);

  std::string msg;
  if (what) {
    msg = what;
  } else {
    msg = "Unknown application error";
  }

  JsonValue message;
  message = msg;

  error.insert("code", code);
  error.insert("message", message);

  resp.insert("error", error);
}

void JsonRpcServer::makeMethodNotFoundResponse(Common::JsonValue& resp) {
  using Common::JsonValue;

  JsonValue error(JsonValue::OBJECT);

  JsonValue code;
  code = static_cast<int64_t>(CryptoNote::JsonRpc::errMethodNotFound);  // ambigous declaration of JsonValue::operator=
                                                                        // (between int and JsonValue)

  JsonValue message;
  message = "Method not found";

  error.insert("code", code);
  error.insert("message", message);

  resp.insert("error", error);
}

void JsonRpcServer::makeInvalidPasswordResponse(Common::JsonValue& resp) {
  using Common::JsonValue;

  JsonValue error(JsonValue::OBJECT);

  JsonValue code;
  code = static_cast<int64_t>(CryptoNote::JsonRpc::errInvalidPassword);

  JsonValue message;
  message = "Invalid or no rpc password";

  error.insert("code", code);
  error.insert("message", message);

  resp.insert("error", error);
}

void JsonRpcServer::fillJsonResponse(const Common::JsonValue& v, Common::JsonValue& resp) { resp.insert("result", v); }

void JsonRpcServer::makeJsonParsingErrorResponse(Common::JsonValue& resp) {
  using Common::JsonValue;

  resp = JsonValue(JsonValue::OBJECT);
  resp.insert("jsonrpc", "2.0");
  resp.insert("id", nullptr);

  JsonValue error(JsonValue::OBJECT);
  JsonValue code;
  code = static_cast<int64_t>(
      CryptoNote::JsonRpc::errParseError);  // ambigous declaration of JsonValue::operator= (between int and JsonValue)

  JsonValue message = "Parse error";

  error.insert("code", code);
  error.insert("message", message);

  resp.insert("error", error);
}

}  // namespace CryptoNote
