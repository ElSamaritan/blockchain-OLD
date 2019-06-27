/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
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

#include <Xi/Blockchain/Block/Version.hpp>

#ifndef CURRENT_MIXIN_CHECKPOINT_INDEX
#pragma error "CURRENT_MIXIN_CHECKPOINT_INDEX must be defined."
#endif

#undef MakeMixinCheckpoint

namespace Xi {
namespace Config {
namespace Mixin {

struct MixinCheckpointResolver {
  template <Blockchain::Block::Version::value_type _Index>
  static inline uint8_t required(Blockchain::Block::Version version);
};

template <>
inline uint8_t MixinCheckpointResolver::required<0>(Blockchain::Block::Version) {
  return MixinCheckpoint<0>::required();
}
template <Blockchain::Block::Version::value_type _Index>
inline uint8_t MixinCheckpointResolver::required(Blockchain::Block::Version version) {
  if (version >= MixinCheckpoint<_Index>::version())
    return MixinCheckpoint<_Index>::required();
  else
    return required<_Index - 1>(version);
}

inline uint8_t required(Blockchain::Block::Version version) {
  return MixinCheckpointResolver::required<CURRENT_MIXIN_CHECKPOINT_INDEX>(version);
}

}  // namespace Mixin
}  // namespace Config
}  // namespace Xi

#undef CURRENT_MIXIN_CHECKPOINT_INDEX
