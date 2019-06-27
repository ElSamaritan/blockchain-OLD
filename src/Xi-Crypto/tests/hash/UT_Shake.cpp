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

#include <map>
#include <string>

#include <Xi/Crypto/Hash/Shake.hh>

#include "hash/OracleTest.hpp"

#define XI_TEST_SUITE Xi_Crypto_Hash_Shake

TEST(XI_TEST_SUITE, Hash128) {
  using Hash = Xi::Crypto::Hash::Shake::Hash128;
  static const std::vector<std::pair<std::string, Hash>> Oracle{
      {{"", Hash{{0x7f, 0x9c, 0x2b, 0xa4, 0xe8, 0x8f, 0x82, 0x7d, 0x61, 0x60, 0x45, 0x50, 0x76, 0x05, 0x85, 0x3e}}},
       {"iu6QiD1lpH",
        Hash{{0x22, 0x1c, 0x5c, 0xe6, 0xb7, 0xde, 0x92, 0x04, 0x20, 0x3c, 0xe3, 0x22, 0xa5, 0x4a, 0x4f, 0xdc}}},
       {"oPY1ZcUoPL",
        Hash{{0xb6, 0xf1, 0x7a, 0x22, 0x3d, 0x33, 0x65, 0xb4, 0xf1, 0xc6, 0xe9, 0xa1, 0x5e, 0xc3, 0xaf, 0x21}}},
       {"BXHHILmBXf",
        Hash{{0x3c, 0x24, 0xa6, 0x0e, 0xee, 0xfa, 0xa2, 0x06, 0x36, 0xd5, 0xf1, 0x87, 0x3d, 0x4f, 0xbf, 0x9a}}}

      }};
  XiCryptoTest::genericHashTest<Hash>(Oracle);
}

TEST(XI_TEST_SUITE, Hash256) {
  using Hash = Xi::Crypto::Hash::Shake::Hash256;
  static const std::vector<std::pair<std::string, Hash>> Oracle{
      {{"", Hash{{0x46, 0xb9, 0xdd, 0x2b, 0x0b, 0xa8, 0x8d, 0x13, 0x23, 0x3b, 0x3f, 0xeb, 0x74, 0x3e, 0xeb, 0x24,
                  0x3f, 0xcd, 0x52, 0xea, 0x62, 0xb8, 0x1b, 0x82, 0xb5, 0x0c, 0x27, 0x64, 0x6e, 0xd5, 0x76, 0x2f}}},
       {"iu6QiD1lpH",
        Hash{{0x62, 0xa6, 0xc9, 0x2a, 0x53, 0x9e, 0x01, 0x6f, 0xc0, 0x66, 0x6a, 0xe3, 0x71, 0x7a, 0xec, 0x6f,
              0x70, 0x63, 0xa2, 0x7a, 0x5b, 0xea, 0xa7, 0x32, 0x5a, 0xcd, 0x2c, 0xf6, 0x21, 0x77, 0xa4, 0x77}}},
       {"oPY1ZcUoPL",
        Hash{{0xdb, 0xd4, 0xda, 0x57, 0xbb, 0x95, 0x69, 0x6b, 0x44, 0x3d, 0xf3, 0xed, 0x6c, 0x7f, 0x1c, 0x21,
              0x71, 0xd4, 0xfe, 0x47, 0x38, 0x25, 0x2a, 0x4e, 0xe9, 0x2a, 0xd5, 0x9f, 0xb2, 0x5e, 0xb2, 0xa1}}},
       {"BXHHILmBXf",
        Hash{{0x6b, 0xb7, 0xd4, 0x8a, 0x46, 0x9c, 0x89, 0x87, 0x17, 0xb2, 0x9a, 0xb4, 0x96, 0xf6, 0xdf, 0xd7,
              0x21, 0x9b, 0xb3, 0x6f, 0x63, 0x9f, 0x69, 0xf2, 0xef, 0x59, 0x6a, 0xa8, 0xba, 0xa2, 0x31, 0x6a}}}}};
  XiCryptoTest::genericHashTest<Hash>(Oracle);
}
