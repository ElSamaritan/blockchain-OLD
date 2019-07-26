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

#include <Xi/Global.hh>
#include <Xi/Config/Network.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <P2p/NetNodeConfig.h>
#include <CryptoNoteCore/Currency.h>

#include "Xi/App/IOptions.h"

namespace Xi {
namespace App {
struct NodeOptions : public IOptions {
  XI_PROPERTY(std::string, bind, "0.0.0.0")
  XI_PROPERTY(uint16_t, port, Xi::Config::Network::Configuration::p2pDefaultPort())
  XI_PROPERTY(bool, hidePort, false)
  XI_PROPERTY(bool, localIp, false)
  XI_PROPERTY(uint16_t, externalPort, 0)
  XI_PROPERTY(uint16_t, banDuration, 60)
  XI_PROPERTY(bool, autoBan, false)
  XI_PROPERTY(std::vector<std::string>, addPeers, {})
  XI_PROPERTY(std::vector<std::string>, exclusivePeers, {})
  XI_PROPERTY(std::vector<std::string>, priorityPeers, {})
  XI_PROPERTY(std::vector<std::string>, seedPeers, {})

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(bind(), bind)
  KV_MEMBER_RENAME(port(), port)
  KV_MEMBER_RENAME(hidePort(), hide_port)
  KV_MEMBER_RENAME(localIp(), local_ip)
  KV_MEMBER_RENAME(externalPort(), external_port)
  KV_MEMBER_RENAME(banDuration(), ban_duration)
  KV_MEMBER_RENAME(autoBan(), auto_ban)
  KV_MEMBER_RENAME(addPeers(), peers_add)
  KV_MEMBER_RENAME(exclusivePeers(), peers_exclusive)
  KV_MEMBER_RENAME(priorityPeers(), peers_priority)
  KV_MEMBER_RENAME(seedPeers(), peers_seed)
  KV_END_SERIALIZATION

  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;

  CryptoNote::NetNodeConfig getConfig(const std::string& dataDir, const CryptoNote::Currency& currency) const;
};
}  // namespace App
}  // namespace Xi
