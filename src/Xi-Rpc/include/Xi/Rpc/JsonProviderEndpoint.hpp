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

#pragma once

#include <cinttypes>
#include <string_view>
#include <chrono>

#include <Xi/Http/Endpoint.h>
#include <Common/JsonValue.h>

#include "Xi/Rpc/IServiceProvider.hpp"

namespace Xi {
namespace Rpc {

/// https://www.jsonrpc.org/specification
class JsonProviderEndpoint : public Http::Endpoint {
 public:
  using Json = Common::JsonValue;

  enum struct ErrorCode : int64_t {
    ParseError = -32700,        ///	Parse error	Invalid JSON was received by the server. An error occurred on the server
                                /// while parsing the JSON text.
    InvalidRequest = -32600,    ///	Invalid Request	The JSON sent is not a valid Request object.
    NotFound = -32601,          ///	Method not found	The method does not exist / is not available.
    InvalidParameter = -32602,  ///	Invalid params	Invalid method parameter(s).
    InternalServerError = -32603,  /// Internal error	Internal JSON-RPC error.

    // First usable user defined error code: -32000
    Timeout = -32000,         /// Operation timed out.
    NotInitialized = -32001,  /// Underlying service is not initialized.
    LimitReached = -32002,    /// Number of batches exceeded limitation.
    Unauthorized = -32003,    /// Authorization failed.
    // Last usable user defined error code: -32299
  };

 public:
  JsonProviderEndpoint(SharedIServiceProvider service);
  ~JsonProviderEndpoint() override = default;

  std::chrono::microseconds timeout() const;
  uint32_t batchLimit() const;

  bool acceptsRequest(const Http::Request& request) override;

 protected:
  Http::Response doHandleRequest(const Http::Request& request) override;

  /*!
   * \brief handleBatch Handles an individual command.
   * \param icommand The current command being proccessed.
   * \param error Set to a value if already an error occured.
   * \return Response object for this command, if NIL it is omitted.
   */
  Json handleBatch(Json&& icommand, std::optional<ErrorCode> error);

  Http::Response makeJsonResponse(Json&& value);
  Json makeError(ErrorCode code, std::string_view message = "") const;
  Json makeError(ErrorCode code, std::optional<Json>&& id, std::string_view message = "") const;
  Json makeResult(Json&& result, std::optional<Json>&& id) const;

  Http::Response doMakeBadRequest(const std::string& why) override;
  Http::Response doMakeNotFound(const std::string& why) override;
  Http::Response doMakeUnauthorized(const std::string& why) override;
  Http::Response doMakeNotImplemented() override;
  Http::Response doMakeInternalServerError(const std::string& why) override;

 private:
  SharedIServiceProvider m_service;
};

}  // namespace Rpc
}  // namespace Xi
