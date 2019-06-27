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

namespace Crypto {
namespace CNX {

template <uint32_t _WindowSize, uint32_t _MinScratchpadSize, uint32_t _MaxScratchpadSize>
struct Configuration {
  static constexpr uint32_t windowSize() { return _WindowSize; }

  static constexpr uint32_t minScratchpadSize() { return _MinScratchpadSize; }
  static constexpr uint32_t maxScratchpadSize() { return _MaxScratchpadSize; }
  static constexpr uint32_t slopeScratchpadSize() { return (maxScratchpadSize() - minScratchpadSize()) / windowSize(); }
  static constexpr uint32_t scratchpadSizeForOffset(uint32_t offset) {
    return minScratchpadSize() + offset * slopeScratchpadSize();
  }

  static_assert(scratchpadSizeForOffset(0) == minScratchpadSize(), "");
  static_assert(scratchpadSizeForOffset(windowSize()) == maxScratchpadSize(), "");
  static_assert((scratchpadSizeForOffset(0) % (8 * 16)) == 0,
                "Memory size must always algin with INIT_SIZE_BULK * AES_KEY_SIZE");
  static_assert((slopeScratchpadSize() % (8 * 16)) == 0,
                "Memory size must always algin with INIT_SIZE_BULK * AES_KEY_SIZE");

  static constexpr uint32_t offsetForHeight(uint32_t height) { return height % (windowSize() + 1); }

  static_assert(offsetForHeight(0) == 0, "");
  static_assert(offsetForHeight(windowSize()) == windowSize(), "");
};

}  // namespace CNX
}  // namespace Crypto
