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

#include <algorithm>
#include <stdexcept>
#include <cassert>
#include <algorithm>

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

uint64_t CryptoNote::Difficulty::LWMA_1::operator()(const std::vector<uint64_t>& timestamps,
                                                    const std::vector<uint64_t>& cumulativeDifficulties,
                                                    uint32_t windowSize, uint16_t blockTime) const {
  // This old way was not very proper
  // uint64_t  T = DIFFICULTY_TARGET;
  // uint64_t  N = DIFFICULTY_WINDOW; // N=60, 90, and 150 for T=600, 120, 60.

  auto N = windowSize;
  assert(timestamps.size() == N + 1);

  auto T = blockTime;
  uint64_t L(0), next_D, i, this_timestamp(0), previous_timestamp(0), avg_D;

  previous_timestamp = timestamps[0] - T;
  for (i = 1; i <= N; i++) {
    // Safely prevent out-of-sequence timestamps
    if (timestamps[i] > previous_timestamp) {
      this_timestamp = timestamps[i];
    } else {
      this_timestamp = previous_timestamp + 1;
    }
    L += i * std::min<uint64_t>(6ULL * T, this_timestamp - previous_timestamp);
    previous_timestamp = this_timestamp;
  }
  if (L < N * N * T / 20) {
    L = N * N * T / 20;
  }
  avg_D = (cumulativeDifficulties[N] - cumulativeDifficulties[0]) / N;

  // Prevent round off error for small D and overflow for large D.
  if (avg_D > 2000000 * N * N * T) {
    next_D = (avg_D / (200 * L)) * (N * (N + 1) * T * 99);
  } else {
    next_D = (avg_D * N * (N + 1) * T * 99) / (200 * L);
  }

  // Optional. Make all insignificant digits zero for easy reading.
  i = 1000000000;
  while (i > 1) {
    if (next_D > i * 100) {
      next_D = ((next_D + i / 2) / i) * i;
      break;
    } else {
      i /= 10;
    }
  }
  return next_D;
}

std::shared_ptr<CryptoNote::Difficulty::IDifficultyAlgorithm> CryptoNote::Difficulty::makeDifficultyAlgorithm(
    std::string_view id) {
  if (id == "LWMA-v1") {
    return std::make_shared<LWMA_1>();
  } else if (id == "LWMA-v2") {
    return std::make_shared<LWMA_2>();
  } else if (id == "LWMA-v3") {
    return std::make_shared<LWMA_3>();
  } else {
    return nullptr;
  }
}
