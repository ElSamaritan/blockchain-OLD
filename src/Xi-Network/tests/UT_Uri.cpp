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
#include <Xi/Network/Uri.hpp>

#include <string>

#define XI_TESTSUITE T_Xi_Network_Uri

TEST(XI_TESTSUITE, FromString) {
  using namespace ::testing;
  using namespace ::Xi::Network;

  {
    const auto uri = Uri::fromString("p2p://seeds.galaxia-project.com:40000");
    ASSERT_FALSE(uri.isError());
    EXPECT_EQ(uri->scheme(), "p2p");
    EXPECT_EQ(uri->host(), "seeds.galaxia-project.com");
    EXPECT_EQ(uri->port(), Port{40000});
  }
}
