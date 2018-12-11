#pragma once

#include <cinttypes>
#include <algorithm>
#include <vector>

#include "config/BlockVersion.h"

#undef MakeBlockVersionCheckpoint

namespace CryptoNote {
namespace Config {
namespace BlockVersion {

struct BlockVersionCheckpointResolver {
  template<uint8_t _Index>
  static inline uint8_t version(uint32_t height);

  template<uint8_t>
  static inline void versions(std::vector<uint8_t>& versions);

  template<uint8_t _Index>
  static inline uint8_t maximum();

  template<uint8_t _UpgradeHeight>
  static inline uint32_t upgradeHeight(uint8_t majorVersion);

  template<uint8_t>
  static inline void upgradeHeights(std::vector<uint32_t>& heights);

  template<uint8_t>
  static inline void forks(std::vector<uint32_t>& heights);
};

template<> inline uint8_t BlockVersionCheckpointResolver::version<0>(uint32_t) {
  return BlockVersionCheckpoint<0>::version();
}
template<uint8_t _Index> inline uint8_t BlockVersionCheckpointResolver::version(uint32_t height) {
  if(height >= BlockVersionCheckpoint<_Index>::height())
    return BlockVersionCheckpoint<_Index>::version();
  else
    return version<_Index - 1>(height);
}

template<>
inline void BlockVersionCheckpointResolver::versions<0>(std::vector<uint8_t>& _versions) {
  _versions.push_back(BlockVersionCheckpoint<0>::version());
}
template<uint8_t _Index>
inline void BlockVersionCheckpointResolver::versions(std::vector<uint8_t>& _versions) {
  versions<_Index - 1>(_versions);
  _versions.push_back(BlockVersionCheckpoint<_Index>::version());
}

template<> inline uint8_t BlockVersionCheckpointResolver::maximum<0>() {
  return BlockVersionCheckpoint<0>::version();
}
template<uint8_t _Index> inline uint8_t BlockVersionCheckpointResolver::maximum() {
  return std::max(BlockVersionCheckpoint<_Index>::version(), maximum<_Index - 1>());
}

template<> inline uint32_t BlockVersionCheckpointResolver::upgradeHeight<0>(uint8_t majorVersion) {
  if(majorVersion != BlockVersionCheckpoint<0>::version())
    return static_cast<uint32_t>(-1);
  else
    return BlockVersionCheckpoint<0>::height();
}
template<uint8_t _Index> inline uint32_t BlockVersionCheckpointResolver::upgradeHeight(uint8_t majorVersion) {
  if(majorVersion == BlockVersionCheckpoint<_Index>::version())
    return BlockVersionCheckpoint<_Index>::height();
  else
    return upgradeHeight<_Index - 1>(majorVersion);
}

template<>
inline void BlockVersionCheckpointResolver::upgradeHeights<0>(std::vector<uint32_t>& heights) {
  heights.push_back(BlockVersionCheckpoint<0>::height());
}
template<uint8_t _Index>
inline void BlockVersionCheckpointResolver::upgradeHeights(std::vector<uint32_t>& heights) {
  upgradeHeights<_Index - 1>(heights);
  heights.push_back(BlockVersionCheckpoint<_Index>::height());
}

template<>
inline void BlockVersionCheckpointResolver::forks<0>(std::vector<uint32_t>& heights) {
  if(BlockVersionCheckpoint<0>::isFork()) {
    heights.push_back(BlockVersionCheckpoint<0>::height());
  }
}
template<uint8_t _Index>
inline void BlockVersionCheckpointResolver::forks(std::vector<uint32_t>& heights) {
  upgradeHeights<_Index - 1>(heights);
  if(BlockVersionCheckpoint<_Index>::isFork()) {
    heights.push_back(BlockVersionCheckpoint<_Index>::height());
  }
}

inline uint8_t version(uint32_t height) {
  return BlockVersionCheckpointResolver::version<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>(height);
}

inline std::vector<uint8_t> versions() {
  std::vector<uint8_t> _versions;
  BlockVersionCheckpointResolver::versions<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>(_versions);
  return _versions;
}

inline uint8_t maximum() {
  return BlockVersionCheckpointResolver::maximum<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>();
}

inline uint32_t upgradeHeight(uint8_t majorVersion) {
  return BlockVersionCheckpointResolver::upgradeHeight<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>(majorVersion);
}

inline std::vector<uint32_t> upgradeHeights() {
  std::vector<uint32_t> reval;
  BlockVersionCheckpointResolver::upgradeHeights<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>(reval);
  return reval;
}

inline std::vector<uint32_t> forks() {
  std::vector<uint32_t> reval;
  BlockVersionCheckpointResolver::forks<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>(reval);
  return reval;
}

}
}
}

#undef CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX
