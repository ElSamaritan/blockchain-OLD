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

#include <Xi/TypeSafe/Flag.hpp>
#include <Serialization/FlagSerialization.hpp>

namespace Xi {
namespace Blockchain {
namespace Block {

enum Feature : uint16_t {
  None = 0,
  UpgradeVote = 1 << 0,
  BlockReward = 1 << 1,
  StaticReward = 1 << 2,
  Transfers = 1 << 3,
  Fusions = 1 << 4,
};

XI_TYPESAFE_FLAG_MAKE_OPERATIONS(Feature)
XI_SERIALIZATION_FLAG(Feature)

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_FLAG_RANGE(Xi::Blockchain::Block::Feature, BlockReward, Fusions)
XI_SERIALIZATION_FLAG_TAG(Xi::Blockchain::Block::Feature, BlockReward, "block_reward")
XI_SERIALIZATION_FLAG_TAG(Xi::Blockchain::Block::Feature, StaticReward, "static_reward")
XI_SERIALIZATION_FLAG_TAG(Xi::Blockchain::Block::Feature, Transfers, "transfers")
XI_SERIALIZATION_FLAG_TAG(Xi::Blockchain::Block::Feature, Fusions, "fusions")
