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
#include <cinttypes>

#include <gmock/gmock.h>

#include <Xi/Algorithm/Math.h>

#define XI_UNIT_TEST_SUITE Algorithm_Math

namespace {

template <typename _IntegerT>
void genericOverflowTest() {
  using namespace Xi;

  const auto max = std::numeric_limits<_IntegerT>::max();

  _IntegerT acc = 0;
  for (_IntegerT i = 1; i < 32; ++i) {
    EXPECT_FALSE(hasAdditionOverflow(max - i + 1, i - 1, &acc));
    EXPECT_EQ(acc, max);
    EXPECT_FALSE(hasAdditionOverflow(max - i, i, &acc));
    EXPECT_EQ(acc, max);
    EXPECT_TRUE(hasAdditionOverflow(max - i, i + 1, &acc));
  }
}

}  // namespace

TEST(XI_UNIT_TEST_SUITE, OverflowUInt32) { genericOverflowTest<uint32_t>(); }

TEST(XI_UNIT_TEST_SUITE, OverflowUInt64) { genericOverflowTest<uint64_t>(); }
