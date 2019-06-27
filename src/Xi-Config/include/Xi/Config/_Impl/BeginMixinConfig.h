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

#include "Xi/Config/BlockVersion.h"

namespace Xi {
namespace Config {
namespace Mixin {
template <Blockchain::Block::Version::value_type _Index>
struct MixinCheckpoint;
}
}  // namespace Config
}  // namespace Xi

#define MakeMixinCheckpoint(_Index, _Version, _Required)                                                          \
  namespace Xi {                                                                                                  \
  namespace Config {                                                                                              \
  namespace Mixin {                                                                                               \
  template <>                                                                                                     \
  struct MixinCheckpoint<_Index> {                                                                                \
    static inline constexpr uint8_t index() { return _Index; }                                                    \
    static inline constexpr Blockchain::Block::Version version() { return Blockchain::Block::Version{_Version}; } \
    static inline constexpr uint8_t required() { return _Required; }                                              \
    static_assert(::Xi::Config::BlockVersion::exists(Blockchain::Block::Version{_Version}),                       \
                  "Non existing block version referenced.");                                                      \
  };                                                                                                              \
  }                                                                                                               \
  }                                                                                                               \
  }
