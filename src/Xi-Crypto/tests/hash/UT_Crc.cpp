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

#include <Xi/Crypto/Hash/Crc.hpp>

#include "hash/OracleTest.hpp"

#define XI_TEST_SUITE Xi_Crypto_Hash_Crc

TEST(XI_TEST_SUITE, Hash16) {
  using Hash = Xi::Crypto::Hash::Crc::Hash16;
  static const std::vector<std::pair<std::string, Hash>> Oracle{{{"", Hash{{0x00, 0x00}}},
                                                                 {"iu6QiD1lpH", Hash{{0xd9, 0x58}}},
                                                                 {"oPY1ZcUoPL", Hash{{0xce, 0xfe}}},
                                                                 {"BXHHILmBXf", Hash{{0x8a, 0xed}}}}};
  XiCryptoTest::genericHashTest<Hash>(Oracle);
}

TEST(XI_TEST_SUITE, Hash32) {
  using Hash = Xi::Crypto::Hash::Crc::Hash32;
  static const std::vector<std::pair<std::string, Hash>> Oracle{{{"", Hash{{0x00, 0x00, 0x00, 0x00}}},
                                                                 {"iu6QiD1lpH", Hash{{0x6e, 0xa6, 0xc4, 0x60}}},
                                                                 {"oPY1ZcUoPL", Hash{{0xfc, 0x47, 0x35, 0x98}}},
                                                                 {"BXHHILmBXf", Hash{{0xe4, 0x66, 0x35, 0x6e}}}}};
  XiCryptoTest::genericHashTest<Hash>(Oracle);
}
