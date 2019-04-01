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

#include <random>
#include <unordered_set>

#include <crypto/CryptoTypes.h>
#include <crypto/crypto.h>
#include <CryptoNoteCore/CryptoNoteBasic.h>

#define XI_TESTSUITE UT_Xi_Core_Time

TEST(XI_TESTSUITE, SingleDurations) {
  using namespace ::testing;

  EXPECT_EQ(parseDuration("1ms").valueOrThrow(), std::chrono::milliseconds{1});
  EXPECT_EQ(parseDuration("-12 hours").valueOrThrow(), std::chrono::hours{-12});
  EXPECT_EQ(parseDuration(" -277 microseconds   ").valueOrThrow(), std::chrono::microseconds{-277});
  EXPECT_EQ(parseDuration(" 33 days").valueOrThrow(), std::chrono::hours{33 * 24});
  EXPECT_EQ(parseDuration("4W").valueOrThrow(), std::chrono::hours{4 * 7 * 24});
}

TEST(XI_TESTSUITE, CompoundDurations) {
  using namespace ::testing;
  using namespace ::Xi::Time;
}
