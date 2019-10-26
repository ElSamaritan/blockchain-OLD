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
#include <string>

#include <Serialization/ISerializer.h>
#include <Serialization/EnumSerialization.hpp>
#include <Serialization/OptionalSerialization.hpp>
#include <Xi/Config/NetworkType.h>
#include <Xi/Blockchain/Block/Version.hpp>
#include <Xi/Blockchain/Block/Height.hpp>
#include <Xi/Blockchain/Block/Hash.hpp>

namespace Xi {
namespace Blockchain {
namespace Explorer {

enum struct SyncStatus {
  Synced = 0x0001,
  Syncing = 0x0002,
  Detached = 0x0003,
  Disconnected = 0x0004,
};

XI_SERIALIZATION_ENUM(SyncStatus)

enum struct NodeType {
  Full = 0x0001,
  Light = 0x0002,
};

XI_SERIALIZATION_ENUM(NodeType)

struct NodeStatus {
  struct {
    struct {
      uint16_t major;
      uint16_t minor;
      uint16_t review;

      KV_BEGIN_SERIALIZATION
      KV_MEMBER(major)
      KV_MEMBER(minor)
      KV_MEMBER(review)
      KV_END_SERIALIZATION
    } version;

    SyncStatus sync;
    NodeType type;
    uint64_t uptime;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(version)
    KV_MEMBER(sync)
    KV_MEMBER(type)
    KV_MEMBER(uptime)
    KV_END_SERIALIZATION
  } node;

  struct {
    Block::Hash top_hash;
    Block::Height top_height;
    Block::Version top_version;
    Config::Network::Type network;
    uint32_t hashrate;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(top_hash)
    KV_MEMBER(top_height)
    KV_MEMBER(top_version)
    KV_MEMBER(hashrate)
    KV_MEMBER(network)
    KV_END_SERIALIZATION
  } chain;

  struct {
    uint32_t outgoing_count;
    uint32_t incoming_count;
    std::optional<Block::Height> height;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(outgoing_count)
    KV_MEMBER(incoming_count)
    KV_MEMBER(height)
    KV_END_SERIALIZATION
  } p2p;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(node)
  KV_MEMBER(chain)
  KV_MEMBER(p2p)
  KV_END_SERIALIZATION
};

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_ENUM_RANGE(Xi::Blockchain::Explorer::SyncStatus, Synced, Disconnected)
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::SyncStatus, Synced, "synced")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::SyncStatus, Syncing, "syncing")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::SyncStatus, Detached, "detached")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::SyncStatus, Disconnected, "disconnected")

XI_SERIALIZATION_ENUM_RANGE(Xi::Blockchain::Explorer::NodeType, Full, Light)
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::NodeType, Full, "full")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::NodeType, Light, "light")
