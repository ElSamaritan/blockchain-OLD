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

#include <Xi/Span.hpp>
#include <Serialization/ISerializer.h>
#include <Serialization/EnumSerialization.hpp>
#include <CryptoNoteCore/CryptoNote.h>

namespace Xi {
namespace Blockchain {
namespace Explorer {

using TransactionHash = CryptoNote::TransactionHash;
using TransactionHashVector = std::vector<CryptoNote::TransactionHash>;
XI_DECLARE_SPANS(TransactionHash)

enum struct TransactionType {
  BlockReward = 1,
  StaticReweard,
  Transfer,
  Fusion,
};

XI_SERIALIZATION_ENUM(TransactionType)

enum struct TransactionContainer {
  MainChain = 1,
  AlternativeChain,
  Pool,
};

XI_SERIALIZATION_ENUM(TransactionContainer)

struct ShortTransactionInfo {
  TransactionType type;
  TransactionContainer container;
  TransactionHash hash;

  uint64_t fee;
  uint64_t sum_input;
  uint64_t sum_output;
  uint64_t unlock_time;

  uint64_t blob_size;

  KV_BEGIN_SERIALIZATION

  KV_MEMBER(type)
  KV_MEMBER(container)
  KV_MEMBER(hash)

  KV_MEMBER(fee)
  KV_MEMBER(sum_input)
  KV_MEMBER(sum_output)

  KV_MEMBER(blob_size)

  KV_END_SERIALIZATION
};

using ShortTransactionInfoVector = std::vector<ShortTransactionInfo>;

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_ENUM_RANGE(Xi::Blockchain::Explorer::TransactionType, BlockReward, Fusion)
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::TransactionType, BlockReward, "block_reward")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::TransactionType, StaticReweard, "static_reward")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::TransactionType, Transfer, "transfer")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::TransactionType, Fusion, "fusion")

XI_SERIALIZATION_ENUM_RANGE(Xi::Blockchain::Explorer::TransactionContainer, MainChain, Pool)
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::TransactionContainer, MainChain, "main_chain")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::TransactionContainer, AlternativeChain, "alternative_chain")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Explorer::TransactionContainer, Pool, "pool")
