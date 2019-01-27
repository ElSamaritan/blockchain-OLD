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

#include <algorithm>
#include <stdexcept>

#include "Difficulty.h"

// LWMA-3 difficulty algorithm
// Copyright (c) 2017-2018 Zawy, MIT License
// https://github.com/zawy12/difficulty-algorithms/issues/3
uint64_t CryptoNote::Difficulty::LWMA_3::operator()(const std::vector<uint64_t>& timestamps,
                                                    const std::vector<uint64_t>& cumulativeDifficulties,
                                                    uint32_t windowSize, uint16_t blockTime) const {
  uint16_t T = blockTime;
  uint64_t N = windowSize;
  uint64_t L(0), ST, sum_3_ST(0), next_D, prev_D, nextTime, lastTime;

  // If we dont have enough data this method is missused and we shall throw an exception.
  if (timestamps.size() < static_cast<uint64_t>(N + 1) ||
      cumulativeDifficulties.size() < static_cast<uint64_t>(N + 1)) {
    throw std::runtime_error{"LWMA called with insufficient timestamps or difficulties."};
  }

  lastTime = timestamps[0];

  for (uint64_t i = 1; i <= N; i++) {
    if (timestamps[i] > lastTime) {
      nextTime = timestamps[i];
    } else {
      nextTime = lastTime + 1;
    }
    ST = std::min(6 * static_cast<uint64_t>(T), nextTime - lastTime);
    lastTime = nextTime;
    L += ST * i;
    if (i > N - 3) {
      sum_3_ST += ST;
    }
  }

  next_D =
      (static_cast<int64_t>(cumulativeDifficulties[N] - cumulativeDifficulties[0]) * T * (N + 1) * 99) / (100 * 2 * L);

  prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N - 1];

  next_D = std::max((prev_D * 67) / 100, std::min(next_D, (prev_D * 150) / 100));

  if (sum_3_ST < (9 * T) / 10) {
    next_D = std::max(next_D, (prev_D * 108) / 100);
  }

  return static_cast<uint64_t>(next_D);
}

// LWMA-2 difficulty algorithm
// Copyright (c) 2017-2018 Zawy, MIT License
// https://github.com/zawy12/difficulty-algorithms/issues/3
uint64_t CryptoNote::Difficulty::LWMA_2::operator()(const std::vector<uint64_t>& timestamps,
                                                    const std::vector<uint64_t>& cumulativeDifficulties,
                                                    uint32_t windowSize, uint16_t blockTime) const {
  uint16_t T = blockTime;
  int64_t N = windowSize;
  int64_t L(0), ST, sum_3_ST(0), next_D, prev_D;

  // If we dont have enough data this method is missused and we shall throw an exception.
  if (timestamps.size() < static_cast<uint64_t>(N + 1) ||
      cumulativeDifficulties.size() < static_cast<uint64_t>(N + 1)) {
    throw std::runtime_error{"LWMA called with insufficient timestamps or difficulties."};
  }

  for (int64_t i = 1; i <= N; i++) {
    ST = static_cast<int64_t>(timestamps[i]) - static_cast<int64_t>(timestamps[i - 1]);

    ST = std::max(-4 * static_cast<int64_t>(T), std::min(ST, 6 * static_cast<int64_t>(T)));

    L += ST * i;

    if (i > N - 3) {
      sum_3_ST += ST;
    }
  }

  next_D =
      (static_cast<int64_t>(cumulativeDifficulties[N] - cumulativeDifficulties[0]) * T * (N + 1) * 99) / (100 * 2 * L);

  prev_D = cumulativeDifficulties[N] - cumulativeDifficulties[N - 1];

  next_D = std::max((prev_D * 67) / 100, std::min(next_D, (prev_D * 150) / 100));

  if (sum_3_ST < (8 * T) / 10) {
    next_D = std::max(next_D, (prev_D * 108) / 100);
  }

  return static_cast<uint64_t>(next_D);
}
