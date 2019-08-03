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

#include <gmock/gmock.h>

#include <Xi/Algorithm/Math.h>
#include <Xi/Blockchain/Transaction/CanonicalAmount.hpp>

#define XI_UNIT_TEST_SUITE Xi_Blockchain_Transaction_CanoncialAmount

TEST(XI_UNIT_TEST_SUITE, EncodeDecode) {
  using namespace Xi;
  using namespace Xi::Blockchain::Transaction;

  std::vector<CanonicalAmount::value_type> values{};
  for (CanonicalAmount::value_type i = 1; i < 10; ++i) {
    values.push_back(std::numeric_limits<CanonicalAmount::value_type>::min() + i);
    values.push_back(pow64(10, 15) * i);
  }

  std::default_random_engine eng{};
  std::uniform_int_distribution<CanonicalAmount::value_type> indexDist{0, 15};
  std::uniform_int_distribution<CanonicalAmount::value_type> digitDist{1, 9};
  for (size_t i = 0; i < 20000; ++i) {
    values.push_back(pow64(10, indexDist(eng)) * digitDist(eng));
  }

  for (const auto i : values) {
    Byte encode = 0xFF;
    ASSERT_TRUE(CanonicalAmount::encode(i, encode));
    CanonicalAmount::value_type decode = std::numeric_limits<CanonicalAmount::value_type>::max();
    ASSERT_TRUE(CanonicalAmount::decode(encode, decode));
    EXPECT_EQ(i, decode);
  }
}
