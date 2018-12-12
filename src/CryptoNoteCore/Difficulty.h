// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.
#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

namespace CryptoNote {
namespace Difficulty {

struct LWMA_3 {
  uint64_t operator()(const std::vector<uint64_t> &timestamps, const std::vector<uint64_t> &cumulativeDifficulties,
                      uint32_t windowSize, uint16_t blockTime) const;
};

struct LWMA_2 {
  uint64_t operator()(const std::vector<uint64_t> &timestamps, const std::vector<uint64_t> &cumulativeDifficulties,
                      uint32_t windowSize, uint16_t blockTime) const;
};

}  // namespace Difficulty
}  // namespace CryptoNote

template <typename T>
T clamp(const T &n, const T &lower, const T &upper) {
  return std::max(lower, std::min(n, upper));
}
