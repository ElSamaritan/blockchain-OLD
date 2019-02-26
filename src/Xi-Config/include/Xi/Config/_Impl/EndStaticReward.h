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

#include "Xi/Config/StaticReward.h"

#undef MakeStaticRewardCheckpoint

#ifndef CURRENT_STATIC_REWARD_CHECKPOINT_INDEX
#pragma error "CURRENT_STATIC_REWARD_CHECKPOINT_INDEX must be defines"
#endif

namespace Xi {
namespace Config {
namespace StaticReward {

struct StaticRewardCheckpointResolver {
  template <uint8_t>
  static inline uint64_t amount(uint8_t version);

  template <uint8_t>
  static inline std::string address(uint8_t version);
};

template <>
inline uint64_t StaticRewardCheckpointResolver::amount<0>(uint8_t) {
  return StaticRewardCheckpoint<0>::amount();
}
template <uint8_t _Index>
inline uint64_t StaticRewardCheckpointResolver::amount(uint8_t version) {
  if (version >= StaticRewardCheckpoint<_Index>::version())
    return StaticRewardCheckpoint<_Index>::amount();
  else
    return amount<_Index - 1>(version);
}

template <>
inline std::string StaticRewardCheckpointResolver::address<0>(uint8_t) {
  return StaticRewardCheckpoint<0>::address();
}
template <uint8_t _Index>
inline std::string StaticRewardCheckpointResolver::address(uint8_t version) {
  if (version >= StaticRewardCheckpoint<_Index>::version())
    return StaticRewardCheckpoint<_Index>::address();
  else
    return address<_Index - 1>(version);
}

inline uint64_t amount(uint8_t version) {
  return StaticRewardCheckpointResolver::amount<CURRENT_STATIC_REWARD_CHECKPOINT_INDEX>(version);
}

inline std::string address(uint8_t version) {
  return StaticRewardCheckpointResolver::address<CURRENT_STATIC_REWARD_CHECKPOINT_INDEX>(version);
}

inline bool isEnabled(uint8_t version) { return !address(version).empty() && amount(version) > 0; }

}  // namespace StaticReward
}  // namespace Config
}  // namespace Xi

#undef CURRENT_STATIC_REWARD_CHECKPOINT_INDEX
