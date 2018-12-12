#pragma once

#include <vector>
#include <cinttypes>
#include <chrono>

#include "CryptoNoteCore/Difficulty.h"
#include "config/BlockVersion.h"

namespace CryptoNote {
namespace Config {
namespace Difficulty {
template <uint8_t _Index>
struct DifficultyCheckpoint;
}
}  // namespace Config
}  // namespace CryptoNote

#define MakeDifficultyCheckpoint(_Index, _Version, _Window, _Initial, _TimeLimit, _Algorithm) \
  namespace CryptoNote {                                                                      \
  namespace Config {                                                                          \
  namespace Difficulty {                                                                      \
  template <>                                                                                 \
  struct DifficultyCheckpoint<_Index> {                                                       \
    static inline constexpr uint8_t index() { return _Index; }                                \
    static inline constexpr uint8_t version() { return _Version; }                            \
    static inline constexpr uint32_t windowSize() { return _Window; }                         \
    static inline constexpr uint64_t initialValue() { return _Initial; }                      \
    static inline constexpr std::chrono::seconds timeLimit() { return _TimeLimit; }           \
    using algorithm = _Algorithm;                                                             \
    static_assert(::CryptoNote::Config::BlockVersion::exists(_Version),                       \
                  "Non existing major block version referenced.");                            \
  };                                                                                          \
  }                                                                                           \
  }                                                                                           \
  }
