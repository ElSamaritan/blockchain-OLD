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

namespace Crypto {
namespace CNX {

template <uint32_t _WindowSize, uint32_t _MinRandomizerSize, uint32_t _MaxRandomizerSize, uint32_t _MinScratchpadSize,
          uint32_t _MaxScratchpadSize>
struct Configuration {
  static constexpr int variant() { return 1; }
  static constexpr uint32_t windowSize() { return _WindowSize; }

  static constexpr uint32_t minRandomizerSize() { return _MinRandomizerSize; }
  static constexpr uint32_t maxRandomizerSize() { return _MaxRandomizerSize; }
  static constexpr uint32_t slopeRandomizerSize() { return (maxRandomizerSize() - minRandomizerSize()) / windowSize(); }
  static constexpr uint32_t randomizerSizeForOffset(uint32_t offset) {
    return maxRandomizerSize() - offset * slopeRandomizerSize();
  }

  static_assert(randomizerSizeForOffset(0) == maxRandomizerSize(), "");
  static_assert(randomizerSizeForOffset(windowSize()) == minRandomizerSize(), "");

  static constexpr uint32_t minScratchpadSize() { return _MinScratchpadSize; }
  static constexpr uint32_t maxScratchpadSize() { return _MaxScratchpadSize; }
  static constexpr uint32_t slopeScratchpadSize() { return (maxScratchpadSize() - minScratchpadSize()) / windowSize(); }
  static constexpr uint32_t scratchpadSizeForOffset(uint32_t offset) {
    return minScratchpadSize() + offset * slopeScratchpadSize();
  }

  static_assert(scratchpadSizeForOffset(0) == minScratchpadSize(), "");
  static_assert(scratchpadSizeForOffset(windowSize()) == maxScratchpadSize(), "");

  static_assert(minScratchpadSize() % maxRandomizerSize() == 0, "Scratchpad and randomizer must align in memory.");
  static_assert(slopeScratchpadSize() % slopeRandomizerSize() == 0, "Scratchpad and randomizer must align in memory.");

  static constexpr uint32_t offsetForHeight(uint32_t height) {
    uint32_t base_offset = (height % windowSize());
    int32_t offset = static_cast<int32_t>(height % (windowSize() * 2)) - static_cast<int32_t>(base_offset * 2);
    if (offset < 0) {
      offset = static_cast<int32_t>(base_offset);
    }
    return static_cast<uint32_t>(offset);
  }

  static_assert(offsetForHeight(0) == 0, "");
  static_assert(offsetForHeight(windowSize()) == windowSize(), "");
  static_assert(offsetForHeight(windowSize() + 1) == windowSize() - 1, "");
  static_assert(offsetForHeight(windowSize() - 1) == windowSize() - 1, "");
  static_assert(offsetForHeight(2 * windowSize()) == 0, "");
};

}  // namespace CNX
}  // namespace Crypto
