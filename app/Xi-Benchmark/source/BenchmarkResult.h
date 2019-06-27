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

#include <vector>
#include <chrono>
#include <string>

#include <Xi/ExternalIncludePush.h>
#include <cpuinfo_x86.h>
#include <Xi/ExternalIncludePop.h>

namespace XiBenchmark {
double to_hashrate(std::chrono::nanoseconds duration, size_t hashes);

struct BenchmarkTimeSpan {
  std::chrono::high_resolution_clock::time_point Begin;
  std::chrono::high_resolution_clock::time_point End;
  std::chrono::high_resolution_clock::duration Duration;

  void start() { Begin = std::chrono::high_resolution_clock::now(); }

  void stop() {
    End = std::chrono::high_resolution_clock::now();
    Duration = End - Begin;
  }
};

struct BenchmarkResult {
  std::string Algorithm;
  uint32_t Blocks;
  uint32_t Size;
  std::vector<BenchmarkTimeSpan> ThreadResults;

  BenchmarkResult(uint32_t blocks, uint32_t size, size_t threads, const std::string& algo)
      : Algorithm{algo}, Blocks{blocks}, Size{size}, ThreadResults{threads} {}

  std::chrono::nanoseconds bestDuration() const;
  std::chrono::nanoseconds worstDuration() const;
  std::chrono::nanoseconds averageDuration() const;
  std::chrono::nanoseconds totalDuration() const;

  std::vector<std::chrono::nanoseconds> threadDurations() const;
  size_t totalHashes() const;

  double bestHashrate() const;
  double worstHashrate() const;
  double averageHashrate() const;
  double totalHashrate() const;
};

struct BencharkSummary {
  cpu_features::X86Info CPUInfo;
  std::string Architecture;
  std::vector<BenchmarkResult> Benchmarks;

  BencharkSummary();
};
}  // namespace XiBenchmark
