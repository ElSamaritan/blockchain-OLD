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

#include "Xi/App/NodeOptions.h"

#include <algorithm>

#include <Xi/Exceptional.hpp>
#include <Common/StringTools.h>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(NodeOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidIp, "ip provided is invalid", NodeOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidHost, "host provided is invalid", NodeOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(NullBanDuration, "autoban is enabled but ban duration zero", NodeOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidConfiguration, "net node initialization failed", NodeOptions)
}  // namespace

namespace Xi {
namespace App {

void NodeOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("p2p")
    ("p2p-bind-ip", "the ip address your node listens to",
      cxxopts::value<std::string>(bind())->default_value(bind()), "<ip4>")
    ("p2p-bind-port", "the explicit ports you node should listen on",
      cxxopts::value<uint16_t>(port())->default_value(std::to_string(port())), "<port>")
    ("p2p-hide-port", "hides your local port from being published to the network",
      cxxopts::value<bool>(hidePort())->default_value("false")->implicit_value("true"), "<enabled>")
    ("p2p-local-ip", "enables connections to local ips, useful for running mutiple nodes on one server",
      cxxopts::value<bool>(localIp())->default_value("false")->implicit_value("true"), "<enabled>")
    ("p2p-external-port", "the port opened or to open on your router",
      cxxopts::value<uint16_t>(externalPort())->default_value(std::to_string(externalPort())), "<port>")
    ("p2p-ban-duration", "the ban duration to apply as default on newly banned peers",
      cxxopts::value<uint16_t>(banDuration())->default_value(std::to_string(banDuration())), "<minutes>")
    ("p2p-ban-auto", "enabled protection mechnisms that automatically ban malicious peers",
      cxxopts::value<bool>(autoBan())->default_value("false")->implicit_value("true"), "<enabled>")
    ("p2p-peers-add", "Adds additional peers to the peerlist on startup",
      cxxopts::value<std::vector<std::string>>(addPeers()), "<ip4:port>*")
    ("p2p-peers-exclusive", "Adds exclusive peers, your node will only connect to those.",
      cxxopts::value<std::vector<std::string>>(exclusivePeers()), "<ip4:port>*")
    ("p2p-peers-priority", "Adds peers with a higher priority, prefered to connect to.",
      cxxopts::value<std::vector<std::string>>(priorityPeers()), "<ip4:port>*")
    ("p2p-peers-seed", "Adds additional seed peers.",
      cxxopts::value<std::vector<std::string>>(seedPeers()), "<ip4:port>*")
  ;
  // clang-format on
}

bool NodeOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  exceptional_if<NullBanDurationError>(autoBan() && banDuration() == 0);

  {
    uint32_t _ = 0;
    exceptional_if_not<InvalidIpError>(Common::parseIpAddress(_, bind()), bind());
  }

  const auto checkHosts = [](std::vector<std::string> &hosts) {
    hosts.erase(std::remove_if(hosts.begin(), hosts.end(), [](const auto &host) { return host.empty(); }), hosts.end());
    for (const auto &host : hosts) {
      uint32_t ip = 0;
      uint16_t port = 0;
      exceptional_if_not<InvalidHostError>(Common::parseIpAddressAndPort(ip, port, host), host);
    }
  };

  checkHosts(addPeers());
  checkHosts(exclusivePeers());
  checkHosts(priorityPeers());
  checkHosts(seedPeers());

  return false;
}

CryptoNote::NetNodeConfig NodeOptions::getConfig(const std::string &dataDir,
                                                 const CryptoNote::Currency &currency) const {
  CryptoNote::NetNodeConfig config{};
  config.setAutoBlock(autoBan());
  config.setBlockDuration(std::chrono::minutes{banDuration()});
  if (!config.init(currency.network(), currency.networkUniqueName(), bind(), port(), externalPort(), localIp(),
                   hidePort(), dataDir, addPeers(), exclusivePeers(), priorityPeers(), seedPeers())) {
    exceptional<InvalidConfigurationError>();
  }
  return config;
}

}  // namespace App
}  // namespace Xi
