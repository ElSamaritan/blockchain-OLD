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

#include "Xi/Crypto/Random.hh"

#include <string.h>

#if defined(_WIN32)
#include <Windows.h>
#include <wincrypt.h>
#else
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <Xi/Global.hh>
#include <Xi/Runtime.hh>

#include "Xi/Crypto/Hash/Keccak.hh"

#define XI_CRYPTO_RANDOM_STATE_DEFAULT 0
#define XI_CRYPTO_RANDOM_STATE_INITIALIZED 1
#define XI_CRYPTO_RANDOM_STATE_UPDATING 2

static XI_RUNTIME_THREAD_LOCAL uint64_t xi_crypto_random_state_bytes[XI_HASH_1600_SIZE / sizeof(uint64_t)];
static XI_RUNTIME_THREAD_LOCAL int xi_crypto_random_state_current = XI_CRYPTO_RANDOM_STATE_DEFAULT;
static XI_RUNTIME_THREAD_LOCAL size_t xi_crypto_random_state_pos = 0;

int xi_crypto_random_state_init(void) {
  int ec = xi_crypto_random_system_bytes((xi_byte_t*)xi_crypto_random_state_bytes, XI_HASH_1600_SIZE);
  if(ec != XI_RETURN_CODE_SUCCESS) {
    return ec;
  } else {
    xi_crypto_random_state_current = XI_CRYPTO_RANDOM_STATE_INITIALIZED;
    xi_crypto_random_state_pos = 0;
    return XI_RETURN_CODE_SUCCESS;
  }
}

int xi_crypto_random_state_update(void) {
  xi_crypto_hash_keccakf(xi_crypto_random_state_bytes, 24U);
  xi_crypto_random_state_pos = 0;
  return XI_RETURN_CODE_SUCCESS;
}

int xi_crypto_random_system_bytes(xi_byte_t *out, size_t count)
{
#if defined(_WIN32)
  HCRYPTPROV hCryptProv;
  if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) != TRUE) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if (CryptGenRandom(hCryptProv, (DWORD)count, (BYTE*)out) != TRUE) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  CryptReleaseContext(hCryptProv, 0);
  return XI_RETURN_CODE_SUCCESS;
#else
  const int fileDescriptor = open("/dev/urandom", O_RDONLY | O_NOCTTY | O_CLOEXEC);
  if (fileDescriptor < 0) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  size_t offset = 0;
  while(offset < count) {
      const ssize_t iBytesRead = read(fileDescriptor, out + offset, count - offset);
      if(iBytesRead < 1) {
          return XI_RETURN_CODE_NO_SUCCESS;
      }
      offset += (size_t)iBytesRead;
  }
  return XI_RETURN_CODE_SUCCESS;
#endif
}

int xi_crypto_random_bytes(xi_byte_t *out, size_t count)
{
  if(xi_crypto_random_state_pos == XI_CRYPTO_RANDOM_STATE_DEFAULT) {
    const int ec = xi_crypto_random_state_init();
    if(ec != XI_RETURN_CODE_SUCCESS) {
      return ec;
    }
  }

  if(xi_crypto_random_state_current != XI_CRYPTO_RANDOM_STATE_INITIALIZED) {
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    int ec = xi_crypto_random_state_update();
    if(ec != XI_RETURN_CODE_SUCCESS) {
      return ec;
    }

    size_t generated = 0;
    do {
      const size_t nLeft = count - generated;
      const size_t nCopy = XI_HASH_1600_SIZE < nLeft ? XI_HASH_1600_SIZE : nLeft;
      memcpy(out + generated, xi_crypto_random_state_bytes + xi_crypto_random_state_pos, nCopy);
      xi_crypto_random_state_pos += nCopy;
      generated += nCopy;

      ec = xi_crypto_random_state_update();
      if(ec != XI_RETURN_CODE_SUCCESS) {
        return ec;
      }
    } while(generated < count);
    return XI_RETURN_CODE_SUCCESS;
  }
}

int xi_crypto_random_bytes_determenistic(xi_byte_t *out, size_t count, const xi_byte_t *seed, size_t seedLength)
{
  if(count == 0) {
    return XI_RETURN_CODE_SUCCESS;
  }

  uint64_t buffer[XI_HASH_1600_SIZE / sizeof(uint64_t)];
  int ec = xi_crypto_hash_keccak_1600(seed, seedLength, (xi_byte_t*)buffer);
  if(ec != XI_RETURN_CODE_SUCCESS) {
    return ec;
  }

  size_t generated = 0;
  while(generated < count) {
    const size_t nLeft = count - generated;
    const size_t nCopy = XI_HASH_1600_SIZE < nLeft ? XI_HASH_1600_SIZE : nLeft;
    memcpy(out + generated, buffer, nCopy);
    generated += nCopy;
    xi_crypto_hash_keccakf(buffer, 24);
  }

  return XI_RETURN_CODE_SUCCESS;
}
