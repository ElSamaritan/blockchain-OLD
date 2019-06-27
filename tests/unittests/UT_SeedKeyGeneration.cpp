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

#include <random>
#include <unordered_set>

#include <crypto/CryptoTypes.h>
#include <crypto/crypto.h>
#include <CryptoNoteCore/CryptoNoteBasic.h>

#define XI_TESTSUITE UT_Xi_Crypto_Seed_Key_Generation

TEST(UT_Xi_Crypto_Seed_Key_Generation, Consistent) {
  using namespace ::testing;

  std::unordered_set<Crypto::PublicKey> pubKeys;
  std::unordered_set<Crypto::SecretKey> secKeys;
  for (uint32_t i = 0; i < 4; ++i) {
    Crypto::Hash seed;
    ASSERT_EQ(Xi::Crypto::Random::generate(seed.span()), Xi::Crypto::Random::RandomError::Success);

    const auto keyPair = CryptoNote::generateDeterministicKeyPair(seed);
    for (uint32_t j = 0; j < 3; ++j) {
      const auto jKeyPair = CryptoNote::generateDeterministicKeyPair(seed);
      EXPECT_EQ(keyPair.publicKey, jKeyPair.publicKey);
      EXPECT_EQ(keyPair.secretKey, jKeyPair.secretKey);
    }
  }
}
