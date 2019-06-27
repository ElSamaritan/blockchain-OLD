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
#include <variant>

#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/OptionalSerialization.hpp>
#include <Serialization/VariantSerialization.hpp>
#include <CryptoNoteCore/CryptoNoteSerialization.h>
#include <CryptoNoteCore/CryptoNote.h>

#include "Xi/Blockchain/Block/Block.hpp"
#include "Xi/Blockchain/Explorer/Data/Data.hpp"
#include "Xi/Blockchain/Services/BlockExplorer/Limits.hpp"

namespace Xi {
namespace Blockchain {
namespace Services {
namespace BlockExplorer {

// ============================================================================================================ Blocks
struct BlockHeightRange {
  Block::Height start;
  Block::Height end;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(start)
  KV_MEMBER(end)
  KV_END_SERIALIZATION
};

struct BlockHeightSpan {
  std::optional<Block::Height> start;
  Block::Offset count;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(start)
  KV_MEMBER(count)
  KV_END_SERIALIZATION
};

// clang-format off
XI_SERIALIZATION_VARIANT_INVARIANT(
    GetBlockRequest,
        Block::Height,
        Block::Hash
)

XI_SERIALIZATION_VARIANT_INVARIANT(
    GetBlocksRequest,
        Block::HashVector,
        Block::HeightVector,
        BlockHeightRange,
        BlockHeightSpan
)

template<typename _InfoT>
using GetBlocksResponse = std::vector<std::optional<_InfoT>>;

// ====================================================================================================== Transactions
using GetTransactionRequest = Block::Hash;
using GetTransactionsRequest = std::vector<Block::Hash>;

template<typename _InfoT>
using GetTransactionsResponse = std::vector<std::optional<_InfoT>>;

// ============================================================================================================ Serach
using SearchRequest = std::string;

XI_SERIALIZATION_VARIANT_INVARIANT(
    SearchResponse,
        Explorer::ShortBlockInfo,
        Explorer::ShortTransactionInfo
)

// clang-format on

}  // namespace BlockExplorer
}  // namespace Services
}  // namespace Blockchain
}  // namespace Xi

// clang-format off
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Services::BlockExplorer::GetBlockRequest, 0, 0x0001, "height")
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Services::BlockExplorer::GetBlockRequest, 1, 0x0002, "hash")

XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Services::BlockExplorer::GetBlocksRequest, 0, 0x0001, "hash")
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Services::BlockExplorer::GetBlocksRequest, 1, 0x0002, "height")
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Services::BlockExplorer::GetBlocksRequest, 2, 0x0003, "height_range")
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Services::BlockExplorer::GetBlocksRequest, 3, 0x0004, "height_span")

XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Services::BlockExplorer::SearchResponse, 0, 0x0001, "block")
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Services::BlockExplorer::SearchResponse, 1, 0x0002, "transaction")
// clang-format on
