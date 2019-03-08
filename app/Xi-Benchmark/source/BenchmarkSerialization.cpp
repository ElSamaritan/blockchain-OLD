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

#include "BenchmarkSerialization.h"

void XiBenchmark::serialize_cpu_features(const cpu_features::X86Features &features,
                                         CryptoNote::ISerializer &serializer) {
  bool aes = features.aes != 0;
  serializer(aes, "aes");
  bool erms = features.erms != 0;
  serializer(erms, "erms");
  bool f16c = features.f16c != 0;
  serializer(f16c, "f16c");
  bool fma3 = features.fma3 != 0;
  serializer(fma3, "fma3");
  bool vpclmulqdq = features.vpclmulqdq != 0;
  serializer(vpclmulqdq, "vpclmulqdq");
  bool bmi1 = features.bmi1 != 0;
  serializer(bmi1, "bmi1");
  bool bmi2 = features.bmi2 != 0;
  serializer(bmi2, "bmi2");

  bool ssse3 = features.ssse3 != 0;
  serializer(ssse3, "ssse3");
  bool sse4_1 = features.sse4_1 != 0;
  serializer(sse4_1, "sse4_1");
  bool sse4_2 = features.sse4_2 != 0;
  serializer(sse4_2, "sse4_2");

  bool avx = features.avx != 0;
  serializer(avx, "avx");
  bool avx2 = features.avx2 != 0;
  serializer(avx2, "avx2");

  bool avx512f = features.avx512f != 0;
  serializer(avx512f, "avx512f");
  bool avx512cd = features.avx512cd != 0;
  serializer(avx512cd, "avx512cd");
  bool avx512er = features.avx512er != 0;
  serializer(avx512er, "avx512er");
  bool avx512pf = features.avx512pf != 0;
  serializer(avx512pf, "avx512pf");
  bool avx512bw = features.avx512bw != 0;
  serializer(avx512bw, "avx512bw");
  bool avx512dq = features.avx512dq != 0;
  serializer(avx512dq, "avx512dq");
  bool avx512vl = features.avx512vl != 0;
  serializer(avx512vl, "avx512vl");
  bool avx512ifma = features.avx512ifma != 0;
  serializer(avx512ifma, "avx512ifma");
  bool avx512vbmi = features.avx512vbmi != 0;
  serializer(avx512vbmi, "avx512vbmi");
  bool avx512vbmi2 = features.avx512vbmi2 != 0;
  serializer(avx512vbmi2, "avx512vbmi2");
  bool avx512vnni = features.avx512vnni != 0;
  serializer(avx512vnni, "avx512vnni");
  bool avx512bitalg = features.avx512bitalg != 0;
  serializer(avx512bitalg, "avx512bitalg");
  bool avx512vpopcntdq = features.avx512vpopcntdq != 0;
  serializer(avx512vpopcntdq, "avx512vpopcntdq");
  bool avx512_4vnniw = features.avx512_4vnniw != 0;
  serializer(avx512_4vnniw, "avx512_4vnniw");
  bool avx512_4vbmi2 = features.avx512_4vbmi2 != 0;
  serializer(avx512_4vbmi2, "avx512_4vbmi2");

  bool smx = features.smx != 0;
  serializer(smx, "smx");
  bool sgx = features.sgx != 0;
  serializer(sgx, "sgx");
  bool cx16 = features.cx16 != 0;
  serializer(cx16, "cx16");
  bool sha = features.sha != 0;
  serializer(sha, "sha");
  bool popcnt = features.popcnt != 0;
  serializer(popcnt, "popcnt");
  bool movbe = features.movbe != 0;
  serializer(movbe, "movbe");
  bool rdrnd = features.rdrnd != 0;
  serializer(rdrnd, "rdrnd");
}

void XiBenchmark::serialize_cpu_info(const cpu_features::X86Info &info, CryptoNote::ISerializer &serializer) {
  serializer.beginObject("features");
  serialize_cpu_features(info.features, serializer);
  serializer.endObject();
  int family = info.family;
  serializer(family, "family");
  int model = info.model;
  serializer(model, "model");
  int stepping = info.stepping;
  serializer(stepping, "stepping");
  std::string vendor{info.vendor};
  serializer(vendor, "vendor");
}

void XiBenchmark::serialize_benchmark_result(const XiBenchmark::BenchmarkResult &result,
                                             CryptoNote::ISerializer &serializer) {
  std::string algorithm = result.Algorithm;
  serializer(algorithm, "algorithm");
  uint32_t blocks = result.Blocks;
  serializer(blocks, "blocks");
  uint32_t size = result.Size;
  serializer(size, "size");
  int64_t bestDuration = result.bestDuration().count();
  serializer(bestDuration, "bestDuration");
  int64_t worstDuration = result.worstDuration().count();
  serializer(worstDuration, "worstDuration");
  int64_t averageDuration = result.averageDuration().count();
  serializer(averageDuration, "averageDuration");
  int64_t totalDuration = result.totalDuration().count();
  serializer(totalDuration, "totalDuration");
  size_t threads = result.ThreadResults.size();
  serializer(threads, "threads");
  size_t totalHashes = result.totalHashes();
  serializer(totalHashes, "totalHashes");
  double bestHashrate = result.bestHashrate();
  serializer(bestHashrate, "bestHashrate");
  double worstHashrate = result.worstHashrate();
  serializer(worstHashrate, "worstHashrate");
  double averageHashrate = result.averageHashrate();
  serializer(averageHashrate, "averageHashrate");
  double totalHashrate = result.totalHashrate();
  serializer(totalHashrate, "totalHashrate");

  serializer.beginArray(threads, "threadResults");
  for (const auto &iThreadResult : result.ThreadResults) {
    serializer.beginObject("");
    int64_t duration = iThreadResult.Duration.count();
    serializer(duration, "duration");
    double hashrate = to_hashrate(iThreadResult.Duration, result.Blocks);
    serializer(hashrate, "hashrate");
    serializer.endObject();
  }
  serializer.endArray();
}

void XiBenchmark::serialize_benchmark_summary(const XiBenchmark::BencharkSummary &summary,
                                              CryptoNote::ISerializer &serializer) {
  serializer.beginObject("cpuInfo");
  serialize_cpu_info(summary.CPUInfo, serializer);
  serializer.endObject();
  std::string arch = summary.Architecture;
  serializer(arch, "architecture");

  size_t runs = summary.Benchmarks.size();
  serializer.beginArray(runs, "benchmarks");
  for (const auto &benchmark : summary.Benchmarks) {
    serializer.beginObject("");
    serialize_benchmark_result(benchmark, serializer);
    serializer.endObject();
  }
  serializer.endArray();
}
