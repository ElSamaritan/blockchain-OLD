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

#pragma once

#include <memory>
#include <string>

#include <Xi/Http/Client.h>
#include <Xi/Http/Request.h>
#include <Xi/Http/Response.h>

#include <HTTP/HttpRequest.h>
#include <HTTP/HttpResponse.h>
#include <System/TcpConnection.h>
#include <System/TcpStream.h>

#include "Serialization/SerializationTools.h"

namespace CryptoNote {

class ConnectException : public std::runtime_error {
 public:
  ConnectException(const std::string& whatArg);
};

using HttpClient = Xi::Http::Client;

template <typename Request, typename Response>
void invokeJsonCommand(HttpClient& client, const std::string& url, const Request& req, Response& res) {
  using namespace ::Xi::Http;

  const auto response = client.postSync(url, Xi::Http::ContentType::Json, storeToJson(req));
  if (response.status() != StatusCode::Ok) {
    throw std::runtime_error("HTTP status: " + Xi::to_string(response.status()));
  }

  if (!loadFromJson(res, response.body())) {
    throw std::runtime_error("Failed to parse JSON response");
  }
}

template <typename Request, typename Response>
void invokeBinaryCommand(HttpClient& client, const std::string& url, const Request& req, Response& res) {
  using namespace ::Xi::Http;

  const auto response = client.postSync(url, Xi::Http::ContentType::Binary, storeToBinaryKeyValue(req));
  if (response.getStatus() != StatusCode::Ok) {
    throw std::runtime_error("HTTP status: " + std::to_string(response.getStatus()));
  }

  if (!loadFromBinaryKeyValue(res, response.getBody())) {
    throw std::runtime_error("Failed to parse binary response");
  }
}

}  // namespace CryptoNote
