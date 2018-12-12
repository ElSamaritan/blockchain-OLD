#pragma once

#include <cinttypes>

#include "config/BlockVersion.h"

namespace CryptoNote {
namespace Config {
namespace Dust {
template <uint8_t _Index>
struct DustCheckpoint;
template <uint8_t _Index>
struct FusionDustCheckpoint;
}  // namespace Dust
}  // namespace Config
}  // namespace CryptoNote

#define MakeDustCheckpoint(_Index, _Version, _Dust)                     \
  namespace CryptoNote {                                                \
  namespace Config {                                                    \
  namespace Dust {                                                      \
  template <>                                                           \
  struct DustCheckpoint<_Index> {                                       \
    static inline constexpr uint8_t index() { return _Index; }          \
    static inline constexpr uint8_t version() { return _Version; }      \
    static inline constexpr uint64_t dust() { return _Dust; }           \
    static_assert(::CryptoNote::Config::BlockVersion::exists(_Version), \
                  "Non existing major block version referenced.");      \
  };                                                                    \
  }                                                                     \
  }                                                                     \
  }

#define MakeFusionDustCheckpoint(_Index, _Version, _Dust)               \
  namespace CryptoNote {                                                \
  namespace Config {                                                    \
  namespace Dust {                                                      \
  template <>                                                           \
  struct FusionDustCheckpoint<_Index> {                                 \
    static inline constexpr uint8_t index() { return _Index; }          \
    static inline constexpr uint8_t version() { return _Version; }      \
    static inline constexpr uint64_t dust() { return _Dust; }           \
    static_assert(::CryptoNote::Config::BlockVersion::exists(_Version), \
                  "Non existing major block version referenced.");      \
  };                                                                    \
  }                                                                     \
  }                                                                     \
  }
