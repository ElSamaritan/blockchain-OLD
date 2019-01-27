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

#include "Xi/Config/BlockVersion.h"

namespace Xi {
namespace Config {
namespace Dust {
template <uint8_t _Index>
struct DustCheckpoint;
template <uint8_t _Index>
struct FusionDustCheckpoint;
}  // namespace Dust
}  // namespace Config
}  // namespace Xi

#define MakeDustCheckpoint(_Index, _Version, _Dust)                                                              \
  namespace Xi {                                                                                                 \
  namespace Config {                                                                                             \
  namespace Dust {                                                                                               \
  template <>                                                                                                    \
  struct DustCheckpoint<_Index> {                                                                                \
    static inline constexpr uint8_t index() { return _Index; }                                                   \
    static inline constexpr uint8_t version() { return _Version; }                                               \
    static inline constexpr uint64_t dust() { return _Dust; }                                                    \
    static_assert(::Xi::Config::BlockVersion::exists(_Version), "Non existing major block version referenced."); \
  };                                                                                                             \
  }                                                                                                              \
  }                                                                                                              \
  }

#define MakeFusionDustCheckpoint(_Index, _Version, _Dust)                                                        \
  namespace Xi {                                                                                                 \
  namespace Config {                                                                                             \
  namespace Dust {                                                                                               \
  template <>                                                                                                    \
  struct FusionDustCheckpoint<_Index> {                                                                          \
    static inline constexpr uint8_t index() { return _Index; }                                                   \
    static inline constexpr uint8_t version() { return _Version; }                                               \
    static inline constexpr uint64_t dust() { return _Dust; }                                                    \
    static_assert(::Xi::Config::BlockVersion::exists(_Version), "Non existing major block version referenced."); \
  };                                                                                                             \
  }                                                                                                              \
  }                                                                                                              \
  }
