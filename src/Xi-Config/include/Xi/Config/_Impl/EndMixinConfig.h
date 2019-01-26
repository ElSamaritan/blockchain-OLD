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

#ifndef CURRENT_MIXIN_CHECKPOINT_INDEX
#pragma error "CURRENT_MIXIN_CHECKPOINT_INDEX must be defined."
#endif

#undef MakeMixinCheckpoint

namespace Xi {
namespace Config {
namespace Mixin {

struct MixinCheckpointResolver {
  template <uint8_t _Index>
  static inline bool isZeroMixinAllowed(uint8_t version);

  template <uint8_t _Index>
  static inline uint8_t minimum(uint8_t version);

  template <uint8_t _Index>
  static inline uint8_t maximum(uint8_t version);

  template <uint8_t _Index>
  static inline uint8_t defaultValue(uint8_t version);
};

template <>
inline bool MixinCheckpointResolver::isZeroMixinAllowed<0>(uint8_t) {
  return MixinCheckpoint<0>::minimum() == 0;
}
template <uint8_t _Index>
inline bool MixinCheckpointResolver::isZeroMixinAllowed(uint8_t version) {
  if (version >= MixinCheckpoint<_Index>::version())
    return MixinCheckpoint<_Index>::minimum() == 0;
  else
    return isZeroMixinAllowed<_Index - 1>(version);
}

template <>
inline uint8_t MixinCheckpointResolver::minimum<0>(uint8_t) {
  return MixinCheckpoint<0>::minimum();
}
template <uint8_t _Index>
inline uint8_t MixinCheckpointResolver::minimum(uint8_t version) {
  if (version >= MixinCheckpoint<_Index>::version())
    return MixinCheckpoint<_Index>::minimum();
  else
    return minimum<_Index - 1>(version);
}

template <>
inline uint8_t MixinCheckpointResolver::maximum<0>(uint8_t) {
  return MixinCheckpoint<0>::maximum();
}
template <uint8_t _Index>
inline uint8_t MixinCheckpointResolver::maximum(uint8_t version) {
  if (version >= MixinCheckpoint<_Index>::version())
    return MixinCheckpoint<_Index>::maximum();
  else
    return maximum<_Index - 1>(version);
}

template <>
inline uint8_t MixinCheckpointResolver::defaultValue<0>(uint8_t) {
  return MixinCheckpoint<0>::defaultValue();
}
template <uint8_t _Index>
inline uint8_t MixinCheckpointResolver::defaultValue(uint8_t version) {
  if (version >= MixinCheckpoint<_Index>::version())
    return MixinCheckpoint<_Index>::defaultValue();
  else
    return defaultValue<_Index - 1>(version);
}

inline bool isZeroMixinAllowed(uint8_t version) {
  return MixinCheckpointResolver::isZeroMixinAllowed<CURRENT_MIXIN_CHECKPOINT_INDEX>(version);
}
inline uint8_t minimum(uint8_t version) {
  return MixinCheckpointResolver::minimum<CURRENT_MIXIN_CHECKPOINT_INDEX>(version);
}
inline uint8_t maximum(uint8_t version) {
  return MixinCheckpointResolver::maximum<CURRENT_MIXIN_CHECKPOINT_INDEX>(version);
}
inline uint8_t defaultValue(uint8_t version) {
  return MixinCheckpointResolver::defaultValue<CURRENT_MIXIN_CHECKPOINT_INDEX>(version);
}

}  // namespace Mixin
}  // namespace Config
}  // namespace Xi

#undef CURRENT_MIXIN_CHECKPOINT_INDEX
