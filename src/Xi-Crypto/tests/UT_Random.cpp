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

#include <gmock/gmock.h>

#include <thread>
#include <algorithm>

#include <Xi/Crypto/Random.hh>

#define XI_TEST_SUITE Xi_Crypto_Random

TEST(XI_TEST_SUITE, Generate) {
  using namespace ::testing;
  using namespace Xi::Crypto::Random;

  {
    Xi::ByteArray<74> bytes;
    generate(bytes);
    EXPECT_THAT(bytes, Contains(Ne(0)));
  }

  {
    Xi::ByteArray<0> bytes;
    generate(bytes);
  }

  for (size_t i = 0; i < 16; ++i) {
    size_t count = 1ull << i;
    auto vecByte = generate(count);
    EXPECT_EQ(vecByte.size(), count);
  }

  {
    Xi::ByteArray<512> first, second;
    auto generateArray = [](auto &buffer) { generate(buffer); };
    std::thread t1{std::bind(generateArray, std::ref(first))};
    std::thread t2{std::bind(generateArray, std::ref(second))};
    t1.join();
    t2.join();
    EXPECT_NE(first, second);
  }
}

TEST(XI_TEST_SUITE, GenerateDetermenistic) {
  using namespace ::testing;
  using namespace Xi::Crypto::Random;

  Xi::ByteArray<74> bytes;
  generate(bytes);
  EXPECT_THAT(bytes, Contains(Ne(0)));

  Xi::ByteArray<512> first, second;
  for (size_t i = 0; i < 10; ++i) {
    auto generateArray = [&bytes](auto &buffer) { generate(buffer, bytes); };
    std::thread t1{std::bind(generateArray, std::ref(first))};
    std::thread t2{std::bind(generateArray, std::ref(second))};
    t1.join();
    t2.join();
    EXPECT_EQ(first, second);
  }
}
