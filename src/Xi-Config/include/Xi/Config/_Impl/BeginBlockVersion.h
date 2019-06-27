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

namespace Xi {
namespace Config {
namespace BlockVersion {
template <Blockchain::Block::Version::value_type>
struct BlockVersionCheckpoint {
  static inline constexpr bool exists() { return false; }
};
}  // namespace BlockVersion
}  // namespace Config
}  // namespace Xi

#define MakeBlockVersionCheckpoint(_Index, _Height, _Version, _IsFork)                                            \
  namespace Xi {                                                                                                  \
  namespace Config {                                                                                              \
  namespace BlockVersion {                                                                                        \
  template <>                                                                                                     \
  struct BlockVersionCheckpoint<_Index> {                                                                         \
    static inline constexpr bool exists() { return true; }                                                        \
    static inline constexpr uint8_t index() { return _Index; }                                                    \
    static inline constexpr uint32_t height() { return _Height; }                                                 \
    static inline constexpr Blockchain::Block::Version version() { return Blockchain::Block::Version{_Version}; } \
    static inline constexpr bool isFork() { return _IsFork; }                                                     \
  };                                                                                                              \
  }                                                                                                               \
  }                                                                                                               \
  }
