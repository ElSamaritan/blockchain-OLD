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

#include <vector>

#include <gmock/gmock.h>

#include <Xi/Crypto/Random/Random.hh>
#include <Xi/Crypto/Hash/Keccak.hh>
#include <Xi/Crypto/Hash/Sha3.hh>

#define XI_TEST_SUITE Xi_Crypto_Hash_Keccak

TEST(XI_TEST_SUITE, Consistent) {
  Xi::ByteArray<32> random;
  ASSERT_EQ(Xi::Crypto::Random::generate(random), Xi::Crypto::Random::RandomError::Success);

  Xi::Crypto::Hash::Keccak::Hash256 previousHash;
  compute(random, previousHash);

  for (size_t i = 0; i < 10; ++i) {
    Xi::Crypto::Hash::Keccak::Hash256 iHash;
    compute(random, iHash);
    EXPECT_TRUE(previousHash == iHash);
  }
}

TEST(XI_TEST_SUITE, ConsistentWithStreamedInterface) {
  for (size_t i = 0; i < 25; ++i) {
    Xi::ByteArray<1024> random;
    ASSERT_EQ(Xi::Crypto::Random::generate(random), Xi::Crypto::Random::RandomError::Success);

    Xi::Crypto::Hash::Keccak::Hash256 singleComputation;
    compute(random, singleComputation);

    std::vector<Xi::ConstByteSpan> chunks;
    for (size_t j = 0; j < 1024; j += 32) {
      chunks.push_back(Xi::ConstByteSpan{random.data() + j, 32});
    }

    Xi::Crypto::Hash::Keccak::Hash256 iSingleHash;
    compute(random, iSingleHash);

    xi_crypto_hash_keccak_state state;
    int ec = xi_crypto_hash_keccak_init(&state);
    EXPECT_EQ(ec, XI_RETURN_CODE_SUCCESS);
    for (auto iChunk : chunks) {
      ec = xi_crypto_hash_keccak_update(&state, iChunk.data(), iChunk.size());
      EXPECT_EQ(ec, XI_RETURN_CODE_SUCCESS);
    }
    Xi::Crypto::Hash::Keccak::Hash256 iChunkedHash;
    ec = xi_crypto_hash_keccak_finish(&state, iChunkedHash.data());
    EXPECT_EQ(ec, XI_RETURN_CODE_SUCCESS);

    for (size_t k = 0; k < Xi::Crypto::Hash::Keccak::Hash256::bytes(); ++k) {
      EXPECT_EQ(singleComputation[k], iSingleHash[k]);
      EXPECT_EQ(singleComputation[k], iChunkedHash[k]);
    }
  }
}

TEST(XI_TEST_SUITE, ConsistentWithSha3) {
  for (size_t i = 0; i < 25; ++i) {
    Xi::ByteArray<1024> random;
    ASSERT_EQ(Xi::Crypto::Random::generate(random), Xi::Crypto::Random::RandomError::Success);

    Xi::Crypto::Hash::Keccak::Hash256 keccak;
    compute(random, keccak);

    Xi::Crypto::Hash::Sha3::Hash256 sha;
    compute(random, sha);

    for (size_t k = 0; k < Xi::Crypto::Hash::Keccak::Hash256::bytes(); ++k) {
      EXPECT_EQ(keccak[k], sha[k]);
    }
  }
}

TEST(XI_TEST_SUITE, ConsistentWithSha3StreamInterface) {
  for (size_t i = 0; i < 25; ++i) {
    Xi::ByteArray<1024> random;
    ASSERT_EQ(Xi::Crypto::Random::generate(random), Xi::Crypto::Random::RandomError::Success);

    Xi::Crypto::Hash::Keccak::Hash256 singleComputation;
    compute(random, singleComputation);

    std::vector<Xi::ConstByteSpan> chunks;
    for (size_t j = 0; j < 1024; j += 32) {
      chunks.push_back(Xi::ConstByteSpan{random.data() + j, 32});
    }

    Xi::Crypto::Hash::Sha3::Hash256 iSingleHash;
    compute(random, iSingleHash);

    xi_crypto_hash_keccak_state state;
    int ec = xi_crypto_hash_keccak_init(&state);
    EXPECT_EQ(ec, XI_RETURN_CODE_SUCCESS);
    for (auto iChunk : chunks) {
      ec = xi_crypto_hash_keccak_update(&state, iChunk.data(), iChunk.size());
      EXPECT_EQ(ec, XI_RETURN_CODE_SUCCESS);
    }
    Xi::Crypto::Hash::Keccak::Hash256 iChunkedHash;
    ec = xi_crypto_hash_keccak_finish(&state, iChunkedHash.data());
    EXPECT_EQ(ec, XI_RETURN_CODE_SUCCESS);

    for (size_t k = 0; k < Xi::Crypto::Hash::Keccak::Hash256::bytes(); ++k) {
      EXPECT_EQ(singleComputation[k], iSingleHash[k]);
      EXPECT_EQ(singleComputation[k], iChunkedHash[k]);
    }
  }
}
