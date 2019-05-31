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

#include <Xi/Byte.hh>

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>

typedef struct xi_crypto_random_state xi_crypto_random_state;

xi_crypto_random_state *xi_crypto_random_state_create(void);

int xi_crypto_random_state_init(xi_crypto_random_state *state);
int xi_crypto_random_state_init_determentistic(xi_crypto_random_state *state, const xi_byte_t *seed, size_t seedLength);
void xi_crypto_random_state_permutation(xi_crypto_random_state *state);
int xi_crypto_random_bytes_from_state(xi_byte_t *out, size_t count, xi_crypto_random_state *state);
void xi_crypto_random_state_destroy(xi_crypto_random_state *state);

int xi_crypto_random_system_bytes(xi_byte_t *out, size_t count);
int xi_crypto_random_bytes(xi_byte_t *out, size_t count);
int xi_crypto_random_bytes_determenistic(xi_byte_t *out, size_t count, const xi_byte_t *seed, size_t seedLength);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include <type_traits>

#include <Xi/Exceptional.hpp>

namespace Xi {
namespace Crypto {
namespace Random {
XI_DECLARE_EXCEPTIONAL_CATEGORY(Random)
XI_DECLARE_EXCEPTIONAL_INSTANCE(Generation, "failed to generate random bytes", Random)

ByteVector generate(size_t count);
ByteVector generate(size_t count, ConstByteSpan seed);

void generate(ByteSpan out);
void generate(ByteSpan out, ConstByteSpan seed);

template <typename _ValueT>
typename std::enable_if_t<std::is_standard_layout_v<_ValueT>, _ValueT> generate() {
  using value_type = std::remove_cv_t<_ValueT>;
  value_type reval{};
  generate(Xi::ByteSpan{reinterpret_cast<Xi::Byte *>(&reval), sizeof(value_type)});
  return reval;
}

template <typename _ValueT>
typename std::enable_if_t<std::is_standard_layout_v<_ValueT>, _ValueT> generate(ConstByteSpan seed) {
  using value_type = std::remove_cv_t<_ValueT>;
  value_type reval{};
  generate(Xi::ByteSpan{reinterpret_cast<Xi::Byte *>(&reval), sizeof(value_type)}, seed);
  return reval;
}

}  // namespace Random
}  // namespace Crypto
}  // namespace Xi
#endif
