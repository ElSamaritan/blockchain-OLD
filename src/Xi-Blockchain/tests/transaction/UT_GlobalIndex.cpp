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

#include <limits>
#include <random>
#include <vector>
#include <algorithm>

#include <gmock/gmock.h>

#include <Xi/Blockchain/Transaction/GlobalIndex.hpp>

#define XI_UNIT_TEST_SUITE Xi_Blockchain_Transaction_GlobalIndex

TEST(XI_UNIT_TEST_SUITE, EncodeDecode) {
  using namespace Xi;
  using namespace Xi::Blockchain::Transaction;

  std::default_random_engine eng{};
  std::uniform_int_distribution<GlobalIndex> indexDist{0, std::numeric_limits<GlobalIndex>::max()};

  for (size_t i = 0; i < 20; ++i) {
    for (size_t j = 0; j < i; ++j) {
      GlobalIndexVector values{};
      for (size_t k = 0; k < j; ++k) {
        values.push_back(indexDist(eng));
      }

      GlobalDeltaIndexVector encoded{};
      ASSERT_TRUE(deltaEncodeGlobalIndices(values, encoded));
      std::sort(values.begin(), values.end());
      GlobalIndexVector decoded{};
      ASSERT_TRUE(deltaDecodeGlobalIndices(encoded, decoded));
      EXPECT_EQ(values, decoded);

      if (j > 0) {
        const auto offset = encoded.front();
        encoded.front() -= offset;

        GlobalIndexVector offsetDecoded{};
        ASSERT_TRUE(deltaDecodeGlobalIndices(encoded, offsetDecoded, offset));
        EXPECT_EQ(values, offsetDecoded);
      }
    }
  }
}
