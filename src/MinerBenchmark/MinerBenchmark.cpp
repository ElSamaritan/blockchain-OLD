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

#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <cinttypes>
#include <climits>
#include <random>
#include <atomic>
#include <iostream>

#include <Xi/Global.h>

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <cpuinfo_x86.h>
#include <Xi/Utils/ExternalIncludePop.h>

#include <CommonCLI/CommonCLI.h>
#include <Logging/ConsoleLogger.h>
#include <Logging/LoggerRef.h>
#include <Serialization/JsonOutputStreamSerializer.h>
#include <crypto/cnx/cnx.h>

#include "MinerBenchmark/BenchmarkResult.h"
#include "MinerBenchmark/BenchmarkSerialization.h"

namespace {
using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, uint16_t>;

std::vector<uint16_t> generate_random_blocks(size_t count, size_t size) {
  std::vector<uint16_t> blocks;
  random_bytes_engine rbe;
  blocks.resize(count * size / 2);
  std::generate(blocks.begin(), blocks.end(), std::ref(rbe));
  return blocks;
}
}  // namespace

using HashAlgorithm = Crypto::CNX::Hash_v0;

int main(int argc, char** argv) {
  try {
    using namespace ::CLIMinerBenchmark;

    cxxopts::Options cliOptions("xi-benchmark", "benchmarks the current cn-x hash algorithm implementation");
    CommonCLI::emplaceCLIOptions(cliOptions);

    bool silent = false;
    size_t threads = std::thread::hardware_concurrency();
    uint32_t blocks = 1000;
    const uint32_t size = 410;
    // clang-format off
    cliOptions.add_options("benchmark")
        ("s,silent", "disables progress logging", cxxopts::value<bool>(silent)->implicit_value("true"))

        ("t,threads", "threads to use, 0 for benchmarking all variations",
            cxxopts::value<size_t>(threads)->default_value(std::to_string(threads)))

        ("b,blocks", "number of blocks to hash",
            cxxopts::value<uint32_t>(blocks)->default_value(std::to_string(blocks)))
    ;
    // clang-format on

    auto cliParseResult = cliOptions.parse(argc, argv);
    if (CommonCLI::handleCLIOptions(cliOptions, cliParseResult)) {
      return 0;
    }

    XI_UNUSED_REVAL(CommonCLI::make_crash_dumper("xi-benchmark"));

    Logging::ConsoleLogger consoleLogger{(silent ? Logging::INFO : Logging::TRACE)};
    consoleLogger.setPattern("");
    Logging::LoggerRef logger{consoleLogger, ""};

    const size_t maxThreadUsage = threads == 0 ? std::thread::hardware_concurrency() : threads;
    const size_t minThreadUsage = threads == 0 ? 1 : threads;
    const auto blockData = generate_random_blocks(blocks * maxThreadUsage, size);
    BencharkSummary summary;

    for (threads = minThreadUsage; threads <= maxThreadUsage; ++threads) {
      logger() << "starting benchmark using " << threads << " threads";
      BenchmarkResult result{blocks, size, threads, "CNX_v0_0"};
      std::vector<std::thread> worker;
      worker.reserve(threads);

      for (size_t i = 0; i < threads; ++i) {
        worker.emplace_back([&blockData, &result, i, blocks, size]() {
          result.ThreadResults[i].start();
          const char* data = reinterpret_cast<const char*>(blockData.data());
          for (size_t j = 0; j < blocks; ++j) {
            Crypto::Hash hash;
            HashAlgorithm{}(data + (i * blocks + j) * size, size, hash);
          }
          result.ThreadResults[i].stop();
        });
      }
      for (size_t i = 0; i < worker.size(); ++i) {
        worker[i].join();
      }
      summary.Benchmarks.push_back(result);
      logger() << "Block Size       : " << size;
      logger() << "Hashes Per Thread: " << blocks;
      logger() << "Slowest          : " << result.worstDuration().count() << "ns";
      logger() << "Best             : " << result.bestDuration().count() << "ns";
      logger() << "Average          : " << result.averageDuration().count() << "ns";
      logger() << "Total Hashes     : " << result.totalHashes();
      logger() << "Slowest Hashrate : " << result.worstHashrate() << "H/s";
      logger() << "Best    Hashrate : " << result.bestHashrate() << "H/s";
      logger() << "Average Hashrate : " << result.averageHashrate() << "H/s";
      logger() << "Total   Hashrate : " << result.totalHashrate() << "H/s";
    }

    CryptoNote::JsonOutputStreamSerializer serializer{};
    serialize_benchmark_summary(summary, serializer);
    logger() << serializer.getValue().toString();
  } catch (cxxopts::OptionException& e) {
    std::cout << e.what();
    return -1;
  }

  return 0;
}
