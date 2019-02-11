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

#ifndef CURRENT_TIME_CHECKPOINT_INDEX
#pragma error "CURRENT_TIME_CHECKPOINT_INDEX must be defined."
#endif

#include <cinttypes>

#include "Xi/Config/Time.h"

#undef MakeDifficultyCheckpoint

namespace Xi {
namespace Config {
namespace Time {

struct TimeCheckpointResolver {
  template <uint8_t _Index>
  static inline uint32_t pastWindowSize(uint8_t version);

  template <uint8_t _Index>
  static inline std::chrono::seconds futureTimeLimit(uint8_t version);
};

template <>
inline uint32_t TimeCheckpointResolver::pastWindowSize<0>(uint8_t) {
  return TimeCheckpoint<0>::pastWindowSize();
}
template <uint8_t _Index>
inline uint32_t TimeCheckpointResolver::pastWindowSize(uint8_t version) {
  if (version >= TimeCheckpoint<_Index>::version())
    return TimeCheckpoint<_Index>::pastWindowSize();
  else
    return pastWindowSize<_Index - 1>(version);
}

template <>
inline std::chrono::seconds TimeCheckpointResolver::futureTimeLimit<0>(uint8_t) {
  return TimeCheckpoint<0>::futureTimeLimit();
}
template <uint8_t _Index>
inline std::chrono::seconds TimeCheckpointResolver::futureTimeLimit(uint8_t version) {
  if (version >= TimeCheckpoint<_Index>::version())
    return TimeCheckpoint<_Index>::futureTimeLimit();
  else
    return futureTimeLimit<_Index - 1>(version);
}

inline uint32_t pastWindowSize(uint8_t version) {
  return TimeCheckpointResolver::pastWindowSize<CURRENT_TIME_CHECKPOINT_INDEX>(version);
}

inline std::chrono::seconds futureTimeLimit(uint8_t version) {
  return TimeCheckpointResolver::futureTimeLimit<CURRENT_TIME_CHECKPOINT_INDEX>(version);
}

}  // namespace Time
}  // namespace Config
}  // namespace Xi

#undef CURRENT_TIME_CHECKPOINT_INDEX
