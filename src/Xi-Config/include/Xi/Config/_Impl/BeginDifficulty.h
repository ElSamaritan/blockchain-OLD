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

#include <vector>
#include <cinttypes>
#include <chrono>

#include <Xi/Blockchain/Block/Version.hpp>

#include "CryptoNoteCore/Difficulty.h"
#include "Xi/Config/BlockVersion.h"

namespace Xi {
namespace Config {
namespace Difficulty {
template <Blockchain::Block::Version::value_type _Index>
struct DifficultyCheckpoint;
}
}  // namespace Config
}  // namespace Xi

#define MakeDifficultyCheckpoint(_Index, _Version, _Window, _Initial, _Algorithm)                                 \
  namespace Xi {                                                                                                  \
  namespace Config {                                                                                              \
  namespace Difficulty {                                                                                          \
  template <>                                                                                                     \
  struct DifficultyCheckpoint<_Index> {                                                                           \
    static inline constexpr uint8_t index() { return _Index; }                                                    \
    static inline constexpr Blockchain::Block::Version version() { return Blockchain::Block::Version{_Version}; } \
    static inline constexpr uint32_t windowSize() { return _Window; }                                             \
    static inline constexpr uint64_t initialValue() { return _Initial; }                                          \
    using algorithm = _Algorithm;                                                                                 \
    static_assert(::Xi::Config::BlockVersion::exists(Blockchain::Block::Version{_Version}),                       \
                  "Non existing major block version referenced.");                                                \
  };                                                                                                              \
  }                                                                                                               \
  }                                                                                                               \
  }
