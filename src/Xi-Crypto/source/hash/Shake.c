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

#include "Xi/Crypto/Hash/Shake.hh"

#include <openssl/evp.h>

#include <Xi/Global.hh>

EVP_MD_CTX *xi_crypto_hash_shake_128_init(void)
{
  EVP_MD_CTX* mctx = EVP_MD_CTX_create();

  if(mctx == NULL) {
    return NULL;
  }

  if(EVP_DigestInit_ex(mctx, EVP_shake128(), NULL) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return NULL;
  }

  return mctx;
}

int xi_crypto_hash_shake_128_update(xi_crypto_hash_shake_128_state *state, const xi_byte_t *data, size_t length)
{
  if(EVP_DigestUpdate(state, data, length) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    return XI_RETURN_CODE_SUCCESS;
  }
}

int xi_crypto_hash_shake_128_finalize(xi_crypto_hash_shake_128_state *state, xi_crypto_hash_128 out)
{
  unsigned int mdLen = 0;
  if(EVP_DigestFinal_ex(state, out, &mdLen) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    return XI_RETURN_CODE_SUCCESS;
  }
}

void xi_crypto_hash_shake_128_destroy(xi_crypto_hash_shake_128_state *state)
{
  if(state != NULL) {
    EVP_MD_CTX_destroy(state);
  }
}

int xi_crypto_hash_shake_128(const xi_byte_t *data, size_t length, xi_crypto_hash_128 out)
{
  EVP_MD_CTX *state = xi_crypto_hash_shake_128_init();
  if(state == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }

  if(xi_crypto_hash_shake_128_update(state, data, length) != XI_RETURN_CODE_SUCCESS) {
    xi_crypto_hash_shake_128_destroy(state);
    return XI_RETURN_CODE_NO_SUCCESS;
  }

  if(xi_crypto_hash_shake_128_finalize(state, out) != XI_RETURN_CODE_SUCCESS) {
    xi_crypto_hash_shake_128_destroy(state);
    return XI_RETURN_CODE_NO_SUCCESS;
  }

  xi_crypto_hash_shake_128_destroy(state);
  return XI_RETURN_CODE_SUCCESS;
}

xi_crypto_hash_shake_256_state *xi_crypto_hash_shake_256_init(void)
{
  EVP_MD_CTX* mctx = EVP_MD_CTX_create();

  if(mctx == NULL) {
    return NULL;
  }

  if(EVP_DigestInit_ex(mctx, EVP_shake256(), NULL) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return NULL;
  }

  return mctx;
}

int xi_crypto_hash_shake_256_update(xi_crypto_hash_shake_256_state *state, const xi_byte_t *data, size_t length)
{
  if(EVP_DigestUpdate(state, data, length) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    return XI_RETURN_CODE_SUCCESS;
  }
}

int xi_crypto_hash_shake_256_finalize(xi_crypto_hash_shake_256_state *state, xi_crypto_hash_256 out)
{
  unsigned int mdLen = 0;
  if(EVP_DigestFinal_ex(state, out, &mdLen) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    return XI_RETURN_CODE_SUCCESS;
  }
}

void xi_crypto_hash_shake_256_destroy(xi_crypto_hash_shake_256_state *state)
{
  if(state != NULL) {
    EVP_MD_CTX_destroy(state);
  }
}

int xi_crypto_hash_shake_256(const xi_byte_t *data, size_t length, xi_crypto_hash_256 out)
{
  EVP_MD_CTX *state = xi_crypto_hash_shake_256_init();
  if(state == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }

  if(xi_crypto_hash_shake_256_update(state, data, length) != XI_RETURN_CODE_SUCCESS) {
    xi_crypto_hash_shake_256_destroy(state);
    return XI_RETURN_CODE_NO_SUCCESS;
  }

  if(xi_crypto_hash_shake_256_finalize(state, out) != XI_RETURN_CODE_SUCCESS) {
    xi_crypto_hash_shake_256_destroy(state);
    return XI_RETURN_CODE_NO_SUCCESS;
  }

  xi_crypto_hash_shake_256_destroy(state);
  return XI_RETURN_CODE_SUCCESS;
}
