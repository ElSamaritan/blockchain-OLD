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
#include <vector>

#include <Xi/Blockchain/Block/Block.hpp>
#include <Serialization/ISerializer.h>
#include <Serialization/EnumSerialization.hpp>
#include <CryptoNoteCore/CryptoNote.h>

namespace Xi {
namespace Blockchain {
namespace Explorer {

enum struct BlockSource {
  MainChain = 1,
  AlternativeChain,
};
XI_SERIALIZATION_ENUM(BlockSource)

struct ShortBlockInfo {
  /// The verification hash of this block (Not proof of work hash)
  Block::Hash hash;
  /// Number of previous blocks generated + 1
  Block::Height height;
  /// Unix timestamp
  uint64_t timestamp;
  /// The chain source type
  BlockSource chain;

  /// cumulative proof of work based on this block and all prefious blocks
  uint64_t cumulative_difficulty;
  /// size of this block + size of all previous blocks
  uint64_t cumulative_size;
  /// sum of all coins generated so far - sum of coins burned
  uint64_t cumulative_supply;
  /// transactions count + transactions count of all previous blocks
  uint64_t cumulative_transactions_count;

  KV_BEGIN_SERIALIZATION

  KV_MEMBER(hash)
  KV_MEMBER(height)
  KV_MEMBER(timestamp)
  KV_MEMBER(chain)

  KV_MEMBER(cumulative_difficulty)
  KV_MEMBER(cumulative_size)
  KV_MEMBER(cumulative_supply)
  KV_MEMBER(cumulative_transactions_count)

  KV_END_SERIALIZATION
};

using ShortBlockInfoVector = std::vector<ShortBlockInfo>;

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_ENUM_RANGE(Xi::Blockchain::Explorer::BlockSource, MainChain, AlternativeChain)
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::BlockSource, MainChain, "main_chain")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::BlockSource, AlternativeChain, "alternative_chain")
