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

#include <string>
#include <cinttypes>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <boost/optional.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Config/Network.h>
#include <Xi/Http/SSLConfiguration.h>
#include <Common/StringTools.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Rpc/RpcRemoteConfiguration.h>

#include "Xi/App/IOptions.h"

namespace Xi {
namespace App {
struct RemoteRpcOptions : public IOptions {
  std::string Address = "127.0.0.1";
  uint16_t Port = Xi::Config::Network::Configuration::rpcDefaultPort();
  std::string AccessToken = "";

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(Address)
  KV_MEMBER(Port)
  KV_MEMBER(AccessToken)
  KV_END_SERIALIZATION

  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;

  CryptoNote::RpcRemoteConfiguration getConfig(const Xi::Http::SSLConfiguration& sslConfig) const;
};
}  // namespace App
}  // namespace Xi
