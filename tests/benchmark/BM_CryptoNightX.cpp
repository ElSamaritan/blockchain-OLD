#include <benchmark/benchmark.h>

#include "HashBasedBenchmark.h"
#include "crypto/cnx/cnx.h"

using HashAlgorithm = Crypto::CNX::Hash_v0;

static void HeightArguments(benchmark::internal::Benchmark* b) {
  for (uint32_t i = 0; i <= 2 * HashAlgorithm::windowSize(); i += 128)
      b->Args({i});
}

BENCHMARK_DEFINE_F(HashBasedBenchmark, BM_CryptoNightX)(benchmark::State& state) {
  unsigned char const * data = HashBasedBenchmark::data();
  uint32_t height = static_cast<uint32_t>(state.range(0));
  state.counters["Height"] = height;
  for (auto _ : state)
  {
    (void)_;
    for(std::size_t i = 0; i < BlockCount; ++i)
      HashAlgorithm{}(data + i * BlockSize, BlockSize, HashPlaceholder, height);
  }
}

BENCHMARK_REGISTER_F(HashBasedBenchmark, BM_CryptoNightX)->Apply(HeightArguments)->Unit(benchmark::kMillisecond);
