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

#include <Serialization/ISerializer.h>
#include <Serialization/OptionalSerialization.hpp>
#include <Serialization/SerializationOverloads.h>
#include <CryptoNoteCore/CryptoNote.h>

#include "Xi/Blockchain/Explorer/Data/ShortTransactionInfo.hpp"
#include "Xi/Blockchain/Explorer/Data/ShortBlockInfo.hpp"

namespace Xi {
namespace Blockchain {
namespace Explorer {

struct BlockInfo : ShortBlockInfo {
  Block::Version version;
  Block::Version upgrade_vote;
  /// Nonce used to fullfill the proof of work requirement
  Block::Nonce nonce;
  /// The previous block hash this block is based on.
  Block::Hash previous_hash;

  /// Binary size of this block, everything included.
  uint64_t blob_size;
  /// Number of transactions included in this block (Including miner reward and static reward)
  uint64_t transactions_count;

  /// Transaction used to generate coins for the miner, also refered as coinbase transaction
  ShortTransactionInfo miner_reward;
  /// If static rewards are enable, this holds the static reward transaction info
  std::optional<ShortTransactionInfo> static_reward;

  /// All coin transfer embedded in this block. (no coinbase or static reward transaction)
  std::vector<ShortTransactionInfo> transfers;
  /// Sum of all fees from transfers.
  uint64_t sum_fees;

  KV_BEGIN_SERIALIZATION
  KV_BASE(ShortBlockInfo)

  KV_MEMBER(version)
  KV_MEMBER(upgrade_vote)
  KV_MEMBER(nonce)
  KV_MEMBER(previous_hash)

  KV_MEMBER(blob_size)
  KV_MEMBER(transactions_count)

  KV_MEMBER(miner_reward)
  KV_MEMBER(static_reward)

  KV_MEMBER(transfers)
  KV_MEMBER(sum_fees)

  KV_END_SERIALIZATION
};

using BlockInfoVector = std::vector<BlockInfo>;

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi
