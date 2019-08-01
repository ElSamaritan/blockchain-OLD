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

#include "Xi/Blockchain/Block/Header.hpp"

#include <algorithm>

#include <Xi/Exceptions.hpp>
#include <Common/VectorOutputStream.h>
#include <Serialization/BinaryOutputStreamSerializer.h>
#include <Serialization/OptionalSerialization.hpp>

namespace Xi {
namespace Blockchain {
namespace Block {

bool Header::serialize(CryptoNote::ISerializer &serializer) {
  return serialize(serializer, false);
}

Hash Header::headerHash() const {
  return Hash::computeObjectHash(*this).takeOrThrow();
}

Hash Header::proofOfWorkPrefix() const {
  ByteVector buffer{};
  buffer.reserve(64);

  {
    Common::VectorOutputStream stream{buffer};
    CryptoNote::BinaryOutputStreamSerializer serializer{stream};
    if (!const_cast<Header *>(this)->serialize(serializer, true)) {
      exceptional<RuntimeError>("unable to compute hash due to serialization failure");
    }
  }

  return Hash::compute(buffer).takeOrThrow();
}

Hash Header::proofOfWorkHash(const Hash &transactionTreeHash) const {
  std::array<Hash, 2> hashes{{proofOfWorkPrefix(), transactionTreeHash}};
  return Hash::computeMerkleTree(hashes).takeOrThrow();
}

bool Header::serialize(CryptoNote::ISerializer &serializer, bool isPoWPrefix) {
  XI_RETURN_EC_IF_NOT(serializer(features, "features"), false);
  XI_RETURN_EC_IF_NOT(serializer(version, "version"), false);
  XI_RETURN_EC_IF_NOT(serializer(upgradeVote, "upgrade_vote"), false);
  if (!isPoWPrefix) {
    XI_RETURN_EC_IF_NOT(serializer(nonce, "nonce"), false);
  }
  XI_RETURN_EC_IF_NOT(serializer(timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializer(previousBlockHash, "previous_block_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(staticRewardHash, "static_reward_hash"), false);
  if (!isPoWPrefix) {
    XI_RETURN_EC_IF_NOT(serializer(mergeMiningTag, "merge_mining_tag"), false);
  }
  return true;
}

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi
