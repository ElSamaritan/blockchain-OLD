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

#include <Xi/Global.hh>
#include <Xi/Config/Network.h>
#include <Serialization/ISerializer.h>
#include <Xi/Http/ServerLimitsConfiguration.h>

#include "Xi/App/IOptions.h"

namespace Xi {
namespace App {

class ServerRpcOptions : public IOptions {
 public:
  XI_PROPERTY(bool, enabled, false)
  XI_PROPERTY(std::string, bind, "127.0.0.1")
  XI_PROPERTY(uint16_t, port, Config::Network::Configuration::rpcDefaultPort())
  XI_PROPERTY(std::string, accessToken, "")
  XI_PROPERTY(std::string, cors, "")
  XI_PROPERTY(Http::ServerLimitsConfiguration, limits)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(enabled(), enabled)
  KV_MEMBER_RENAME(bind(), bind)
  KV_MEMBER_RENAME(port(), port)
  KV_MEMBER_RENAME(accessToken(), access_token)
  KV_MEMBER_RENAME(cors(), cors)
  KV_MEMBER_RENAME(limits(), limits)
  KV_END_SERIALIZATION

  void loadEnvironment(Environment& env) override;
  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;
};

}  // namespace App
}  // namespace Xi
