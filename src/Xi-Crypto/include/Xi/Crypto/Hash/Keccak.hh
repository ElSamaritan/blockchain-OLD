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
 * ---------------------------------------------------------------------------------------------- *
 * Previous Work                                                                                  *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * - Copyright (c) 2014-2019, The Monero Project                                                  *
 * - Markku-Juhani O. Saarinen <mjos@iki.fi>                                                      *
 *                                                                                                *
 * ============================================================================================== */

#pragma once

#include <Xi/Byte.hh>

#include "Xi/Crypto/Hash/Hash.hh"

#if defined(__cplusplus)
extern "C" {
#endif

#include <inttypes.h>
#include <stddef.h>

#define XI_CRYPTO_HASH_KECCAK_HASH_SIZE 32U
#define XI_CRYPTO_HASH_KECCAK_HASH_DATA_AREA 136U

typedef struct xi_crypto_hash_keccak_state {
  /// 1600 bits algorithm hashing state
  uint64_t hash[25];

  /// 1088-bit buffer for leftovers, block size = 136 B for 256-bit keccak
  uint64_t message[17];

  /// count of bytes in the message[] buffer
  size_t rest;
} xi_crypto_hash_keccak_state;

int xi_crypto_hash_keccak(const xi_byte_t *in, size_t inlen, xi_byte_t *md, size_t mdlen);
void xi_crypto_hash_keccakf(uint64_t st[], int rounds);

int xi_crypto_hash_keccak_init(xi_crypto_hash_keccak_state *ctx);
int xi_crypto_hash_keccak_update(xi_crypto_hash_keccak_state *ctx, const xi_byte_t *in, size_t inlen);
int xi_crypto_hash_keccak_finish(xi_crypto_hash_keccak_state *ctx, xi_byte_t *md);

int xi_crypto_hash_keccak_256(const xi_byte_t *in, size_t inlen, xi_byte_t *md);
int xi_crypto_hash_keccak_1600(const xi_byte_t *in, size_t inlen, xi_byte_t *md);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

namespace Xi {
namespace Crypto {
namespace Hash {
namespace Keccak {

XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash256, 256);
XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash1600, 1600);

void compute(ConstByteSpan data, Hash256 &out);
void compute(ConstByteSpan data, Hash1600 &out);

}  // namespace Keccak
}  // namespace Hash
}  // namespace Crypto
}  // namespace Xi

#endif
