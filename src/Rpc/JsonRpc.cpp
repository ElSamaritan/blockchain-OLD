// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "JsonRpc.h"

namespace CryptoNote {

namespace JsonRpc {

JsonRpcError::JsonRpcError() : code(0) {
}

JsonRpcError::JsonRpcError(int c) : code(c) {
  switch (c) {
    case errParseError:
      message = "Parse error";
      break;
    case errInvalidRequest:
      message = "Invalid request";
      break;
    case errMethodNotFound:
      message = "Method not found";
      break;
    case errInvalidParams:
      message = "Invalid params";
      break;
    case errInternalError:
      message = "Internal error";
      break;
    case errInvalidPassword:
      message = "Invalid or no password supplied";
      break;
    default:
      message = "Unknown error";
      break;
  }
}

JsonRpcError::JsonRpcError(int c, const std::string& msg) : code(c), message(msg) {
}

void invokeJsonRpcCommand(HttpClient& httpClient, JsonRpcRequest& jsReq, JsonRpcResponse& jsRes) {
  using namespace Xi::Http;

  const auto httpRes = httpClient.postSync("/json_rpc", ContentType::Json, jsReq.getBody());

  if (httpRes.status() != StatusCode::Ok) {
    throw std::runtime_error("JSON-RPC call failed, HTTP status = " + Xi::to_string(httpRes.status()));
  }

  jsRes.parse(httpRes.body());

  JsonRpcError err;
  if (jsRes.getError(err)) {
    throw err;
  }
}

}  // namespace JsonRpc
}  // namespace CryptoNote
