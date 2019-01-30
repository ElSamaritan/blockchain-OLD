/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#pragma once

#include <cinttypes>
#include <algorithm>
#include <vector>

#include "Xi/Config/BlockVersion.h"

#undef MakeBlockVersionCheckpoint

namespace Xi {
namespace Config {
namespace BlockVersion {

struct BlockVersionCheckpointResolver {
  template <uint8_t _Index>
  static inline uint8_t version(uint32_t height);

  template <uint8_t _Index>
  static inline constexpr bool exists(uint8_t majorVersion);

  template <uint8_t>
  static inline void versions(std::vector<uint8_t>& versions);

  template <uint8_t _Index>
  static inline uint8_t maximum();

  template <uint8_t _UpgradeHeight>
  static inline uint32_t upgradeHeight(uint8_t majorVersion);

  template <uint8_t>
  static inline void upgradeHeights(std::vector<uint32_t>& heights);

  template <uint8_t>
  static inline void forks(std::vector<uint32_t>& heights);
};

template <>
inline uint8_t BlockVersionCheckpointResolver::version<0>(uint32_t) {
  return BlockVersionCheckpoint<0>::version();
}
template <uint8_t _Index>
inline uint8_t BlockVersionCheckpointResolver::version(uint32_t height) {
  if (height >= BlockVersionCheckpoint<_Index>::height())
    return BlockVersionCheckpoint<_Index>::version();
  else
    return version<_Index - 1>(height);
}

template <>
inline constexpr bool BlockVersionCheckpointResolver::exists<0>(uint8_t majorVersion) {
  return BlockVersionCheckpoint<0>::version() == majorVersion;
}

template <uint8_t _Version>
inline constexpr bool BlockVersionCheckpointResolver::exists(uint8_t majorVersion) {
  return BlockVersionCheckpoint<_Version>::version() == majorVersion || exists<_Version - 1>(majorVersion);
}

template <>
inline void BlockVersionCheckpointResolver::versions<0>(std::vector<uint8_t>& _versions) {
  _versions.push_back(BlockVersionCheckpoint<0>::version());
}
template <uint8_t _Index>
inline void BlockVersionCheckpointResolver::versions(std::vector<uint8_t>& _versions) {
  versions<_Index - 1>(_versions);
  _versions.push_back(BlockVersionCheckpoint<_Index>::version());
}

template <>
inline uint8_t BlockVersionCheckpointResolver::maximum<0>() {
  return BlockVersionCheckpoint<0>::version();
}
template <uint8_t _Index>
inline uint8_t BlockVersionCheckpointResolver::maximum() {
  return std::max(BlockVersionCheckpoint<_Index>::version(), maximum<_Index - 1>());
}

template <>
inline uint32_t BlockVersionCheckpointResolver::upgradeHeight<0>(uint8_t majorVersion) {
  if (majorVersion != BlockVersionCheckpoint<0>::version())
    return static_cast<uint32_t>(-1);
  else
    return BlockVersionCheckpoint<0>::height();
}
template <uint8_t _Index>
inline uint32_t BlockVersionCheckpointResolver::upgradeHeight(uint8_t majorVersion) {
  if (majorVersion == BlockVersionCheckpoint<_Index>::version())
    return BlockVersionCheckpoint<_Index>::height();
  else
    return upgradeHeight<_Index - 1>(majorVersion);
}

template <>
inline void BlockVersionCheckpointResolver::upgradeHeights<0>(std::vector<uint32_t>& heights) {
  heights.push_back(BlockVersionCheckpoint<0>::height());
}
template <uint8_t _Index>
inline void BlockVersionCheckpointResolver::upgradeHeights(std::vector<uint32_t>& heights) {
  upgradeHeights<_Index - 1>(heights);
  heights.push_back(BlockVersionCheckpoint<_Index>::height());
}

template <>
inline void BlockVersionCheckpointResolver::forks<0>(std::vector<uint32_t>& heights) {
  if (BlockVersionCheckpoint<0>::isFork()) {
    heights.push_back(BlockVersionCheckpoint<0>::height());
  }
}
template <uint8_t _Index>
inline void BlockVersionCheckpointResolver::forks(std::vector<uint32_t>& heights) {
  upgradeHeights<_Index - 1>(heights);
  if (BlockVersionCheckpoint<_Index>::isFork()) {
    heights.push_back(BlockVersionCheckpoint<_Index>::height());
  }
}

inline uint8_t version(uint32_t height) {
  return BlockVersionCheckpointResolver::version<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>(height);
}

inline constexpr bool exists(uint8_t majorVersion) {
  return BlockVersionCheckpointResolver::exists<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>(majorVersion);
}

inline std::vector<uint8_t> versions() {
  std::vector<uint8_t> _versions;
  BlockVersionCheckpointResolver::versions<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>(_versions);
  return _versions;
}

inline uint8_t maximum() { return BlockVersionCheckpointResolver::maximum<CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX>(); }

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

}  // namespace BlockVersion
}  // namespace Config
}  // namespace Xi

#undef CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX
