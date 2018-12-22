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

#include <gmock/gmock.h>
#include <Xi/Http/ContentType.h>

#include <string>

#define XI_TESTSUITE T_Xi_Http_ContentType

TEST(XI_TESTSUITE, ToString) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_ANY_THROW(to_string(static_cast<Http::ContentType>(-1)));
  EXPECT_THAT(to_string(Http::ContentType::Json), Eq("application/json"));
}

TEST(XI_TESTSUITE, LexicalCast) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_ANY_THROW(lexical_cast<Http::ContentType>(""));
  EXPECT_ANY_THROW(lexical_cast<Http::ContentType>("Application/json"));
  EXPECT_ANY_THROW(lexical_cast<Http::ContentType>(" application/json"));
  EXPECT_ANY_THROW(lexical_cast<Http::ContentType>("application/json "));
  EXPECT_EQ(lexical_cast<Http::ContentType>("application/json"), Http::ContentType::Json);
}
