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
#include <Xi/Encoding/Base64.h>

#include <string>

#define XI_TESTSUITE T_Xi_Encoding_Base64

namespace {
const std::string DecodedString{"This is a test string for Xi."};
const std::string EncodedString{"VGhpcyBpcyBhIHRlc3Qgc3RyaW5nIGZvciBYaS4="};
}  // namespace

TEST(XI_TESTSUITE, Encode) {
  using namespace ::testing;

  EXPECT_THAT(Xi::Base64::encode(DecodedString), Eq(EncodedString));
}

TEST(XI_TESTSUITE, Decode) {
  using namespace ::testing;

  EXPECT_THAT(Xi::Base64::decode(EncodedString), Eq(DecodedString));
}
