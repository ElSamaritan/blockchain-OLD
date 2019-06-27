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

#pragma once

#include <Xi/Global.hh>
#include <Xi/Byte.hh>

#include "Xi/Crypto/Hash/Hash.hh"

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>

typedef struct evp_md_ctx_st xi_crypto_hash_shake_128_state;
xi_crypto_hash_shake_128_state *xi_crypto_hash_shake_128_init(void);
int xi_crypto_hash_shake_128_update(xi_crypto_hash_shake_128_state *state, const xi_byte_t *data, size_t length);
int xi_crypto_hash_shake_128_finalize(xi_crypto_hash_shake_128_state *state, xi_crypto_hash_128 out);
void xi_crypto_hash_shake_128_destroy(xi_crypto_hash_shake_128_state *state);
int xi_crypto_hash_shake_128(const xi_byte_t *data, size_t length, xi_crypto_hash_128 out);

typedef struct evp_md_ctx_st xi_crypto_hash_shake_256_state;
xi_crypto_hash_shake_256_state *xi_crypto_hash_shake_256_init(void);
int xi_crypto_hash_shake_256_update(xi_crypto_hash_shake_256_state *state, const xi_byte_t *data, size_t length);
int xi_crypto_hash_shake_256_finalize(xi_crypto_hash_shake_256_state *state, xi_crypto_hash_256 out);
void xi_crypto_hash_shake_256_destroy(xi_crypto_hash_shake_256_state *state);
int xi_crypto_hash_shake_256(const xi_byte_t *data, size_t length, xi_crypto_hash_256 out);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

namespace Xi {
namespace Crypto {
namespace Hash {
namespace Shake {
XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash128, 128)
XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash256, 256)

void compute(ConstByteSpan data, Hash128 &out);
void compute(ConstByteSpan data, Hash256 &out);
}  // namespace Shake
}  // namespace Hash
}  // namespace Crypto
}  // namespace Xi

#endif
