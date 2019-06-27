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

#include "Xi/Crypto/Hash/Sha3.hh"

#include <openssl/evp.h>
#include <openssl/sha.h>

#include <Xi/Global.hh>

#define XI_OPEN_SLL_FREE_AND_FAIL

int xi_crypto_hash_sha3_224(const xi_byte_t *data, size_t length, xi_crypto_hash_224 out)
{
  EVP_MD_CTX *mctx;
  mctx = EVP_MD_CTX_create();
  if(mctx == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  else if(EVP_DigestInit_ex(mctx, EVP_sha3_224(), NULL) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  else if(EVP_DigestUpdate(mctx, data, length) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    unsigned int mdLen = 0;
    if(EVP_DigestFinal_ex(mctx, out, &mdLen) != 1) {
      EVP_MD_CTX_destroy(mctx);
      return XI_RETURN_CODE_NO_SUCCESS;
    } else {
      EVP_MD_CTX_destroy(mctx);
      return XI_RETURN_CODE_SUCCESS;
    }
  }
}

int xi_crypto_hash_sha3_256(const xi_byte_t *data, size_t length, xi_crypto_hash_256 out)
{
  EVP_MD_CTX *mctx;
  mctx = EVP_MD_CTX_create();
  if(mctx == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  else if(EVP_DigestInit_ex(mctx, EVP_sha3_256(), NULL) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  else if(EVP_DigestUpdate(mctx, data, length) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    unsigned int mdLen = 0;
    if(EVP_DigestFinal_ex(mctx, out, &mdLen) != 1) {
      EVP_MD_CTX_destroy(mctx);
      return XI_RETURN_CODE_NO_SUCCESS;
    } else {
      EVP_MD_CTX_destroy(mctx);
      return XI_RETURN_CODE_SUCCESS;
    }
  }
}

int xi_crypto_hash_sha3_384(const xi_byte_t *data, size_t length, xi_crypto_hash_384 out)
{
  EVP_MD_CTX *mctx;
  mctx = EVP_MD_CTX_create();
  if(mctx == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  else if(EVP_DigestInit_ex(mctx, EVP_sha3_384(), NULL) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  else if(EVP_DigestUpdate(mctx, data, length) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    unsigned int mdLen = 0;
    if(EVP_DigestFinal_ex(mctx, out, &mdLen) != 1) {
      EVP_MD_CTX_destroy(mctx);
      return XI_RETURN_CODE_NO_SUCCESS;
    } else {
      EVP_MD_CTX_destroy(mctx);
      return XI_RETURN_CODE_SUCCESS;
    }
  }
}

int xi_crypto_hash_sha3_512(const xi_byte_t *data, size_t length, xi_crypto_hash_512 out)
{
  EVP_MD_CTX *mctx;
  mctx = EVP_MD_CTX_create();
  if(mctx == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  else if(EVP_DigestInit_ex(mctx, EVP_sha3_512(), NULL) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  else if(EVP_DigestUpdate(mctx, data, length) != 1) {
    EVP_MD_CTX_destroy(mctx);
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    unsigned int mdLen = 0;
    if(EVP_DigestFinal_ex(mctx, out, &mdLen) != 1) {
      EVP_MD_CTX_destroy(mctx);
      return XI_RETURN_CODE_NO_SUCCESS;
    } else {
      EVP_MD_CTX_destroy(mctx);
      return XI_RETURN_CODE_SUCCESS;
    }
  }
}
