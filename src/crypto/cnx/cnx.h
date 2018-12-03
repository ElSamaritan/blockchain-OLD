#pragma once

#include "crypto/hash.h"

namespace Crypto {
namespace CNX {

struct Hash_v0 {
  static constexpr int variant() { return 1; }
  static constexpr uint32_t windowSize() { return 128; }

  static constexpr uint32_t minRandomizerSize() { return 1 << 5; }
  static constexpr uint32_t maxRandomizerSize() { return 1 << 15; }
  static constexpr uint32_t slopeRandomizerSize() { return (maxRandomizerSize() - minRandomizerSize()) / windowSize(); }

  static constexpr uint32_t minScratchpadSize() { return 1 * 1024 * 1024; }
  static constexpr uint32_t maxScratchpadSize() { return 2 * 1024 * 1024; }
  static constexpr uint32_t slopeScratchpadSize() { return (maxScratchpadSize() - minScratchpadSize()) / windowSize(); }

  void operator()(const void *data, size_t length, Hash &hash, uint32_t height) const;
};

}
}
