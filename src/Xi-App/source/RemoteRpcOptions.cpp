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
#include <Xi/Network/Uri.hpp>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(RemoteRpcOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidHost, "Given remote rpc host ist invalid.", RemoteRpcOption)
}  // namespace

void Xi::App::RemoteRpcOptions::loadEnvironment(Xi::App::Environment &env) {
  // clang-format off
  env
    (host(), "RPC_REMOTE_HOST")
    (accessToken(), "RPC_REMOTE_ACCESS_TOKEN")
  ;
  // clang-format on
}

void Xi::App::RemoteRpcOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("remote rpc")
    ("rpc-remote", "url to the rpc remote server (currently the daemon)",
      cxxopts::value<std::string>(host())->default_value(host()), "<url>")

    ("rpc-remote-access-token", "access token required by the rpc server",
      cxxopts::value<std::string>(accessToken())->default_value(accessToken()), "<token>")
  ;
  // clang-format on
}

bool Xi::App::RemoteRpcOptions::evaluateParsedOptions(const cxxopts::Options &options,
                                                      const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  const auto uri = Network::Uri::fromString(host());
  if (uri.isError()) {
    exceptional<InvalidHostError>("Remote rpc host is invalid: {}", uri.error().message());
  }
  if (!uri->scheme().empty()) {
    const auto protocol = Network::parseProtocol(uri->scheme());
    exceptional_if<InvalidHostError>(protocol.isError(), "Unsupported rpc scheme: {}", uri->scheme());
    exceptional_if<InvalidHostError>(isHttpBased(*protocol), "Unsupported rpc scheme: {}", uri->scheme());
  }
  return false;
}

CryptoNote::RpcRemoteConfiguration Xi::App::RemoteRpcOptions::getConfig(
    const Xi::Http::SSLConfiguration &sslConfig) const {
  CryptoNote::RpcRemoteConfiguration cfg{};
  const auto uri = Network::Uri::fromString(host()).takeOrThrow();
  if (uri.scheme().empty()) {
    if (sslConfig.enabled()) {
      cfg.Host = toString(Network::Protocol::Xis).takeOrThrow() + ":" + host();
    } else {
      cfg.Host = toString(Network::Protocol::Xi).takeOrThrow() + ":" + host();
    }
  } else {
    cfg.Host = host();
  }
  cfg.AccessToken = accessToken();
  cfg.Ssl = sslConfig;
  return cfg;
}
