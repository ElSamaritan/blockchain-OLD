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

#include <random>
#include <unordered_set>

#include <crypto/CryptoTypes.h>
#include <crypto/crypto.h>
#include <CryptoNoteCore/CryptoNoteBasic.h>

#define XI_TESTSUITE UT_Xi_Crypto_Seed_Key_Generation

/// This test takes long, you may want to run this though depending on the heights static rewards are enabled to
/// ensure you do not get duplicate keys.
TEST(DISABLED_UT_Xi_Crypto_Seed_Key_Generation, NoDuplicates) {
  using namespace ::testing;

  const uint32_t continiousTests = 2000000;

  std::unordered_set<Crypto::PublicKey> pubKeys;
  std::unordered_set<Crypto::SecretKey> secKeys;
  for (uint32_t i = 0; i < continiousTests; ++i) {
    auto keyPair = CryptoNote::generateKeyPair(i);
    EXPECT_TRUE(pubKeys.insert(keyPair.publicKey).second);
    EXPECT_TRUE(secKeys.insert(keyPair.secretKey).second);
  }
}
