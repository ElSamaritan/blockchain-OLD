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
#include <string>

#include <Xi/Blockchain/Block/Version.hpp>

#include "Xi/Config/BlockVersion.h"

namespace Xi {
namespace Config {
namespace StaticReward {
template <uint8_t _Index>
struct StaticRewardCheckpoint;
}
}  // namespace Config
}  // namespace Xi

#define MakeStaticRewardCheckpoint(_Index, _Version, _Amount, _Address)                                           \
  namespace Xi {                                                                                                  \
  namespace Config {                                                                                              \
  namespace StaticReward {                                                                                        \
  template <>                                                                                                     \
  struct StaticRewardCheckpoint<_Index> {                                                                         \
    static inline constexpr Blockchain::Block::Version::value_type index() { return _Index; }                     \
    static inline constexpr Blockchain::Block::Version version() { return Blockchain::Block::Version{_Version}; } \
    static inline constexpr uint64_t amount() { return _Amount; }                                                 \
    static inline std::string address() {                                                                         \
      static const std::string __Address{_Address};                                                               \
      return __Address;                                                                                           \
    }                                                                                                             \
    static_assert(::Xi::Config::BlockVersion::exists(Blockchain::Block::Version{_Version}),                       \
                  "Non existing major block version referenced.");                                                \
  };                                                                                                              \
  }                                                                                                               \
  }                                                                                                               \
  }
