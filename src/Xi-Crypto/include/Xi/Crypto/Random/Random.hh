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

#include <Xi/Byte.hh>

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>

typedef struct xi_crypto_random_state xi_crypto_random_state;

xi_crypto_random_state *xi_crypto_random_state_create(void);

int xi_crypto_random_state_init(xi_crypto_random_state *state);
int xi_crypto_random_state_init_deterministic(xi_crypto_random_state *state, const xi_byte_t *seed, size_t seedLength);
void xi_crypto_random_state_permutation(xi_crypto_random_state *state);
void xi_crypto_random_state_permutation_deterministic(xi_crypto_random_state *state);
int xi_crypto_random_bytes_from_state(xi_byte_t *out, size_t count, xi_crypto_random_state *state);
int xi_crypto_random_bytes_from_state_deterministic(xi_byte_t *out, size_t count, xi_crypto_random_state *state);
void xi_crypto_random_state_destroy(xi_crypto_random_state *state);

int xi_crypto_random_system_bytes(xi_byte_t *out, size_t count);
int xi_crypto_random_bytes(xi_byte_t *out, size_t count);
int xi_crypto_random_bytes_determenistic(xi_byte_t *out, size_t count, const xi_byte_t *seed, size_t seedLength);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include <type_traits>

#include <Xi/Result.h>
#include <Xi/Exceptional.hpp>

#include "Xi/Crypto/Random/RandomError.hpp"
#include "Xi/Crypto/Random/Engine.hpp"

namespace Xi {
namespace Crypto {
namespace Random {

Result<ByteVector> generate(size_t count);
Result<ByteVector> generate(size_t count, ConstByteSpan seed);

RandomError generate(ByteSpan out);
RandomError generate(ByteSpan out, ConstByteSpan seed);

}  // namespace Random
}  // namespace Crypto
}  // namespace Xi
#endif
