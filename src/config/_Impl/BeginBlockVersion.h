#pragma once

#include <cinttypes>

namespace CryptoNote {
namespace Config {
namespace BlockVersion {
template<uint8_t _Index> struct BlockVersionCheckpoint;
}
}
}

#define MakeBlockVersionCheckpoint(_Index, _Height, _Version, _IsFork) \
  namespace CryptoNote { namespace Config { namespace BlockVersion { \
  template <> struct BlockVersionCheckpoint<_Index> { \
    static inline constexpr uint8_t index() { return _Index; } \
    static inline constexpr uint32_t height() { return _Height; } \
    static inline constexpr uint8_t version() { return _Version; } \
    static inline constexpr bool isFork() { return _IsFork; } \
  }; } } }
