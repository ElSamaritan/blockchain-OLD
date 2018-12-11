#pragma once

#include <cinttypes>

namespace CryptoNote {
namespace Config {
namespace Reward {
template<uint8_t _Index> struct RewardCheckpoint;
}
}
}

#define MakeRewardCheckpoint(_Index, _Version, _Window, _Zone) \
  namespace CryptoNote { namespace Config { namespace Reward { \
  template <> struct RewardCheckpoint<_Index> { \
    static inline constexpr uint8_t index() { return _Index; } \
    static inline constexpr uint8_t version() { return _Version; } \
    static inline constexpr uint32_t window() { return _Window; } \
    static inline constexpr uint64_t fullRewardZone() { return _Zone; } \
  }; } } }
