#pragma once

#include <cinttypes>

namespace CryptoNote {
namespace Config {
namespace Mixin {
template <uint8_t _Index>
struct MixinCheckpoint;
}
}  // namespace Config
}  // namespace CryptoNote

#define MakeMixinCheckpoint(_Index, _Version, _Min, _Max, _Default)      \
  namespace CryptoNote {                                                 \
  namespace Config {                                                     \
  namespace Mixin {                                                      \
  template <>                                                            \
  struct MixinCheckpoint<_Index> {                                       \
    static inline constexpr uint8_t index() { return _Index; }           \
    static inline constexpr uint8_t version() { return _Version; }       \
    static inline constexpr uint16_t minimum() { return _Min; }          \
    static inline constexpr uint16_t maximum() { return _Max; }          \
    static inline constexpr uint16_t defaultValue() { return _Default; } \
  };                                                                     \
  }                                                                      \
  }                                                                      \
  }
