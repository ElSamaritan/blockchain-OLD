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

#include "Xi/Config/MinerReward.h"

#undef MakeRewardCheckpoint

#ifndef CURRENT_REWARD_CHECKPOINT_INDEX
#pragma error "CURRENT_REWARD_CHECKPOINT_INDEX must be defines"
#endif

namespace Xi {
namespace Config {
namespace MinerReward {

struct RewardCheckpointResolver {
  template <uint8_t>
  static inline uint32_t window(uint8_t version);

  template <uint8_t>
  static inline uint64_t fullRewardZone(uint8_t version);

  template <uint8_t>
  static inline uint64_t cutOff(uint8_t version);
};

template <>
inline uint32_t RewardCheckpointResolver::window<0>(uint8_t) {
  return RewardCheckpoint<0>::window();
}
template <uint8_t _Index>
inline uint32_t RewardCheckpointResolver::window(uint8_t version) {
  if (version >= RewardCheckpoint<_Index>::version())
    return RewardCheckpoint<_Index>::window();
  else
    return window<_Index - 1>(version);
}

template <>
inline uint64_t RewardCheckpointResolver::fullRewardZone<0>(uint8_t) {
  return RewardCheckpoint<0>::fullRewardZone();
}
template <uint8_t _Index>
inline uint64_t RewardCheckpointResolver::fullRewardZone(uint8_t version) {
  if (version >= RewardCheckpoint<_Index>::version())
    return RewardCheckpoint<_Index>::fullRewardZone();
  else
    return fullRewardZone<_Index - 1>(version);
}

template <>
inline uint64_t RewardCheckpointResolver::cutOff<0>(uint8_t) {
  return RewardCheckpoint<0>::cutOff();
}
template <uint8_t _Index>
inline uint64_t RewardCheckpointResolver::cutOff(uint8_t version) {
  if (version >= RewardCheckpoint<_Index>::version())
    return RewardCheckpoint<_Index>::cutOff();
  else
    return cutOff<_Index - 1>(version);
}

inline uint32_t window(uint8_t version) {
  return RewardCheckpointResolver::window<CURRENT_REWARD_CHECKPOINT_INDEX>(version);
}

inline uint64_t fullRewardZone(uint8_t version) {
  return RewardCheckpointResolver::fullRewardZone<CURRENT_REWARD_CHECKPOINT_INDEX>(version);
}

inline uint64_t cutOff(uint8_t version) {
  return RewardCheckpointResolver::cutOff<CURRENT_REWARD_CHECKPOINT_INDEX>(version);
}

}  // namespace MinerReward
}  // namespace Config
}  // namespace Xi

#undef CURRENT_REWARD_CHECKPOINT_INDEX
