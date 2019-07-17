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

#include "Xi/Crypto/Hash/FastHash.hh"

#include <stdlib.h>

#include <Xi/Global.hh>

#include "Xi/Crypto/Hash/Keccak.hh"

xi_crypto_hash_fast_hash_state *xi_crypto_hash_fast_hash_create(void) {
  return (xi_crypto_hash_fast_hash_state *)malloc(sizeof(xi_crypto_hash_fast_hash_state));
}

int xi_crypto_hash_fast_hash_init(xi_crypto_hash_fast_hash_state *state) {
  return xi_crypto_hash_keccak_init(state);
}

int xi_crypto_hash_fast_hash_update(xi_crypto_hash_fast_hash_state *state, const xi_byte_t *data, size_t length) {
  if (state == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    return xi_crypto_hash_keccak_update(state, data, length);
  }
}

int xi_crypto_hash_fast_hash_finalize(xi_crypto_hash_fast_hash_state *state, xi_crypto_hash_fast out) {
  if (state == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    return xi_crypto_hash_fast_hash_finalize(state, out);
  }
}

void xi_crypto_hash_fast_hash_destroy(xi_crypto_hash_fast_hash_state *state) {
  if (state != NULL) {
    free(state);
  }
}

int xi_crypto_hash_fast_hash(const xi_byte_t *data, size_t length, xi_crypto_hash_fast out) {
  return xi_crypto_hash_keccak_256(data, length, out);
}
