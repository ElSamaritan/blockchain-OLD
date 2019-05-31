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

#include <Xi/Global.hh>

bool XiBenchmark::serialize_cpu_features(const cpu_features::X86Features &features,
                                         CryptoNote::ISerializer &serializer) {
  bool aes = features.aes != 0;
  XI_RETURN_EC_IF_NOT(serializer(aes, "aes"), false);
  bool erms = features.erms != 0;
  XI_RETURN_EC_IF_NOT(serializer(erms, "erms"), false);
  bool f16c = features.f16c != 0;
  XI_RETURN_EC_IF_NOT(serializer(f16c, "f16c"), false);
  bool fma3 = features.fma3 != 0;
  XI_RETURN_EC_IF_NOT(serializer(fma3, "fma3"), false);
  bool vpclmulqdq = features.vpclmulqdq != 0;
  XI_RETURN_EC_IF_NOT(serializer(vpclmulqdq, "vpclmulqdq"), false);
  bool bmi1 = features.bmi1 != 0;
  XI_RETURN_EC_IF_NOT(serializer(bmi1, "bmi1"), false);
  bool bmi2 = features.bmi2 != 0;
  XI_RETURN_EC_IF_NOT(serializer(bmi2, "bmi2"), false);

  bool ssse3 = features.ssse3 != 0;
  XI_RETURN_EC_IF_NOT(serializer(ssse3, "ssse3"), false);
  bool sse4_1 = features.sse4_1 != 0;
  XI_RETURN_EC_IF_NOT(serializer(sse4_1, "sse4_1"), false);
  bool sse4_2 = features.sse4_2 != 0;
  XI_RETURN_EC_IF_NOT(serializer(sse4_2, "sse4_2"), false);

  bool avx = features.avx != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx, "avx"), false);
  bool avx2 = features.avx2 != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx2, "avx2"), false);

  bool avx512f = features.avx512f != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512f, "avx512f"), false);
  bool avx512cd = features.avx512cd != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512cd, "avx512cd"), false);
  bool avx512er = features.avx512er != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512er, "avx512er"), false);
  bool avx512pf = features.avx512pf != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512pf, "avx512pf"), false);
  bool avx512bw = features.avx512bw != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512bw, "avx512bw"), false);
  bool avx512dq = features.avx512dq != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512dq, "avx512dq"), false);
  bool avx512vl = features.avx512vl != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512vl, "avx512vl"), false);
  bool avx512ifma = features.avx512ifma != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512ifma, "avx512ifma"), false);
  bool avx512vbmi = features.avx512vbmi != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512vbmi, "avx512vbmi"), false);
  bool avx512vbmi2 = features.avx512vbmi2 != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512vbmi2, "avx512vbmi2"), false);
  bool avx512vnni = features.avx512vnni != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512vnni, "avx512vnni"), false);
  bool avx512bitalg = features.avx512bitalg != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512bitalg, "avx512bitalg"), false);
  bool avx512vpopcntdq = features.avx512vpopcntdq != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512vpopcntdq, "avx512vpopcntdq"), false);
  bool avx512_4vnniw = features.avx512_4vnniw != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512_4vnniw, "avx512_4vnniw"), false);
  bool avx512_4vbmi2 = features.avx512_4vbmi2 != 0;
  XI_RETURN_EC_IF_NOT(serializer(avx512_4vbmi2, "avx512_4vbmi2"), false);

  bool smx = features.smx != 0;
  XI_RETURN_EC_IF_NOT(serializer(smx, "smx"), false);
  bool sgx = features.sgx != 0;
  XI_RETURN_EC_IF_NOT(serializer(sgx, "sgx"), false);
  bool cx16 = features.cx16 != 0;
  XI_RETURN_EC_IF_NOT(serializer(cx16, "cx16"), false);
  bool sha = features.sha != 0;
  XI_RETURN_EC_IF_NOT(serializer(sha, "sha"), false);
  bool popcnt = features.popcnt != 0;
  XI_RETURN_EC_IF_NOT(serializer(popcnt, "popcnt"), false);
  bool movbe = features.movbe != 0;
  XI_RETURN_EC_IF_NOT(serializer(movbe, "movbe"), false);
  bool rdrnd = features.rdrnd != 0;
  XI_RETURN_EC_IF_NOT(serializer(rdrnd, "rdrnd"), false);
  return true;
}

bool XiBenchmark::serialize_cpu_info(const cpu_features::X86Info &info, CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.beginObject("features"), false);
  XI_RETURN_EC_IF_NOT(serialize_cpu_features(info.features, serializer), false);
  serializer.endObject();
  int family = info.family;
  XI_RETURN_EC_IF_NOT(serializer(family, "family"), false);
  int model = info.model;
  XI_RETURN_EC_IF_NOT(serializer(model, "model"), false);
  int stepping = info.stepping;
  XI_RETURN_EC_IF_NOT(serializer(stepping, "stepping"), false);
  std::string vendor{info.vendor};
  XI_RETURN_EC_IF_NOT(serializer(vendor, "vendor"), false);
  return true;
}

bool XiBenchmark::serialize_benchmark_result(const XiBenchmark::BenchmarkResult &result,
                                             CryptoNote::ISerializer &serializer) {
  std::string algorithm = result.Algorithm;
  XI_RETURN_EC_IF_NOT(serializer(algorithm, "algorithm"), false);
  uint32_t blocks = result.Blocks;
  XI_RETURN_EC_IF_NOT(serializer(blocks, "blocks"), false);
  uint32_t size = result.Size;
  XI_RETURN_EC_IF_NOT(serializer(size, "size"), false);
  int64_t bestDuration = result.bestDuration().count();
  XI_RETURN_EC_IF_NOT(serializer(bestDuration, "best_duration"), false);
  int64_t worstDuration = result.worstDuration().count();
  XI_RETURN_EC_IF_NOT(serializer(worstDuration, "worst_duration"), false);
  int64_t averageDuration = result.averageDuration().count();
  XI_RETURN_EC_IF_NOT(serializer(averageDuration, "average_duration"), false);
  int64_t totalDuration = result.totalDuration().count();
  XI_RETURN_EC_IF_NOT(serializer(totalDuration, "total_duration"), false);
  size_t threads = result.ThreadResults.size();
  XI_RETURN_EC_IF_NOT(serializer(threads, "threads"), false);
  size_t totalHashes = result.totalHashes();
  XI_RETURN_EC_IF_NOT(serializer(totalHashes, "total_hashes"), false);
  double bestHashrate = result.bestHashrate();
  XI_RETURN_EC_IF_NOT(serializer(bestHashrate, "best_hashrate"), false);
  double worstHashrate = result.worstHashrate();
  XI_RETURN_EC_IF_NOT(serializer(worstHashrate, "worst_hashrate"), false);
  double averageHashrate = result.averageHashrate();
  XI_RETURN_EC_IF_NOT(serializer(averageHashrate, "average_hashrate"), false);
  double totalHashrate = result.totalHashrate();
  XI_RETURN_EC_IF_NOT(serializer(totalHashrate, "total_hashrate"), false);

  XI_RETURN_EC_IF_NOT(serializer.beginArray(threads, "thread_results"), false);
  for (const auto &iThreadResult : result.ThreadResults) {
    XI_RETURN_EC_IF_NOT(serializer.beginObject(""), false);
    int64_t duration = iThreadResult.Duration.count();
    XI_RETURN_EC_IF_NOT(serializer(duration, "duration"), false);
    double hashrate = to_hashrate(iThreadResult.Duration, result.Blocks);
    XI_RETURN_EC_IF_NOT(serializer(hashrate, "hashrate"), false);
    serializer.endObject();
  }
  serializer.endArray();
  return true;
}

bool XiBenchmark::serialize_benchmark_summary(const XiBenchmark::BencharkSummary &summary,
                                              CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer.beginObject("cpu_info"), false);
  XI_RETURN_EC_IF_NOT(serialize_cpu_info(summary.CPUInfo, serializer), false);
  serializer.endObject();
  std::string arch = summary.Architecture;
  XI_RETURN_EC_IF_NOT(serializer(arch, "architecture"), false);

  size_t runs = summary.Benchmarks.size();
  XI_RETURN_EC_IF_NOT(serializer.beginArray(runs, "benchmarks"), false);
  for (const auto &benchmark : summary.Benchmarks) {
    XI_RETURN_EC_IF_NOT(serializer.beginObject(""), false);
    XI_RETURN_EC_IF_NOT(serialize_benchmark_result(benchmark, serializer), false);
    serializer.endObject();
  }
  serializer.endArray();
  return true;
}
