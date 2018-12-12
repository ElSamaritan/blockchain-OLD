#pragma once

#include <cinttypes>

#include "config/Reward.h"

#undef MakeRewardCheckpoint

#ifndef CURRENT_REWARD_CHECKPOINT_INDEX
#pragma error "CURRENT_REWARD_CHECKPOINT_INDEX must be defines"
#endif

namespace CryptoNote {
namespace Config {
namespace Reward {

struct RewardCheckpointResolver {
  template<uint8_t>
  static inline uint32_t window(uint8_t version);

  template<uint8_t>
  static inline uint64_t fullRewardZone(uint8_t version);
};

template<> inline uint32_t RewardCheckpointResolver::window<0>(uint8_t) {
  return RewardCheckpoint<0>::window();
}
template<uint8_t _Index> inline uint32_t RewardCheckpointResolver::window(uint8_t version) {
  if(version >= RewardCheckpoint<_Index>::version())
    return RewardCheckpoint<_Index>::window();
  else
    return window<_Index - 1>(version);
}

template<> inline uint64_t RewardCheckpointResolver::fullRewardZone<0>(uint8_t) {
  return RewardCheckpoint<0>::fullRewardZone();
}
template<uint8_t _Index> inline uint64_t RewardCheckpointResolver::fullRewardZone(uint8_t version) {
  if(version >= RewardCheckpoint<_Index>::version())
    return RewardCheckpoint<_Index>::fullRewardZone();
  else
    return fullRewardZone<_Index - 1>(version);
}

inline uint32_t window(uint8_t version) {
  return RewardCheckpointResolver::window<CURRENT_REWARD_CHECKPOINT_INDEX>(version);
}

inline uint64_t fullRewardZone(uint8_t version) {
  return RewardCheckpointResolver::fullRewardZone<CURRENT_REWARD_CHECKPOINT_INDEX>(version);
}

}
}
}

#undef CURRENT_REWARD_CHECKPOINT_INDEX
