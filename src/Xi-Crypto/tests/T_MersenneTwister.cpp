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

#include <Xi/Crypto/MersenneTwister.h>

#include <random>
#include <array>

#include <Xi/Utils/ExternalIncludePush.h>
#include <gmock/gmock.h>
#include <Xi/Utils/ExternalIncludePop.h>

#define XI_TESTSUITE T_Xi_Crypto_MersenneTwister

TEST(XI_TESTSUITE, ConsistentRandomValues) {
  std::default_random_engine eng;
  std::uniform_int_distribution<uint32_t> dist;

  for (size_t i = 0; i < 32; ++i) {
    const uint32_t seed = dist(eng);
    Xi::Crypto::MersenneTwister twister{seed};
    std::array<uint8_t, 64 * 1024> first;
    twister.nextBytes(first.data(), static_cast<uint32_t>(first.size()));
    twister.setSeed(seed);
    std::array<uint8_t, 64 * 1024> second;
    twister.nextBytes(second.data(), static_cast<uint32_t>(second.size()));
    EXPECT_THAT(first, ::testing::ContainerEq(second));
  }
}
