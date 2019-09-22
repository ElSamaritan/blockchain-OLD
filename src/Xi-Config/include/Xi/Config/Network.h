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
#include <vector>
#include <stdexcept>
#include <optional>

#include <Xi/ExternalIncludePush.h>
#include <boost/uuid/uuid.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/OptionalSerialization.hpp>

#include "Xi/Config/NetworkType.h"

namespace Xi {
namespace Config {
namespace Network {

class Configuration {
 public:
  XI_PROPERTY(Type, type)
  XI_PROPERTY(boost::uuids::uuid, identifier)
  XI_PROPERTY(std::vector<std::string>, seeds)
  XI_PROPERTY(std::string, seedsUrl)
  XI_PROPERTY(std::string, peersUrl)
  XI_PROPERTY(std::string, nodesUrl)

  XI_PROPERTY(uint16_t, p2pPort)
  static uint16_t p2pDefaultPort();

  XI_PROPERTY(uint16_t, rpcPort)
  static uint16_t rpcDefaultPort();

  XI_PROPERTY(uint16_t, pgservicePort)
  static inline constexpr uint16_t pgserviceDefaultPort() {
    return 38070;
  }

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(type(), type)
  XI_RETURN_EC_IF_NOT(s.binary(identifier().data, identifier().size(), "identifier"), false);
  KV_MEMBER_RENAME(seeds(), seeds)
  KV_MEMBER_RENAME(seedsUrl(), seeds_url)
  KV_MEMBER_RENAME(peersUrl(), peers_url)
  KV_MEMBER_RENAME(nodesUrl(), nodes_url)

  KV_MEMBER_RENAME(p2pPort(), p2p_port)
  KV_MEMBER_RENAME(rpcPort(), rpc_port)
  KV_MEMBER_RENAME(pgservicePort(), pgservice_port)
  KV_END_SERIALIZATION

  bool isMainNet() const {
    return this->type() == Type::MainNet;
  }
};

Type defaultNetworkType();

/// Resource path to the default embedded chain configuration.
std::string defaultNetwork();

/// Resource path to embedded network checkpoints.
std::string checkpoints(const Type network);

std::string breakpadServer();

static inline constexpr uint64_t blockIdentifiersSynchronizationBatchSize() {
  return 1000;
}
static inline constexpr uint64_t blocksSynchronizationBatchSize() {
  return 500;
}

static inline constexpr uint64_t blocksP2pSynchronizationMaxBatchSize() {
  return 1000;
}

static inline constexpr uint64_t blocksP2pSynchronizationMaxBlobSize() {
  return 16 * 1024 * 1024;
}
}  // namespace Network
}  // namespace Config
}  // namespace Xi
