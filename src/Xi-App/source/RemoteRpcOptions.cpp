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

#include "Xi/App/RemoteRpcOptions.h"

#include <Xi/Global.hh>
#include <Xi/Exceptional.hpp>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(RemoteRpcOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidIpFormat, "provided ip address has an invalid format.", RemoteRpcOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidPort, "zero is not a valid port.", RemoteRpcOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidHostFormat, "provided host has an invalid format.", RemoteRpcOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(AmbiguousEndpoint, "you may not provde a port or address if you provide a host.",
                                RemoteRpcOption)
}  // namespace

void Xi::App::RemoteRpcOptions::loadEnvironment(Xi::App::Environment &env) {
  // clang-format off
  env
    (address(), "RPC_REMOTE_ADDRESS")
    (port(), "RPC_REMOTE_PORT")
    (accessToken(), "RPC_REMOTE_ACCESS_TOKEN")
  ;
  // clang-format on
}

void Xi::App::RemoteRpcOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("remote rpc")
    ("rpc-remote-address", "remote ip address running the rpc server.",
      cxxopts::value<std::string>(address())->default_value(address()), "#.#.#.#")

    ("rpc-remote-port", "remote port the rpc server is listening on.",
      cxxopts::value<uint16_t>(port())->default_value(std::to_string(port())), "#")

    ("rpc-remote-host", "unified ip address and port of the remote rpc server",
      cxxopts::value<std::string>(), "#.#.#.#:#")

    ("rpc-remote-access-token", "access token required by the rpc server",
      cxxopts::value<std::string>(accessToken())->default_value(accessToken()), "<token>")
  ;
  // clang-format on
}

bool Xi::App::RemoteRpcOptions::evaluateParsedOptions(const cxxopts::Options &options,
                                                      const cxxopts::ParseResult &result) {
  XI_UNUSED(options);

  if (result.count("rpc-remote-host") > 0) {
    if (result.count("rpc-remote-address") > 0 || result.count("rpc-remote-port") > 0) {
      exceptional<AmbiguousEndpointError>();
    }
    uint32_t _ip = 0;
    uint16_t _port = 0;
    if (!Common::parseIpAddressAndPort(_ip, _port, result["rpc-remote-host"].as<std::string>())) {
      exceptional<InvalidHostFormatError>();
    }

    address() = Common::ipAddressToString(_ip);
    port() = _port;
  } else {
    if (port() < 1) {
      exceptional<InvalidPortError>();
    }
    [[maybe_unused]] uint32_t _ = 0;
    if (!Common::parseIpAddress(_, address())) {
      exceptional<InvalidIpFormatError>();
    }
  }
  return false;
}

CryptoNote::RpcRemoteConfiguration Xi::App::RemoteRpcOptions::getConfig(
    const Xi::Http::SSLConfiguration &sslConfig) const {
  CryptoNote::RpcRemoteConfiguration cfg{};
  cfg.Host = address();
  cfg.Port = port();
  cfg.AccessToken = accessToken();
  cfg.Ssl = sslConfig;
  return cfg;
}
