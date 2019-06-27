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

#ifndef CURRENT_DIFFICULTY_CHECKPOINT_INDEX
#pragma error "CURRENT_DIFFICULTY_CHECKPOINT_INDEX must be defined."
#endif

#include <cinttypes>

#include <Xi/Blockchain/Block/Version.hpp>

#include "Xi/Config/Time.h"

#undef MakeDifficultyCheckpoint

namespace Xi {
namespace Config {
namespace Difficulty {

struct DifficultyCheckpointResolver {
  template <Blockchain::Block::Version::value_type _Index>
  static inline uint32_t windowSize(Blockchain::Block::Version version);

  template <Blockchain::Block::Version::value_type _Index>
  static inline uint64_t initialValue(Blockchain::Block::Version version);

  template <Blockchain::Block::Version::value_type _Index>
  static inline uint64_t nextDifficulty(Blockchain::Block::Version version, const std::vector<uint64_t> &timestamps,
                                        const std::vector<uint64_t> &cumulativeDifficulties);
};

template <>
inline uint32_t DifficultyCheckpointResolver::windowSize<0>(Blockchain::Block::Version) {
  return DifficultyCheckpoint<0>::windowSize();
}
template <Blockchain::Block::Version::value_type _Index>
inline uint32_t DifficultyCheckpointResolver::windowSize(Blockchain::Block::Version version) {
  if (version >= DifficultyCheckpoint<_Index>::version())
    return DifficultyCheckpoint<_Index>::windowSize();
  else
    return windowSize<_Index - 1>(version);
}

template <>
inline uint64_t DifficultyCheckpointResolver::initialValue<0>(Blockchain::Block::Version) {
  return DifficultyCheckpoint<0>::initialValue();
}
template <Blockchain::Block::Version::value_type _Index>
inline uint64_t DifficultyCheckpointResolver::initialValue(Blockchain::Block::Version version) {
  if (version >= DifficultyCheckpoint<_Index>::version())
    return DifficultyCheckpoint<_Index>::initialValue();
  else
    return initialValue<_Index - 1>(version);
}

template <>
inline uint64_t DifficultyCheckpointResolver::nextDifficulty<0>(Blockchain::Block::Version /* version */,
                                                                const std::vector<uint64_t> &timestamps,
                                                                const std::vector<uint64_t> &cumulativeDifficulties) {
  if (timestamps.size() < DifficultyCheckpoint<0>::windowSize() + 1 ||
      cumulativeDifficulties.size() < DifficultyCheckpoint<0>::windowSize() + 1)
    return DifficultyCheckpoint<0>::initialValue();
  return DifficultyCheckpoint<0>::algorithm{}(timestamps, cumulativeDifficulties, DifficultyCheckpoint<0>::windowSize(),
                                              Xi::Config::Time::blockTimeSeconds());
}
template <Blockchain::Block::Version::value_type _Index>
inline uint64_t DifficultyCheckpointResolver::nextDifficulty(Blockchain::Block::Version version,
                                                             const std::vector<uint64_t> &timestamps,
                                                             const std::vector<uint64_t> &cumulativeDifficulties) {
  if (version >= DifficultyCheckpoint<_Index>::version()) {
    if (timestamps.size() < DifficultyCheckpoint<_Index>::windowSize() + 1 ||
        cumulativeDifficulties.size() < DifficultyCheckpoint<_Index>::windowSize() + 1)
      return DifficultyCheckpoint<_Index>::initialValue();
    else {
      typename DifficultyCheckpoint<_Index>::algorithm algo{};
      return algo(timestamps, cumulativeDifficulties, DifficultyCheckpoint<_Index>::windowSize(),
                  Xi::Config::Time::blockTimeSeconds());
    }
  } else
    return nextDifficulty<_Index - 1>(timestamps, cumulativeDifficulties);
}

inline uint32_t windowSize(Blockchain::Block::Version version) {
  return DifficultyCheckpointResolver::windowSize<CURRENT_DIFFICULTY_CHECKPOINT_INDEX>(version);
}

inline uint64_t initialValue(Blockchain::Block::Version version) {
  return DifficultyCheckpointResolver::windowSize<CURRENT_DIFFICULTY_CHECKPOINT_INDEX>(version);
}

inline uint64_t nextDifficulty(Blockchain::Block::Version version, const std::vector<uint64_t> &timestamps,
                               const std::vector<uint64_t> &cumulativeDifficulties) {
  return DifficultyCheckpointResolver::nextDifficulty<CURRENT_DIFFICULTY_CHECKPOINT_INDEX>(version, timestamps,
                                                                                           cumulativeDifficulties);
}

}  // namespace Difficulty
}  // namespace Config
}  // namespace Xi

#undef CURRENT_DIFFICULTY_CHECKPOINT_INDEX
