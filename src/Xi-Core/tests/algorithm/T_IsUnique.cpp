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

#include <gmock/gmock.h>
#include <Xi/Algorithm/IsUnique.h>

#include <vector>

#define XI_TESTSUITE T_Xi_Algorithm_IsUnique

TEST(XI_TESTSUITE, EmptyContainerIsUnique) {
  using namespace ::testing;

  std::vector<char> vec;
  EXPECT_TRUE(Xi::Algorithm::is_unique(vec.begin(), vec.end()));
}

TEST(XI_TESTSUITE, NonUnique) {
  using namespace ::testing;

  std::vector<char> vec{{'a', 'b', 'e', 'z', 'e'}};
  EXPECT_FALSE(Xi::Algorithm::is_unique(vec.begin(), vec.end()));
}

TEST(XI_TESTSUITE, Unique) {
  using namespace ::testing;

  std::vector<char> vec{{'a', 'b', 'z', 'e'}};
  EXPECT_TRUE(Xi::Algorithm::is_unique(vec.begin(), vec.end()));
}
