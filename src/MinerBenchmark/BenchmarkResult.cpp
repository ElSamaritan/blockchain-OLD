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

#include "MinerBenchmark/BenchmarkResult.h"

#include <algorithm>
#include <iterator>
#include <numeric>

double CLIMinerBenchmark::to_hashrate(std::chrono::nanoseconds duration, size_t hashes) {
  return static_cast<double>(hashes) * 1000000000.0 / static_cast<double>(duration.count());
}

std::chrono::nanoseconds CLIMinerBenchmark::BenchmarkResult::bestDuration() const {
  const auto durations = threadDurations();
  return *std::min_element(durations.begin(), durations.end());
}

std::chrono::nanoseconds CLIMinerBenchmark::BenchmarkResult::worstDuration() const {
  const auto durations = threadDurations();
  return *std::max_element(durations.begin(), durations.end());
}

std::chrono::nanoseconds CLIMinerBenchmark::BenchmarkResult::averageDuration() const {
  return std::chrono::nanoseconds{static_cast<size_t>(totalDuration().count()) / ThreadResults.size()};
}

std::chrono::nanoseconds CLIMinerBenchmark::BenchmarkResult::totalDuration() const {
  const auto durations = threadDurations();
  return std::accumulate(durations.begin(), durations.end(), std::chrono::nanoseconds{0},
                         [](const auto acc, const auto duration) { return acc + duration; });
}

std::vector<std::chrono::nanoseconds> CLIMinerBenchmark::BenchmarkResult::threadDurations() const {
  std::vector<std::chrono::nanoseconds> reval;
  reval.reserve(ThreadResults.size());
  std::transform(ThreadResults.begin(), ThreadResults.end(), std::back_inserter(reval),
                 [](const auto& timespan) { return std::chrono::nanoseconds{timespan.Duration}; });
  return reval;
}

size_t CLIMinerBenchmark::BenchmarkResult::totalHashes() const { return Blocks * ThreadResults.size(); }
double CLIMinerBenchmark::BenchmarkResult::bestHashrate() const { return to_hashrate(bestDuration(), Blocks); }
double CLIMinerBenchmark::BenchmarkResult::worstHashrate() const { return to_hashrate(worstDuration(), Blocks); }
double CLIMinerBenchmark::BenchmarkResult::averageHashrate() const { return to_hashrate(averageDuration(), Blocks); }
double CLIMinerBenchmark::BenchmarkResult::totalHashrate() const {
  return to_hashrate(averageDuration(), totalHashes());
}

CLIMinerBenchmark::BencharkSummary::BencharkSummary() {
  CPUInfo = cpu_features::GetX86Info();
  Architecture =
      std::string{cpu_features::GetX86MicroarchitectureName(cpu_features::GetX86Microarchitecture(&CPUInfo))};
}
