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

#pragma once

#include <Xi/Global.hh>
#include <Xi/Byte.hh>

#include "Xi/Crypto/Hash/Hash.hh"

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#define XI_HASH_FAST_HASH_SIZE 32u
#define XI_HASH_FAST_HASH_BITS (XI_HASH_FAST_HASH_SIZE * 8u)

typedef xi_byte_t xi_crypto_hash_fast[XI_HASH_FAST_HASH_SIZE];
typedef struct xi_crypto_hash_keccak_state xi_crypto_hash_fast_hash_state;

xi_crypto_hash_fast_hash_state* xi_crypto_hash_fast_hash_create(void);
int xi_crypto_hash_fast_hash_init(xi_crypto_hash_fast_hash_state* state);
int xi_crypto_hash_fast_hash_update(xi_crypto_hash_fast_hash_state* state, const xi_byte_t* data, size_t length);
int xi_crypto_hash_fast_hash_finalize(xi_crypto_hash_fast_hash_state* state, xi_crypto_hash_fast out);
void xi_crypto_hash_fast_hash_destroy(xi_crypto_hash_fast_hash_state* state);

int xi_crypto_hash_fast_hash(const xi_byte_t* data, size_t length, xi_crypto_hash_fast out);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include <type_traits>

namespace Xi {
namespace Crypto {
namespace Hash {

void fastHash(ConstByteSpan data, ByteSpan out);

}  // namespace Hash
}  // namespace Crypto
}  // namespace Xi

#endif
