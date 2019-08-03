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

#include "Xi/App/ServerRpcOptions.h"

#include <cinttypes>
#include <limits>

#include <Common/StringTools.h>
#include <Xi/Exceptional.hpp>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(RpcServerOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidIpBind, "ip binded is invalid", RpcServerOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidPort, "port binded is invalid", RpcServerOptions)
}  // namespace

namespace Xi {
namespace App {

void ServerRpcOptions::loadEnvironment(Environment &env) {
  uint32_t readTimeout = std::numeric_limits<uint32_t>::max();
  uint32_t writeTimeout = std::numeric_limits<uint32_t>::max();
  uint32_t readLimit = std::numeric_limits<uint32_t>::max();
  uint32_t writeLimit = std::numeric_limits<uint32_t>::max();

  // clang-format off
  env
    (enabled(), "RPC_SERVER")
    (cors(), "RPC_SERVER_CORS")
    (bind(), "RPC_SERVER_BIND_IP")
    (port(), "RPC_SERVER_BIND_PORT")
    (accessToken(), "RPC_SERVER_ACCESS_TOKEN")
    (readTimeout, "RPC_SERVER_READ_RATE")
    (writeTimeout, "RPC_SERVER_READ_RATE")
    (readLimit, "RPC_SERVER_READ_RATE")
    (writeLimit, "RPC_SERVER_READ_RATE")
  ;
  // clang-format on

  if (readTimeout != std::numeric_limits<uint32_t>::max()) {
    limits().readTimeout(std::chrono::seconds{readTimeout});
  }
  if (writeTimeout != std::numeric_limits<uint32_t>::max()) {
    limits().writeTimeout(std::chrono::seconds{writeTimeout});
  }
  if (readLimit != std::numeric_limits<uint32_t>::max()) {
    limits().readLimit(readLimit);
  }
  if (writeLimit != std::numeric_limits<uint32_t>::max()) {
    limits().writeLimit(writeLimit);
  }
}

void ServerRpcOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("rpc server")
    ("rpc-server", "Enables a http server for remote protocol commands",
      cxxopts::value<bool>(enabled())->default_value(enabled() ? "true" : "false")->implicit_value("true"))

    ("rpc-server-cors", "Adds header 'Access-Control-Allow-Origin' to the RPC responses using the <domain>. "
                        "Uses the value specified as the domain. Use * for all.",
      cxxopts::value<std::string>(cors())->default_value(cors()))

    ("rpc-server-bind-ip", "Ip the server listens to.",
      cxxopts::value<std::string>(bind())->default_value(bind()), "<ipv4>")

    ("rpc-server-bind-port", "Port the http server shall listen to.",
      cxxopts::value<uint16_t>(port())->default_value(std::to_string(port())), "<port>")

    ("rpc-server-access-token", "Sets an access token required to access the rpc interface",
      cxxopts::value<std::string>()->default_value(accessToken()), "<token>")

    ("rpc-server-read-timeout", "Number of seconds a client has time to write all bytes"
                                " until the connection is considered as timed out.",
      cxxopts::value<uint32_t>()->default_value(std::to_string(limits().readTimeout().count())))

    ("rpc-server-write-timeout", "Number of seconds a client hat time to receive all bytes"
                                 " until the connections is considered as timed out.",
      cxxopts::value<uint32_t>()->default_value(std::to_string(limits().writeTimeout().count())))

    ("rpc-server-read-limit", "Maximum number of bytes per second read by the server from"
                              " a client connection.",
      cxxopts::value<uint32_t>()->default_value(std::to_string(limits().readLimit())))

    ("rpc-server-write-limit", "Maximum number of bytes per second written by the server"
                               " to the client.",
      cxxopts::value<uint32_t>()->default_value(std::to_string(limits().writeLimit())))
  ;
  // clang-format on
}

bool ServerRpcOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  uint32_t ip = 0;
  exceptional_if_not<InvalidIpBindError>(Common::parseIpAddress(ip, bind()));
  exceptional_if<InvalidPortError>(port() == 0);

  if (result.count("rpc-server-read-timeout") > 0) {
    limits().readTimeout(std::chrono::seconds{result["rpc-server-read-timeout"].as<uint32_t>()});
  }

  if (result.count("rpc-server-write-timeout") > 0) {
    limits().writeTimeout(std::chrono::seconds{result["rpc-server-write-timeout"].as<uint32_t>()});
  }

  if (result.count("rpc-server-read-limit") > 0) {
    limits().readLimit(result["rpc-server-read-limit"].as<uint32_t>());
  }

  if (result.count("rpc-server-write-limit") > 0) {
    limits().writeLimit(result["rpc-server-write-limit"].as<uint32_t>());
  }

  XI_RETURN_SC(false);
}

}  // namespace App
}  // namespace Xi
