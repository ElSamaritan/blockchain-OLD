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

#include <Xi/Algorithm/Merge.hpp>

#define XI_UNIT_TEST_SUITE Xi_Algorithm_Merge

TEST(XI_UNIT_TEST_SUITE, DetectsAlreadyPresentKeys) {
  using namespace Xi;

  std::set<uint32_t> set{{1, 7, 22, 731}};

  {
    auto setCopy = set;
    auto setInsertion = set;
    EXPECT_EQ(merge(setCopy, std::move(setInsertion)), 0ULL);
  }

  {
    auto setCopy = set;
    std::set<uint32_t> setInsertion{{7, 1, 13}};
    EXPECT_EQ(merge(setCopy, std::move(setInsertion)), 1ULL);
  }
}

TEST(XI_UNIT_TEST_SUITE, EmptySetMerges) {
  using namespace Xi;

  std::set<uint32_t> set{{1, 7, 22, 731}};

  {
    auto setCopy = set;
    std::set<uint32_t> setInsertion{};
    EXPECT_EQ(merge(setCopy, std::move(setInsertion)), 0ULL);
  }

  {
    std::set<uint32_t> empty{};
    auto setInsertion = set;
    EXPECT_EQ(merge(empty, std::move(setInsertion)), set.size());
  }
}
