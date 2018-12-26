/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include "Xi/Config/Dust.h"

#undef MakeDustCheckpoint

#ifndef CURRENT_DUST_CHECKPOINT_INDEX
#pragma error "CURRENT_DUST_CHECKPOINT_INDEX must be defines"
#endif

namespace Xi {
namespace Config {
namespace Dust {

struct DustCheckpointResolver {
  template <uint8_t>
  static inline uint64_t dust(uint8_t version);
};

template <>
inline uint64_t DustCheckpointResolver::dust<0>(uint8_t) {
  return DustCheckpoint<0>::dust();
}
template <uint8_t _Index>
inline uint64_t DustCheckpointResolver::dust(uint8_t version) {
  if (version >= DustCheckpoint<_Index>::version())
    return DustCheckpoint<_Index>::dust();
  else
    return dust<_Index - 1>(version);
}

inline uint64_t dust(uint8_t version) { return DustCheckpointResolver::dust<CURRENT_DUST_CHECKPOINT_INDEX>(version); }

}  // namespace Dust
}  // namespace Config
}  // namespace Xi

#undef CURRENT_DUST_CHECKPOINT_INDEX

#ifndef CURRENT_FUSION_DUST_CHECKPOINT_INDEX
#pragma error "CURRENT_FUSION_DUST_CHECKPOINT_INDEX must be defines"
#endif

#undef MakeFusionDustCheckpoint

namespace Xi {
namespace Config {
namespace Dust {

struct FusionDustCheckpointResolver {
  template <uint8_t>
  static inline uint64_t dust(uint8_t version);
};

template <>
inline uint64_t FusionDustCheckpointResolver::dust<0>(uint8_t) {
  return FusionDustCheckpoint<0>::dust();
}
template <uint8_t _Index>
inline uint64_t FusionDustCheckpointResolver::dust(uint8_t version) {
  if (version >= FusionDustCheckpoint<_Index>::version())
    return FusionDustCheckpoint<_Index>::dust();
  else
    return dust<_Index - 1>(version);
}

inline uint64_t fusionDust(uint8_t version) {
  return FusionDustCheckpointResolver::dust<CURRENT_FUSION_DUST_CHECKPOINT_INDEX>(version);
}

}  // namespace Dust
}  // namespace Config
}  // namespace Xi

#undef CURRENT_FUSION_DUST_CHECKPOINT_INDEX
