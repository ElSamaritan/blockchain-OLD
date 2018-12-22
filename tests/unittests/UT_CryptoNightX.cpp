/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include <gtest/gtest.h>

#include <array>
#include <random>
#include <memory>
#include <climits>

#include "crypto/hash.h"
#include "crypto/cnx/cnx.h"

namespace {
using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, uint16_t>;

using HashFn = Crypto::CNX::Hash_v0;
}  // namespace

TEST(CryptoNightX, HashConsistency) {
  const uint8_t NumBlocks = 4;
  const uint8_t BlockSize = 76;
  auto data = std::make_unique<std::vector<uint16_t>>();
  random_bytes_engine rbe;
  data->resize(NumBlocks * BlockSize / 2);
  std::generate(data->begin(), data->end(), std::ref(rbe));

  const uint32_t DesiredIterations = 16;
  const uint32_t WaveLength = 2 * HashFn::windowSize();
  const uint32_t Incrementor = WaveLength / DesiredIterations;
  for (uint32_t h = 0; h <= WaveLength; h += Incrementor) {
    for (std::size_t i = 0; i < NumBlocks; ++i) {
      Crypto::Hash h0;
      HashFn{}(reinterpret_cast<uint8_t*>(data->data()) + i * BlockSize, BlockSize, h0, h);
      Crypto::Hash h1;
      HashFn{}(reinterpret_cast<uint8_t*>(data->data()) + i * BlockSize, BlockSize, h1, h);
      for (uint8_t k = 0; k < 32; ++k) EXPECT_EQ(h0.data[k], h1.data[k]);
    }
  }
}
