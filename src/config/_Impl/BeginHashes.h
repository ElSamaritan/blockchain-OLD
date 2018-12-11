#pragma once

#include <vector>
#include <cinttypes>
#include <chrono>

#include "CryptoNoteCore/Difficulty.h"

namespace CryptoNote {
namespace Config {
namespace Hashes {
template <uint8_t _Index>
struct HashCheckpoint;
}
}  // namespace Config
}  // namespace CryptoNote

#define MakeHashCheckpoint(_Index, _Version, _Algorithm) \
  namespace CryptoNote {                                                                      \
  namespace Config {                                                                          \
  namespace Hashes {                                                                          \
  template <>                                                                                 \
  struct HashCheckpoint<_Index> {                                                             \
    static inline constexpr uint8_t index() { return _Index; }                                  \
    static inline constexpr uint8_t version() { return _Version; }                              \
    using algorithm = _Algorithm;                                                             \
  };                                                                                          \
  }                                                                                           \
  }                                                                                           \
  }
