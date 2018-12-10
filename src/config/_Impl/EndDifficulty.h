#pragma once

#ifndef CURRENT_DIFFICULTY_CHECKPOINT_INDEX
#pragma error "CURRENT_DIFFICULTY_CHECKPOINT_INDEX must be defined."
#endif

#include <cinttypes>

#include "config/Time.h"

#undef MakeDifficultyCheckpoint

namespace CryptoNote {
namespace Config {
namespace Difficulty {

struct DifficultyCheckpointResolver {
  template<uint8_t _Index>
  static inline uint32_t windowSize(uint32_t height);

  template<uint8_t _Index>
  static inline uint64_t initialValue(uint32_t height);

  template<uint8_t _Index>
  static inline std::chrono::seconds timeLimit(uint32_t height);

  template<uint8_t _Index>
  static inline std::chrono::seconds maximumTimeLimit();

  template<uint8_t _Index>
  static inline uint64_t nextDifficulty(uint32_t height, const std::vector<uint64_t> &timestamps,
                                        const std::vector<uint64_t> &cumulativeDifficulties);
};

template<> inline uint32_t DifficultyCheckpointResolver::windowSize<0>(uint32_t) {
  return DifficultyCheckpoint<0>::windowSize();
}
template<uint8_t _Index> inline uint32_t DifficultyCheckpointResolver::windowSize(uint32_t height) {
  if(height >= DifficultyCheckpoint<_Index>::height())
    return DifficultyCheckpoint<_Index>::windowSize();
  else
    return windowSize<_Index - 1>(height);
}

template<> inline uint64_t DifficultyCheckpointResolver::initialValue<0>(uint32_t) {
  return DifficultyCheckpoint<0>::initialValue();
}
template<uint8_t _Index> inline uint64_t DifficultyCheckpointResolver::initialValue(uint32_t height) {
  if(height >= DifficultyCheckpoint<_Index>::height())
    return DifficultyCheckpoint<_Index>::initialValue();
  else
    return initialValue<_Index - 1>(height);
}

template<> inline std::chrono::seconds DifficultyCheckpointResolver::timeLimit<0>(uint32_t) {
  return DifficultyCheckpoint<0>::timeLimit();
}
template<uint8_t _Index> inline std::chrono::seconds DifficultyCheckpointResolver::timeLimit(uint32_t height) {
  if(height >= DifficultyCheckpoint<_Index>::height())
    return DifficultyCheckpoint<_Index>::timeLimit();
  else
    return timeLimit<_Index - 1>(height);
}

template<> inline std::chrono::seconds DifficultyCheckpointResolver::maximumTimeLimit<0>() {
  return DifficultyCheckpoint<0>::timeLimit();
}
template<uint8_t _Index> inline std::chrono::seconds DifficultyCheckpointResolver::maximumTimeLimit() {
  return std::chrono::seconds{DifficultyCheckpoint<_Index>::timeLimit().count(), maximumTimeLimit<_Index - 1>().count()};
}

template<>
inline uint64_t DifficultyCheckpointResolver::nextDifficulty<0>(uint32_t /* height */,
      const std::vector<uint64_t> &timestamps, const std::vector<uint64_t> &cumulativeDifficulties) {
  return DifficultyCheckpoint<0>::algorithm{}(
    timestamps, cumulativeDifficulties,
    DifficultyCheckpoint<0>::windowSize(), CryptoNote::Config::Time::blockTimeSeconds());
}
template<uint8_t _Index>
inline uint64_t DifficultyCheckpointResolver::nextDifficulty(uint32_t height,
      const std::vector<uint64_t> &timestamps, const std::vector<uint64_t> &cumulativeDifficulties) {
  if(height >= DifficultyCheckpoint<_Index>::height())
  {
    if(timestamps.size() < DifficultyCheckpoint<_Index>::windowSize()
       || cumulativeDifficulties.size() < DifficultyCheckpoint<_Index>::windowSize())
        return DifficultyCheckpoint<_Index>::defaultValue();
    else {
      typename DifficultyCheckpoint<_Index>::algorithm algo{};
      return algo(timestamps, cumulativeDifficulties, DifficultyCheckpoint<_Index>::windowSize(),
      CryptoNote::Config::Time::blockTimeSeconds());
    }
  }
  else
    return nextDifficulty<_Index - 1>(timestamps, cumulativeDifficulties);
}

inline uint32_t windowSize(uint32_t height) {
  return DifficultyCheckpointResolver::windowSize<CURRENT_DIFFICULTY_CHECKPOINT_INDEX>(height);
}

inline uint64_t initialValue(uint32_t height) {
  return DifficultyCheckpointResolver::windowSize<CURRENT_DIFFICULTY_CHECKPOINT_INDEX>(height);
}

inline std::chrono::seconds timeLimit(uint32_t height) {
  return DifficultyCheckpointResolver::timeLimit<CURRENT_DIFFICULTY_CHECKPOINT_INDEX>(height);
}

inline std::chrono::seconds maximumTimeLimit() {
  return DifficultyCheckpointResolver::maximumTimeLimit<CURRENT_DIFFICULTY_CHECKPOINT_INDEX>();
}

inline uint64_t nextDifficulty(uint32_t height, const std::vector<uint64_t> &timestamps,
                                const std::vector<uint64_t> &cumulativeDifficulties) {
  return DifficultyCheckpointResolver::nextDifficulty<CURRENT_DIFFICULTY_CHECKPOINT_INDEX>(
        height, timestamps, cumulativeDifficulties);
}

}
}
}

#undef CURRENT_DIFFICULTY_CHECKPOINT_INDEX
