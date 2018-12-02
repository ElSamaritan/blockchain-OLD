#include <gtest/gtest.h>

#include <array>
#include <random>
#include <memory>
#include <climits>

#include "crypto/hash.h"
#include "crypto/cnx/cnx.h"

namespace  {
using random_bytes_engine = std::independent_bits_engine<
  std::default_random_engine, CHAR_BIT, uint16_t>;

using HashFn = Crypto::CNX::Hash_v0;
}

TEST(CryptoNightX, HashConsistency) {
  const uint8_t NumBlocks = 4;
  const uint8_t BlockSize = 76;
  auto data = std::make_unique<std::vector<uint16_t>>();
  random_bytes_engine rbe;
  data->resize(NumBlocks * BlockSize / 2);
  std::generate(data->begin(), data->end(), std::ref(rbe));

  for(uint32_t h = 0; h <= 2 * HashFn::windowSize(); ++h) {
    for(std::size_t i = 0; i < NumBlocks; ++i) {
      Crypto::Hash h0;
      HashFn{}(reinterpret_cast<uint8_t*>(data->data()) + i * BlockSize, BlockSize, h0, h);
      Crypto::Hash h1;
      HashFn{}(reinterpret_cast<uint8_t*>(data->data()) + i * BlockSize, BlockSize, h1, h);
      for(uint8_t k = 0; k < 32; ++k)
        EXPECT_EQ(h0.data[k], h1.data[k]);
    }
  }
}
