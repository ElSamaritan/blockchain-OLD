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

#include <cstdint>
#include <vector>
#include <algorithm>
#include <memory>
#include <string_view>

namespace CryptoNote {
namespace Difficulty {

struct IDifficultyAlgorithm {
  virtual ~IDifficultyAlgorithm() = default;

  virtual uint64_t operator()(const std::vector<uint64_t> &timestamps,
                              const std::vector<uint64_t> &cumulativeDifficulties, uint32_t windowSize,
                              uint16_t blockTime) const = 0;
};

struct LWMA_3 : IDifficultyAlgorithm {
  ~LWMA_3() override = default;
  uint64_t operator()(const std::vector<uint64_t> &timestamps, const std::vector<uint64_t> &cumulativeDifficulties,
                      uint32_t windowSize, uint16_t blockTime) const override;
};

struct LWMA_2 : IDifficultyAlgorithm {
  ~LWMA_2() override = default;
  uint64_t operator()(const std::vector<uint64_t> &timestamps, const std::vector<uint64_t> &cumulativeDifficulties,
                      uint32_t windowSize, uint16_t blockTime) const override;
};

struct LWMA_1 : IDifficultyAlgorithm {
  ~LWMA_1() override = default;
  uint64_t operator()(const std::vector<uint64_t> &timestamps, const std::vector<uint64_t> &cumulativeDifficulties,
                      uint32_t windowSize, uint16_t blockTime) const override;
};

std::shared_ptr<IDifficultyAlgorithm> makeDifficultyAlgorithm(std::string_view id);

}  // namespace Difficulty
}  // namespace CryptoNote

template <typename T>
T clamp(const T &n, const T &lower, const T &upper) {
  return std::max(lower, std::min(n, upper));
}
