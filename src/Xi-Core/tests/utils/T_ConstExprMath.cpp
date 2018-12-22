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

#include <Xi/Utils/ConstExprMath.h>
#include <gmock/gmock.h>

TEST(T_Xi_Utils_ConstExprMath, pow) {
  using namespace ::testing;
  using namespace ::Xi;

  EXPECT_THAT(Xi::pow(100u, 0u), Eq(1u));
  EXPECT_THAT(Xi::pow(100u, 2u), Eq(10000u));
  EXPECT_THAT(Xi::pow(2u, 8u), Eq(256u));
  EXPECT_THAT(Xi::pow(4u, 4u), Eq(256u));
}
