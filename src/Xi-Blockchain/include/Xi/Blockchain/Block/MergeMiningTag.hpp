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

#include <Xi/Global.hh>
#include <crypto/Types/Hash.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/VariantSerialization.hpp>

namespace Xi {
namespace Blockchain {
namespace Block {

struct NoMergeMiningTag {
  KV_BEGIN_SERIALIZATION
  XI_UNUSED(s)
  KV_END_SERIALIZATION
};

struct MergeMiningTag {
  ::Crypto::HashVector prefix;
  ::Crypto::HashVector postfix;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(prefix)
  KV_MEMBER(postfix)
  KV_END_SERIALIZATION

  uint64_t size() const;
  uint64_t binarySize() const;
};

struct PrunedMergeMiningTag {
  ::Crypto::Hash proofOfWorkPrefix;
  uint64_t binarySize;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(proofOfWorkPrefix, proof_of_work_prefix)
  KV_MEMBER_RENAME(binarySize, binary_size)
  KV_END_SERIALIZATION
};

using PrunableMergeMiningTag = std::variant<NoMergeMiningTag, MergeMiningTag, PrunedMergeMiningTag>;

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Block::PrunableMergeMiningTag, 0, 0x0001, "none")
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Block::PrunableMergeMiningTag, 1, 0x0002, "raw")
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Block::PrunableMergeMiningTag, 2, 0x0003, "pruned")
