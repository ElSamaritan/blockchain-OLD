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
#include <iomanip>
#include <functional>
#include <map>

#include <Xi/Global.h>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <cpuinfo_x86.h>
#include <Xi/ExternalIncludePop.h>

#include <CommonCLI/CommonCLI.h>
#include <CommonCLI/Centered.h>
#include <Logging/ConsoleLogger.h>
#include <Logging/LoggerRef.h>
#include <Serialization/JsonOutputStreamSerializer.h>
#include <crypto/cnx/cnx.h>

#include "BenchmarkResult.h"
#include "BenchmarkSerialization.h"

namespace {
using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, uint16_t>;
using format_hander = std::function<std::string(const XiBenchmark::BencharkSummary& summary)>;

std::string format_json(const XiBenchmark::BencharkSummary& summary) {
  CryptoNote::JsonOutputStreamSerializer serializer{};
  serialize_benchmark_summary(summary, serializer);
  return serializer.getValue().toString();
}

std::string format_cpu_info(const cpu_features::X86Info& nfo) {
  std::stringstream builder;
  builder << "CPUInfo: V=" << nfo.vendor << ", F=" << nfo.family << ", M=" << nfo.model << ", S=" << nfo.stepping;
  return builder.str();
}

std::string format_table(const XiBenchmark::BencharkSummary& summary) {
  using namespace CommonCLI;
  const std::string seperator = std::string{"+"} + std::string(62, '-') + std::string{"+\n"};
  std::stringstream builder;
  builder.setf(std::ios::fixed);
  builder.precision(3);
  builder << seperator;
  builder << "|" << std::setw(61) << centered(format_cpu_info(summary.CPUInfo)) << " |\n";
  builder << seperator;
  builder << "| " << std::setw(8) << centered("Threads") << " | " << std::setw(10) << centered("Total H/s") << " | "
          << std::setw(10) << centered("Avg H/s") << " | " << std::setw(10) << centered("Worst H/s") << " | "
          << std::setw(10) << centered("Best H/s") << " |\n";
  builder << seperator;
  for (const auto& iResult : summary.Benchmarks) {
    builder << "| " << std::setw(8) << iResult.ThreadResults.size() << " | " << std::setw(10) << iResult.totalHashrate()
            << " | " << std::setw(10) << iResult.averageHashrate() << " | " << std::setw(10) << iResult.worstHashrate()
            << " | " << std::setw(10) << iResult.bestHashrate() << " |\n";
  }
  builder << seperator;
  return builder.str();
}

std::string format_csv(const XiBenchmark::BencharkSummary& summary) {
  using namespace CommonCLI;
  std::stringstream builder;
  builder.setf(std::ios::fixed);
  builder.precision(3);
  builder << "Threads;Total H/s;Avg H/s;Worst H/s;Best H/s\n";
  for (const auto& iResult : summary.Benchmarks) {
    builder << iResult.ThreadResults.size() << ";" << iResult.totalHashrate() << ";" << iResult.averageHashrate() << ";"
            << iResult.worstHashrate() << ";" << iResult.bestHashrate() << "\n";
  }
  return builder.str();
}

std::string format_discord(const XiBenchmark::BencharkSummary& summary) {
  std::stringstream builder{};
  builder << "```cs\n" << format_table(summary) << "```\n";
  return builder.str();
}

const std::map<std::string, format_hander> formatters{
    {"json", format_json}, {"table", format_table}, {"discord", format_discord}, {"csv", format_csv}};

std::vector<uint16_t> generate_random_blocks(size_t count, size_t size) {
  std::vector<uint16_t> blocks;
  random_bytes_engine rbe;
  blocks.resize(count * size / 2);
  std::generate(blocks.begin(), blocks.end(), std::ref(rbe));
  return blocks;
}
}  // namespace

using HashAlgorithm = Crypto::CNX::Hash_v1;

int main(int argc, char** argv) {
  try {
    using namespace ::XiBenchmark;

    cxxopts::Options cliOptions("xi-benchmark", "benchmarks the current cn-x hash algorithm implementation");
    CommonCLI::emplaceCLIOptions(cliOptions);

    bool silent = false;
    size_t threads = std::thread::hardware_concurrency();
    uint32_t blocks = 1000;
    std::string format{"table"};
    const uint32_t size = 410;
    // clang-format off
    cliOptions.add_options("benchmark")
        ("s,silent", "disables progress logging", cxxopts::value<bool>(silent)->implicit_value("true"))

        ("t,threads", "threads to use, 0 for benchmarking all variations",
            cxxopts::value<size_t>(threads)->default_value(std::to_string(threads)))

        ("b,blocks", "number of blocks to hash",
            cxxopts::value<uint32_t>(blocks)->default_value(std::to_string(blocks)))

        ("f,format", "summary format [csv|json|discord|table]",
            cxxopts::value<std::string>(format)->default_value(format))
    ;
    // clang-format on

    auto cliParseResult = cliOptions.parse(argc, argv);
    if (CommonCLI::handleCLIOptions(cliOptions, cliParseResult)) {
      return 0;
    }

    auto formatterSearch = formatters.find(format);
    if (formatterSearch == formatters.end()) {
      throw std::runtime_error{std::string{"unsupported output format: "} + format};
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
      logger(Logging::TRACE) << "starting benchmark using " << threads << " threads\n";
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
      logger(Logging::TRACE) << "Block Size       : " << size;
      logger(Logging::TRACE) << "Hashes Per Thread: " << blocks;
      logger(Logging::TRACE) << "Slowest          : " << result.worstDuration().count() << "ns";
      logger(Logging::TRACE) << "Best             : " << result.bestDuration().count() << "ns";
      logger(Logging::TRACE) << "Average          : " << result.averageDuration().count() << "ns";
      logger(Logging::TRACE) << "Total Hashes     : " << result.totalHashes();
      logger(Logging::TRACE) << "Slowest Hashrate : " << result.worstHashrate() << "H/s";
      logger(Logging::TRACE) << "Best    Hashrate : " << result.bestHashrate() << "H/s";
      logger(Logging::TRACE) << "Average Hashrate : " << result.averageHashrate() << "H/s";
      logger(Logging::TRACE) << "Total   Hashrate : " << result.totalHashrate() << "H/s\n\n";
    }

    logger() << formatterSearch->second(summary);
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  return 0;
}
