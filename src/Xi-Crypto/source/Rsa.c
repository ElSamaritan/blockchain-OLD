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

#include "Xi/Crypto/Rsa.hh"

#include <assert.h>
#include <stdlib.h>

#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>

#define XI_OPENSSL_SC 1

int xi_crypto_rsa_sign(const xi_byte_t* message, const size_t messageLength,
                       const xi_byte_t *secretKey, const size_t secretKeySize,
                       xi_byte_t** signature, size_t* signatureLength)
{
  int rc = XI_OPENSSL_SC;
  EVP_MD_CTX* sslContext = NULL;
  BIO* bio = NULL;
  EVP_PKEY* rsaKey = NULL;

  do {
    sslContext = EVP_MD_CTX_create();
    if(sslContext == NULL) {
      rc = ~XI_OPENSSL_SC;
      XI_PRINT_EC("EVP_MD_CTX_create failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    bio = BIO_new( BIO_s_mem() );
    if(bio == NULL) {
      rc = ~XI_OPENSSL_SC;
      XI_PRINT_EC("BIO_new failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    rc = BIO_write(bio, secretKey, (int)secretKeySize);
    if(rc != (int)secretKeySize) {
      rc = ~XI_OPENSSL_SC;
      XI_PRINT_EC("BIO_write failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    PEM_read_bio_PrivateKey(bio, &rsaKey, 0, 0);
    if(rsaKey == NULL) {
      rc = ~XI_OPENSSL_SC;
      XI_PRINT_EC("PEM_read_bio_PrivateKey failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    rc = EVP_DigestSignInit(sslContext, NULL, EVP_sha512(), NULL, rsaKey);
    if(rc != XI_OPENSSL_SC) {
      XI_PRINT_EC("EVP_DigestSignInit failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    rc = EVP_DigestSignUpdate(sslContext, message, messageLength);
    if(rc != XI_OPENSSL_SC) {
      XI_PRINT_EC("EVP_DigestSignUpdate failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    size_t _signatureLength = 0;
    rc = EVP_DigestSignFinal(sslContext, NULL, &_signatureLength);
    if(rc != XI_OPENSSL_SC) {
      XI_PRINT_EC("EVP_DigestSignFinal failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    *signature = malloc(_signatureLength);
    if(*signature == NULL) {
      rc = ~XI_OPENSSL_SC;
      XI_PRINT_EC("OPENSSL_malloc failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    *signatureLength = _signatureLength;
    rc = EVP_DigestSignFinal(sslContext, *signature, signatureLength);
    if(rc != XI_OPENSSL_SC) {
      XI_PRINT_EC("EVP_DigestSignFinal failed, error %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }
    assert(*signatureLength == _signatureLength);
  } while(XI_FALSE);

  if(rsaKey != NULL) {
    EVP_PKEY_free(rsaKey);
    rsaKey = NULL;
  }

  if(bio != NULL)  {
    BIO_free(bio);
    bio = NULL;
  }

  if(sslContext != NULL) {
    EVP_MD_CTX_destroy(sslContext);
    sslContext = NULL;
  }

  if(rc != XI_OPENSSL_SC) {
    if(*signature != NULL) {
      free(*signature);
      *signature = NULL;
    }
    XI_RETURN_EC(XI_RETURN_CODE_NO_SUCCESS);
  }

  XI_RETURN_SC(XI_RETURN_CODE_SUCCESS);
}

int xi_crypto_rsa_verify(const xi_byte_t* message, const size_t messageLength,
                         const xi_byte_t *publicKey, const size_t publicKeyLength,
                         const xi_byte_t* signature, const size_t signatureLength)
{
  int rc = XI_OPENSSL_SC;
  EVP_MD_CTX* sslContext = NULL;
  BIO* bio = NULL;
  EVP_PKEY* rsaKey = NULL;

  do {
    sslContext = EVP_MD_CTX_create();
    if(sslContext == NULL) {
      rc = ~XI_OPENSSL_SC;
      XI_PRINT_EC("EVP_MD_CTX_create failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    bio = BIO_new( BIO_s_mem() );
    if(bio == NULL) {
      rc = ~XI_OPENSSL_SC;
      XI_PRINT_EC("BIO_new failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    rc = BIO_write(bio, publicKey, (int)publicKeyLength);
    if(rc != (int)publicKeyLength) {
      rc = ~XI_OPENSSL_SC;
      XI_PRINT_EC("BIO_write failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    PEM_read_bio_PUBKEY(bio, &rsaKey, NULL, NULL);
    if(rsaKey == NULL) {
      rc = ~XI_OPENSSL_SC;
      XI_PRINT_EC("PEM_read_bio_PUBKEY failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    rc = EVP_DigestVerifyInit(sslContext, NULL, EVP_sha512(), NULL, rsaKey);
    if(rc != XI_OPENSSL_SC) {
      XI_PRINT_EC("EVP_DigestVerifyInit failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    rc = EVP_DigestVerifyUpdate(sslContext, message, messageLength);
    if(rc != XI_OPENSSL_SC) {
      XI_PRINT_EC("EVP_DigestVerifyUpdate failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }

    rc = EVP_DigestVerifyFinal(sslContext, signature, signatureLength);
    if(rc != XI_OPENSSL_SC) {
      XI_PRINT_EC("EVP_DigestVerifyFinal failed, %s\n", ERR_error_string(ERR_get_error(), NULL));
      break;
    }
  } while(XI_FALSE);

  if(rsaKey != NULL) {
    EVP_PKEY_free(rsaKey);
    rsaKey = NULL;
  }

  if(bio != NULL)  {
    BIO_free(bio);
    bio = NULL;
  }

  if(sslContext != NULL) {
    EVP_MD_CTX_destroy(sslContext);
    sslContext = NULL;
  }

  XI_RETURN_EC_IF_NOT(rc == XI_OPENSSL_SC, XI_RETURN_CODE_NO_SUCCESS);
  XI_RETURN_SC(XI_RETURN_CODE_SUCCESS);
}
