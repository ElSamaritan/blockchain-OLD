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
#include <Xi/Http/Method.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_Method

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_NO_THROW(to_string(static_cast<Http::Method>(-1)));
  EXPECT_THAT(to_string(static_cast<Http::Method>(-1)), Eq("UNKNOWN"));
  EXPECT_THAT(to_string(Xi::Http::Method::Get), Eq("GET"));
  EXPECT_THAT(to_string(Xi::Http::Method::Post), Eq("POST"));
  EXPECT_THAT(to_string(Xi::Http::Method::Put), Eq("PUT"));
}
