﻿/* ============================================================================================== *
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

#include <Xi/Crypto/Hash/Sha2.hh>

#include "hash/OracleTest.hpp"

#define XI_TEST_SUITE Xi_Crypto_Hash_Sha2

TEST(XI_TEST_SUITE, Hash224) {
  using Hash = Xi::Crypto::Hash::Sha2::Hash224;
  static const std::vector<std::pair<std::string, Hash>> Oracle{
      {"", Hash{{0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9, 0x47, 0x61, 0x02, 0xbb, 0x28, 0x82,
                 0x34, 0xc4, 0x15, 0xa2, 0xb0, 0x1f, 0x82, 0x8e, 0xa6, 0x2a, 0xc5, 0xb3, 0xe4, 0x2f}}},
      {"iu6QiD1lpH", Hash{{0xc3, 0x23, 0xee, 0x00, 0xbe, 0x7e, 0x00, 0xb4, 0x02, 0x75, 0x77, 0xff, 0x82, 0xfc,
                           0xf3, 0xc7, 0x23, 0x18, 0x26, 0x67, 0x3c, 0xb3, 0x9e, 0xf1, 0x21, 0xf0, 0x48, 0x14}}},
      {"oPY1ZcUoPL", Hash{{0xda, 0xcb, 0xb3, 0xa1, 0x15, 0x77, 0xc0, 0x7e, 0xac, 0xa2, 0x6e, 0x86, 0x99, 0x1c,
                           0x4f, 0x4a, 0x85, 0xd3, 0xd1, 0x06, 0x78, 0x84, 0xb6, 0xe4, 0x6d, 0x12, 0xa3, 0x57}}},
      {"BXHHILmBXf", Hash{{0x32, 0x4d, 0xd8, 0x10, 0xfb, 0x7c, 0xf3, 0x15, 0xd6, 0xd1, 0x0f, 0xaf, 0xbe, 0x7c,
                           0xa4, 0x24, 0xc1, 0x52, 0x4f, 0x7b, 0x1b, 0x19, 0x21, 0x6e, 0xd8, 0x40, 0xb5, 0xea}}}};
  XiCryptoTest::genericHashTest<Hash>(Oracle);
}

TEST(XI_TEST_SUITE, Hash256) {
  using Hash = Xi::Crypto::Hash::Sha2::Hash256;
  static const std::vector<std::pair<std::string, Hash>> Oracle{
      {"", Hash{{
               0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
               0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55,
           }}},
      {"iu6QiD1lpH",
       Hash{{0x27, 0x43, 0x27, 0x53, 0xbd, 0x9c, 0xa1, 0xf7, 0xe4, 0x32, 0x15, 0x1f, 0x3f, 0x37, 0x7f, 0x5f,
             0x59, 0xfa, 0xbf, 0xdd, 0xc6, 0xb7, 0xe1, 0x14, 0xb9, 0xf0, 0x2a, 0x62, 0xd5, 0x76, 0x5c, 0xb1}}},
      {"oPY1ZcUoPL",
       Hash{{0xa0, 0xf8, 0x29, 0x3d, 0x04, 0x3b, 0xa7, 0x59, 0xae, 0x5e, 0x15, 0x50, 0x02, 0xfe, 0xe1, 0xa5,
             0xd5, 0x20, 0xb1, 0x7f, 0x43, 0xca, 0xa4, 0x6e, 0x84, 0x36, 0xc6, 0x30, 0xc1, 0x0d, 0x70, 0x8b}}},
      {"BXHHILmBXf",
       Hash{{0xc5, 0xca, 0xce, 0x4b, 0x89, 0x3b, 0x1b, 0xf1, 0x9f, 0xaf, 0x81, 0xc7, 0x9d, 0xb9, 0xcb, 0xac,
             0x70, 0xad, 0x72, 0xa4, 0x68, 0x4d, 0xfc, 0x45, 0xe5, 0xf9, 0x5d, 0xd7, 0x51, 0x0e, 0x98, 0xc0}}}};
  XiCryptoTest::genericHashTest<Hash>(Oracle);
}

TEST(XI_TEST_SUITE, Hash384) {
  using Hash = Xi::Crypto::Hash::Sha2::Hash384;
  static const std::vector<std::pair<std::string, Hash>> Oracle{
      {"", Hash{{
               0x38, 0xb0, 0x60, 0xa7, 0x51, 0xac, 0x96, 0x38, 0x4c, 0xd9, 0x32, 0x7e, 0xb1, 0xb1, 0xe3, 0x6a,
               0x21, 0xfd, 0xb7, 0x11, 0x14, 0xbe, 0x07, 0x43, 0x4c, 0x0c, 0xc7, 0xbf, 0x63, 0xf6, 0xe1, 0xda,
               0x27, 0x4e, 0xde, 0xbf, 0xe7, 0x6f, 0x65, 0xfb, 0xd5, 0x1a, 0xd2, 0xf1, 0x48, 0x98, 0xb9, 0x5b,
           }}},
      {"iu6QiD1lpH",
       Hash{{0x65, 0xe2, 0x6f, 0x68, 0x50, 0x8f, 0xb6, 0x9c, 0xbb, 0x0f, 0xfa, 0xde, 0x0f, 0x6e, 0x32, 0xee,
             0x85, 0x07, 0x04, 0x6d, 0x71, 0x51, 0xd8, 0x13, 0x78, 0xdf, 0xf5, 0x94, 0x3d, 0x01, 0xd8, 0x85,
             0x55, 0x71, 0x68, 0x9c, 0x98, 0x45, 0x36, 0x76, 0x57, 0x14, 0x2e, 0x50, 0x7e, 0x66, 0xce, 0x1b}}},
      {"oPY1ZcUoPL",
       Hash{{0xc3, 0x81, 0x9b, 0x5d, 0x60, 0x9b, 0x06, 0x2a, 0xd2, 0xf1, 0xae, 0x31, 0x8a, 0x69, 0x34, 0xe2,
             0xde, 0x89, 0x52, 0x50, 0xa5, 0x17, 0xa3, 0xce, 0x37, 0x7e, 0xd5, 0xc2, 0xe2, 0x28, 0x32, 0x6c,
             0xcf, 0x3f, 0x2a, 0x92, 0x36, 0x88, 0xa8, 0x12, 0x3a, 0x63, 0x9d, 0x8b, 0x28, 0x07, 0x2b, 0xc8}}},
      {"BXHHILmBXf",
       Hash{{0x65, 0xe3, 0x55, 0x85, 0x3a, 0x5e, 0xf7, 0x8d, 0x10, 0x6b, 0x67, 0xbc, 0xd6, 0xac, 0x86, 0xff,
             0x55, 0x28, 0x98, 0x11, 0x67, 0xf4, 0x37, 0xbb, 0x89, 0x56, 0x96, 0x27, 0x08, 0x72, 0x76, 0xdf,
             0xcd, 0x3b, 0xa7, 0xd2, 0x5c, 0x8c, 0x9d, 0x64, 0xf6, 0x2b, 0xb3, 0xef, 0xaf, 0xf4, 0xdb, 0x30}}}};
  XiCryptoTest::genericHashTest<Hash>(Oracle);
}

TEST(XI_TEST_SUITE, Hash512) {
  using Hash = Xi::Crypto::Hash::Sha2::Hash512;
  static const std::vector<std::pair<std::string, Hash>> Oracle{
      {"", Hash{{
               0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd, 0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
               0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc, 0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
               0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0, 0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
               0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81, 0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e,
           }}},
      {"iu6QiD1lpH",
       Hash{{0xea, 0xd3, 0xe5, 0xae, 0xdf, 0x8c, 0x11, 0xd1, 0xa6, 0x91, 0x3d, 0x25, 0xdf, 0x32, 0xba, 0x68,
             0xcc, 0x5b, 0xd8, 0x74, 0xd8, 0x0f, 0xe8, 0xb0, 0xbb, 0x64, 0x9a, 0x93, 0x7a, 0xa3, 0x76, 0x89,
             0xf5, 0x07, 0x82, 0xa3, 0xeb, 0x93, 0xed, 0x37, 0x2d, 0xa3, 0x0a, 0xa8, 0xf3, 0xfd, 0x35, 0x5f,
             0xfd, 0x18, 0x43, 0xca, 0x69, 0x51, 0x6d, 0x7b, 0xd1, 0x85, 0x09, 0xc7, 0x1c, 0xa8, 0xe1, 0x44}}},
      {"oPY1ZcUoPL",
       Hash{{0x41, 0xda, 0x4e, 0xd2, 0xa9, 0x70, 0x26, 0xd7, 0x11, 0x26, 0x7b, 0x53, 0x4c, 0x8b, 0x7c, 0x0f,
             0x26, 0xca, 0x3e, 0x62, 0xf1, 0xd6, 0xae, 0x87, 0x48, 0x98, 0xd9, 0xe2, 0xb0, 0x49, 0x52, 0x18,
             0xa9, 0xf1, 0x94, 0xb7, 0x3c, 0x0e, 0xc4, 0x75, 0x70, 0xeb, 0x8b, 0x04, 0x80, 0x2a, 0x98, 0xb1,
             0x34, 0x29, 0x6a, 0x77, 0x78, 0x68, 0x65, 0xbe, 0xef, 0xdf, 0x74, 0x11, 0x49, 0xac, 0x9a, 0x9e}}},
      {"BXHHILmBXf",
       Hash{{0x22, 0x30, 0xb2, 0xb4, 0xf0, 0x1c, 0x37, 0x7c, 0x05, 0x71, 0x80, 0x30, 0x29, 0x52, 0xe1, 0xb9,
             0x40, 0x14, 0x15, 0xd5, 0x96, 0x8a, 0xa2, 0xad, 0x8f, 0x7f, 0xf8, 0x23, 0x97, 0xb5, 0x85, 0x28,
             0xfe, 0x19, 0x79, 0x75, 0x57, 0x85, 0x8e, 0x5d, 0x50, 0x2f, 0x90, 0xb9, 0x38, 0xdd, 0x63, 0x7c,
             0xde, 0x9d, 0xfa, 0x10, 0xa6, 0x94, 0xdc, 0xb0, 0xc9, 0x02, 0x14, 0x9b, 0xb0, 0x39, 0x4c, 0x2f}}}};
  XiCryptoTest::genericHashTest<Hash>(Oracle);
}
