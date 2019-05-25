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
#include <Xi/Http/Response.h>

#define XI_TESTSUITE T_Xi_Http_Response

TEST(XI_TESTSUITE, Redirection) {
  using namespace ::testing;
  using namespace ::Xi::Http;

  Response res{};
  res.setStatus(StatusCode::TemporaryRedirect);
  EXPECT_TRUE(res.isRedirection());
  res.setStatus(StatusCode::PermanentRedirect);
  EXPECT_TRUE(res.isRedirection());
  res.setStatus(StatusCode::Found);
  EXPECT_TRUE(res.isRedirection());
  res.setStatus(StatusCode::SeeOther);
  EXPECT_TRUE(res.isRedirection());

  res.setStatus(StatusCode::NotFound);
  EXPECT_FALSE(res.isRedirection());
  res.setStatus(StatusCode::Ok);
  EXPECT_FALSE(res.isRedirection());
}
