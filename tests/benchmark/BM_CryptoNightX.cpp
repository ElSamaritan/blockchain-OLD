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

#include <benchmark/benchmark.h>

#include "HashBasedBenchmark.h"
#include "crypto/cnx/cnx.h"

using HashAlgorithm = Crypto::CNX::Hash_v1;

BENCHMARK_DEFINE_F(HashBasedBenchmark, BM_CryptoNightX)(benchmark::State& state) {
  unsigned char const* data = HashBasedBenchmark::data();
  for (auto _ : state) {
    (void)_;
    for (std::size_t i = 0; i < BlockCount; ++i) HashAlgorithm{}(data + i * BlockSize, BlockSize, HashPlaceholder);
  }
}

BENCHMARK_REGISTER_F(HashBasedBenchmark, BM_CryptoNightX)->Unit(benchmark::kMillisecond)->Iterations(2)->Threads(1);
