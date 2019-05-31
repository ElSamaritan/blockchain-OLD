/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include <Xi/Types/Flag.h>
#include <Serialization/FlagSerialization.hpp>

namespace CryptoNote {

enum struct BlockFeatures : uint8_t {
  /// Serializes the static reward fields. If static rewards are enabled for the current major version this flag is
  /// mandatory.
  StaticReward = 1 << 0,

  /// Serializes upgrade version, enabling voting to upgrade to a new major version accepting a new consensus algorithm.
  UpgradeVoting = 1 << 1,

  /// Changes PoW evaluation based on potentially different block headers, encoded in an extra merge mining hashes
  /// field. The evaluation of the block header takes place by comparing an indexed merge mining hash to the provided
  /// block header hash.
  MergeMining = 1 << 2,
};

XI_MAKE_FLAG_OPERATIONS(BlockFeatures)
XI_SERIALIZATION_FLAG(BlockFeatures)

}  // namespace CryptoNote

XI_SERIALIZATION_FLAG_RANGE(CryptoNote::BlockFeatures, StaticReward, MergeMining)
XI_SERIALIZATION_FLAG_TAG(CryptoNote::BlockFeatures, StaticReward, "static_reward")
XI_SERIALIZATION_FLAG_TAG(CryptoNote::BlockFeatures, UpgradeVoting, "upgrade_voting")
XI_SERIALIZATION_FLAG_TAG(CryptoNote::BlockFeatures, MergeMining, "merge_mining")
