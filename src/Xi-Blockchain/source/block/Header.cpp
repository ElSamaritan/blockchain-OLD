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

#include <Xi/Exceptions.hpp>
#include <Common/VectorOutputStream.h>
#include <Serialization/BinaryOutputStreamSerializer.h>

bool Xi::Blockchain::Block::Header::serialize(CryptoNote::ISerializer &serializer) {
  return serialize(serializer, false);
}

Crypto::Hash Xi::Blockchain::Block::Header::headerHash() const {
  return ::Crypto::Hash::computeObjectHash(*this).takeOrThrow();
}

Crypto::Hash Xi::Blockchain::Block::Header::proofOfWorkPrefix() const {
  ByteVector buffer{};
  buffer.reserve(64);

  {
    Common::VectorOutputStream stream{buffer};
    CryptoNote::BinaryOutputStreamSerializer serializer{stream};
    if (!const_cast<Header *>(this)->serialize(serializer, true)) {
      exceptional<RuntimeError>("unable to compute hash due to serialization failure");
    }
  }

  return ::Crypto::Hash::compute(buffer).takeOrThrow();
}

bool Xi::Blockchain::Block::Header::serialize(CryptoNote::ISerializer &serializer, bool isPoWPrefix) {
  XI_RETURN_EC_IF_NOT(serializer(version, "version"), false);
  XI_RETURN_EC_IF_NOT(serializer(upgradeVote, "upgrade_vote"), false);
  if (!isPoWPrefix) {
    XI_RETURN_EC_IF_NOT(serializer(nonce, "nonce"), false);
  }
  XI_RETURN_EC_IF_NOT(serializer(timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializer(previousBlockHash, "previous_block_hash"), false);
  return true;
}
