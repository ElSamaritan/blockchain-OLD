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

#include "Xi/Crypto/Hash/Sha2.hh"

#include <openssl/sha.h>

int xi_crypto_hash_sha2_224(const xi_byte_t *data, size_t length, xi_crypto_hash_224 out)
{
  SHA256_CTX ctx;
  if(SHA224_Init(&ctx) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if(SHA224_Update(&ctx, (const unsigned char*)data, length) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if(SHA224_Final(out, &ctx) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  return XI_RETURN_CODE_SUCCESS;
}

int xi_crypto_hash_sha2_256(const xi_byte_t *data, size_t length, xi_crypto_hash_256 out)
{
  SHA256_CTX ctx;
  if(SHA256_Init(&ctx) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if(SHA256_Update(&ctx, (const unsigned char*)data, length) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if(SHA256_Final(out, &ctx) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  return XI_RETURN_CODE_SUCCESS;
}

int xi_crypto_hash_sha2_384(const xi_byte_t *data, size_t length, xi_crypto_hash_384 out)
{
  SHA512_CTX ctx;
  if(SHA384_Init(&ctx) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if(SHA384_Update(&ctx, (const unsigned char*)data, length) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if(SHA384_Final(out, &ctx) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  return XI_RETURN_CODE_SUCCESS;
}

int xi_crypto_hash_sha2_512(const xi_byte_t *data, size_t length, xi_crypto_hash_512 out)
{
  SHA512_CTX ctx;
  if(SHA512_Init(&ctx) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if(SHA512_Update(&ctx, (const unsigned char*)data, length) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if(SHA512_Final(out, &ctx) != 1) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  return XI_RETURN_CODE_SUCCESS;
}
