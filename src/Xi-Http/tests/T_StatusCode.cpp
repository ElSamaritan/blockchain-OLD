﻿/* ============================================================================================== *
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

#include <gmock/gmock.h>
#include <Xi/Http/StatusCode.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_StatusCode

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_NO_THROW(to_string(static_cast<Http::StatusCode>(-1)));
  EXPECT_THAT(to_string(static_cast<Http::StatusCode>(-1)), Eq("Unknown"));
  EXPECT_THAT(to_string(Xi::Http::StatusCode::Gone), Eq("Gone"));
  EXPECT_THAT(to_string(Xi::Http::StatusCode::NetworkConnectTimeout), Eq("NetworkConnectTimeout"));
}