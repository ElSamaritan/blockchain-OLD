#pragma once

#include <chrono>
#include <limits>

#include <Xi/Utils/Conversion.h>

#include "config/Coin.h"

namespace CryptoNote {
namespace Config {
namespace Time {
constexpr std::chrono::seconds blockTime() { return 1_m; }
constexpr std::chrono::seconds minerRewardUnlockTime() { return 24_h; }

constexpr uint32_t minerRewardUnlockBlocksCount() {
  return static_cast<uint32_t>(
    std::chrono::seconds{minerRewardUnlockTime()}.count() / std::chrono::seconds{blockTime()}.count()
  );
}
constexpr uint32_t expectedBlocksPerDay(){
  return static_cast<uint32_t>(
    std::chrono::seconds{std::chrono::hours{24}}.count() / std::chrono::seconds{blockTime()}.count()
  );
}

constexpr uint16_t blockTimeSeconds() { return static_cast<uint16_t>(std::chrono::seconds{blockTime()}.count()); }

static_assert (std::chrono::seconds{blockTime()}.count() < std::numeric_limits<uint16_t>::max(),
               "Only a maximum of 2^16-1 seconds is allowed as block time.");
static_assert (blockTimeSeconds() > 0, "blockTime must be convert to positve none zero seconds.");
static_assert (minerRewardUnlockBlocksCount() > 0,
               "minerRewardUnlockBlocksCount must be convert to positve none zero amount of blocks.");
}  // namespace Time
}
}
