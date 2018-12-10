#pragma once

#include <vector>
#include <cinttypes>
#include <chrono>

#include "CryptoNoteCore/Difficulty.h"

namespace CryptoNote {
namespace Config {
namespace Difficulty {
template<uint8_t _Index> struct DifficultyCheckpoint;
}
}
}

#define MakeDifficultyCheckpoint(_Index, _Height, _Window, _Initial, _TimeLimit, _Algorithm) \
  namespace CryptoNote { namespace Config { namespace Difficulty { \
  template<> struct DifficultyCheckpoint<_Index> { \
    static inline constexpr uint8_t index() { return _Index; } \
    static inline constexpr uint32_t height() { return _Height; } \
    static inline constexpr uint32_t windowSize() { return _Window; } \
    static inline constexpr uint64_t initialValue() { return _Initial; } \
    static inline constexpr std::chrono::seconds timeLimit() { return _TimeLimit; } \
    using algorithm = _Algorithm; \
  }; } } }
