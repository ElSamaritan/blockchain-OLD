/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include "Xi/Rpc/JsonProviderEndpoint.hpp"

#include <sstream>
#include <optional>
#include <variant>
#include <chrono>

#include <Xi/Exceptions.hpp>
#include <Xi/Algorithm/String.h>
#include <Serialization/JsonOutputStreamSerializer.h>
#include <Serialization/NullOutputSerializer.h>
#include <Serialization/JsonInputStreamSerializer.h>

namespace {
using JsonRpcId = std::optional<std::variant<std::string, int64_t>>;

struct JsonRpcError {
  int64_t code;
  std::string message;
};

struct JsronRpcResponsePrefix {
  std::string jsonrpc{"2.0"};
  JsonRpcId id;
  // + error || result
};

struct JsronRpcRequestPrefix {
  std::string jsonrpc{"2.0"};
  std::string method{""};
  JsonRpcId id;
};
}  // namespace

namespace Xi {
namespace Rpc {

using Json = Common::JsonValue;

JsonProviderEndpoint::JsonProviderEndpoint(SharedIServiceProvider service) : m_service{service} {}

std::chrono::microseconds JsonProviderEndpoint::timeout() const { return std::chrono::seconds{1}; }

uint32_t JsonProviderEndpoint::batchLimit() const { return 10; }

bool JsonProviderEndpoint::acceptsRequest(const Http::Request &request) {
  XI_RETURN_EC_IF_NOT(request.method() == Http::Method::Post || request.method() == Http::Method::Options, false);
  return request.headers().contentType().value_or(Http::ContentType::Json) == Http::ContentType::Json;
}

Http::Response JsonProviderEndpoint::doHandleRequest(const Http::Request &request) {
  if (request.method() == Http::Method::Options) {
    return makeJsonResponse(Json{Json::NIL});
  }
  if (request.method() != Http::Method::Post) {
    return makeJsonResponse(makeError(ErrorCode::NotFound, "unknown http method, use (POST or OPTIONS)"));
  }

  if (request.headers().contains(Http::HeaderContainer::ContentType) &&
      request.headers().contentType() != Http::ContentType::Json) {
    return makeJsonResponse(makeError(ErrorCode::InvalidRequest, "only json is supported"));
  }

  Json jrequest{};
  try {
    jrequest = Json::fromString(request.body());
  } catch (std::exception &) {
    return makeJsonResponse(makeError(ErrorCode::ParseError, "invalid request body"));
  }

  Json jresponse{Json::NIL};
  if (jrequest.isObject()) {
    jresponse = handleBatch(std::move(jrequest), std::nullopt);
  } else if (jrequest.isArray()) {
    if (jrequest.size() == 0) {
      jresponse = makeError(ErrorCode::InvalidRequest, "empty batches are invalid");
    }
    jresponse = Json{Json::ARRAY};

    using clock = std::chrono::steady_clock;
    const auto batchStart = clock::now();

    for (size_t i = 0; i < jrequest.size(); ++i) {
      std::optional<ErrorCode> timeoutError{std::nullopt};
      if (clock::now() - batchStart > timeout()) {
        timeoutError = ErrorCode::Timeout;
      }
      auto batchResult = handleBatch(std::move(jrequest[i]), timeoutError);
      if (!batchResult.isNil()) {
        jresponse.pushBack(std::move(batchResult));
      }
    }
  } else {
    jresponse = makeError(ErrorCode::InvalidRequest, "root object must be array or object");
  }

  return makeJsonResponse(std::move(jresponse));
}

JsonProviderEndpoint::Json JsonProviderEndpoint::handleBatch(JsonProviderEndpoint::Json &&batch,
                                                             std::optional<ErrorCode> error) {
  if (!batch.isObject()) {
    return makeError(ErrorCode::InvalidRequest, "commands must be objects");
  }

  size_t expectedSize = 2;  // MUST contain jsronrpc and method

  // if not give, this is a notification (do not write any response), even on error.
  std::optional<Json> id{std::nullopt};
  try {
    if (batch.contains("id")) {
      id = batch("id");
      if (!(id->isNil() || id->isInteger() || id->isString())) {
        return makeError(ErrorCode::InvalidRequest, "id must be nil, a number or a string");
      }
      expectedSize += 1;
    }
  } catch (...) {
    return makeError(ErrorCode::ParseError, "error parsing id");
  }

  try {
    if (!batch.contains("jsonrpc")) {
      return makeError(ErrorCode::InvalidRequest, move(id), "jsonrpc missing");
    }
    Json jsonrpc = std::move(batch("jsonrpc"));
    if (!jsonrpc.isString()) {
      return makeError(ErrorCode::InvalidRequest, move(id), "jsonrpc must be a string");
    }
    if (jsonrpc.getString() != "2.0") {
      return makeError(ErrorCode::InvalidRequest, move(id), "invalid jsonrpc version (only '2.0' supported)");
    }

    if (!batch.contains("method")) {
      return makeError(ErrorCode::InvalidRequest, move(id), "method missing");
    }
    Json method = std::move(batch("method"));
    if (!method.isString()) {
      return makeError(ErrorCode::InvalidRequest, move(id), "method must be a string");
    }
    std::string smethod = method.getString();
    if (starts_with(smethod, "rpc\u002E") || starts_with(smethod, "rpc.")) {
      return makeError(ErrorCode::InvalidRequest, move(id), "reserved method prefix used");
    }

    Json params{Json::NIL};
    if (batch.contains("params")) {
      params = std::move(batch("params"));
      expectedSize += 1;
    }

    if (batch.size() != expectedSize) {
      return makeError(ErrorCode::InvalidRequest, move(id),
                       "trailing properties, only use properties defined by the specification");
    }

    if (!m_service) {
      return makeError(ErrorCode::NotInitialized, std::move(id), "no service endpoint given");
    }

    if (error.has_value()) {
      return makeError(*error, std::move(id), "");
    }

    Json paramsWrapper{Json::OBJECT};
    paramsWrapper.set("", std::move(params));
    CryptoNote::JsonInputValueSerializer input{std::move(paramsWrapper)};

    if (id.has_value()) {
      CryptoNote::JsonOutputStreamSerializer output{};
      const auto result = (*m_service)(smethod, input, output);

      switch (result) {
        case ServiceError::Exceptional:
          return makeError(ErrorCode::InternalServerError, std::move(id));
        case ServiceError::Timeout:
          return makeError(ErrorCode::Timeout, std::move(id), "operation timed out");
        case ServiceError::NotInitialized:
          return makeError(ErrorCode::NotInitialized, std::move(id), "service is not fully initialized");
        case ServiceError::InvalidArgument:
          return makeError(ErrorCode::InvalidParameter, std::move(id), "request contains invalid parameter(s)");
        case ServiceError::SerializationError:
          return makeError(ErrorCode::InternalServerError, std::move(id), "request object is invalid");
        case ServiceError::NotFound:
          return makeError(ErrorCode::InternalServerError, std::move(id), "service command not found");
        case ServiceError::Success:
          break;
      }
      exceptional_if_not<RuntimeError>(result == ServiceError::Success, "unknonw service return coce");
      auto &outValue = output.getValue();
      if (!outValue.contains("")) {
        return makeResult(Json{Json::NIL}, move(id));
      } else {
        return makeResult(std::move(outValue("")), move(id));
      }
    } else {
      CryptoNote::NullOutputSerializer output{};
      [[maybe_unused]] const auto result = (*m_service)(smethod, input, output);
      return Json{Json::NIL};
    }

  } catch (std::exception &e) {
#if !defined(NDEBUG)
    return makeError(ErrorCode::InternalServerError, std::move(id), e.what());
#else
    XI_UNUSED(e);
    return makeError(ErrorCode::InternalServerError, std::move(id), "");
#endif
  }
}

Http::Response JsonProviderEndpoint::makeJsonResponse(JsonProviderEndpoint::Json &&value) {
  Http::Response response{Http::StatusCode::Ok};
  response.headers().setAllow({Xi::Http::Method::Post, Xi::Http::Method::Options});
  response.headers().setContentType(Xi::Http::ContentType::Json);
  if (!value.isNil()) {
    response.setBody(value.toString());
  }
  return response;
}

JsonProviderEndpoint::Json JsonProviderEndpoint::makeError(ErrorCode code, std::string_view message) const {
  Json reval{Json::OBJECT};
  reval.set("jsonrpc", Json{"2.0"});
  reval.set("id", Json{Json::NIL});
  {
    Json error{Json::OBJECT};
    error.set("code", Json{static_cast<int64_t>(code)});
    error.set("message", Json{message});
    reval.set("error", error);
  }
  return reval;
}

JsonProviderEndpoint::Json JsonProviderEndpoint::makeError(JsonProviderEndpoint::ErrorCode code,
                                                           std::optional<Json> &&id, std::string_view message) const {
  if (id.has_value()) {
    Json reval = makeError(code, message);
    reval.set("id", std::move(*id));
    return reval;
  } else {
    return Json{Json::NIL};
  }
}

JsonProviderEndpoint::Json JsonProviderEndpoint::makeResult(JsonProviderEndpoint::Json &&result,
                                                            std::optional<JsonProviderEndpoint::Json> &&id) const {
  // May not make a result for a notification.
  if (!id.has_value()) {
    return Json{Json::NIL};
  }

  Json reval{Json::OBJECT};
  reval.set("jsonrpc", Json{"2.0"});
  reval.set("id", std::move(*id));
  reval.set("result", std::move(result));
  return reval;
}

Http::Response JsonProviderEndpoint::doMakeBadRequest(const std::string &why) {
  return makeJsonResponse(makeError(ErrorCode::InvalidRequest, why));
}

Http::Response JsonProviderEndpoint::doMakeNotFound(const std::string &why) {
  return makeJsonResponse(makeError(ErrorCode::NotFound, why));
}

Http::Response JsonProviderEndpoint::doMakeNotImplemented() { return makeJsonResponse(makeError(ErrorCode::NotFound)); }

Http::Response JsonProviderEndpoint::doMakeInternalServerError(const std::string &why) {
  return makeJsonResponse(makeError(ErrorCode::InternalServerError, why));
}

}  // namespace Rpc
}  // namespace Xi
