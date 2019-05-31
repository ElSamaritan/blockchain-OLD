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

#include "Xi/Crypto/Random/Random.hh"

#include <string.h>
#include <stdlib.h>

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
#include <Xi/Memory/Clear.hh>

#include "Xi/Crypto/Hash/Keccak.hh"

struct xi_crypto_random_state {
  uint64_t bytes[XI_HASH_1600_SIZE / sizeof (uint64_t)];
  size_t left;
};


static XI_RUNTIME_THREAD_LOCAL xi_crypto_random_state* xi_crypto_random_state_instance = NULL;

#if defined (_WIN32)

#include <synchapi.h>

static CRITICAL_SECTION xi_crypto_random_system_byte_critical_section;

static void __cdecl xi_crypto_random_system_byte_critical_section_initialize(void) {
  InitializeCriticalSection(&xi_crypto_random_system_byte_critical_section);
}
#pragma section(".CRT$XCU", read)
__declspec(allocate(".CRT$XCU"))
const void (__cdecl *_xi_crypto_random_system_byte_critical_section_initialize)(void) =
        xi_crypto_random_system_byte_critical_section_initialize;

#else

#include <pthread.h>

pthread_mutex_t xi_crypto_random_system_byte_critical_section = PTHREAD_MUTEX_INITIALIZER;

#endif

int xi_crypto_random_system_bytes(xi_byte_t *out, size_t count)
{
#if defined(_WIN32)
  EnterCriticalSection(&xi_crypto_random_system_byte_critical_section);
  HCRYPTPROV hCryptProv;
  if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) != TRUE) {
    LeaveCriticalSection(&xi_crypto_random_system_byte_critical_section);
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  if (CryptGenRandom(hCryptProv, (DWORD)count, (BYTE*)out) != TRUE) {
    LeaveCriticalSection(&xi_crypto_random_system_byte_critical_section);
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  LeaveCriticalSection(&xi_crypto_random_system_byte_critical_section);
  CryptReleaseContext(hCryptProv, 0);
  return XI_RETURN_CODE_SUCCESS;
#else
  pthread_mutex_lock(&xi_crypto_random_system_byte_critical_section);
  const int fileDescriptor = open("/dev/urandom", O_RDONLY | O_NOCTTY | O_CLOEXEC);
  if (fileDescriptor < 0) {
    pthread_mutex_unlock(&xi_crypto_random_system_byte_critical_section);
    return XI_RETURN_CODE_NO_SUCCESS;
  }
  size_t offset = 0;
  while(offset < count) {
      const ssize_t iBytesRead = read(fileDescriptor, out + offset, count - offset);
      if(iBytesRead < 1) {
          pthread_mutex_unlock(&xi_crypto_random_system_byte_critical_section);
          return XI_RETURN_CODE_NO_SUCCESS;
      }
      offset += (size_t)iBytesRead;
  }
  pthread_mutex_unlock(&xi_crypto_random_system_byte_critical_section);
  return XI_RETURN_CODE_SUCCESS;
#endif
}

xi_crypto_random_state *xi_crypto_random_state_create(void)
{
  xi_crypto_random_state* reval = NULL;
  reval = (xi_crypto_random_state*)malloc(sizeof(xi_crypto_random_state));
  return reval;
}

int xi_crypto_random_state_init(xi_crypto_random_state *state)
{
  int ec = XI_RETURN_CODE_SUCCESS;
  ec = xi_crypto_random_system_bytes((xi_byte_t*)state->bytes, XI_HASH_1600_SIZE);
  XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
  state->left = XI_HASH_1600_SIZE;
  return XI_RETURN_CODE_SUCCESS;
}

int xi_crypto_random_state_init_determentistic(xi_crypto_random_state *state, const xi_byte_t *seed, size_t seedLength)
{
  memset(state->bytes, 0, XI_HASH_1600_SIZE);
  int ec = XI_RETURN_CODE_SUCCESS;
  ec = xi_crypto_hash_keccak_1600(seed, seedLength, (xi_byte_t*)state->bytes);
  XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
  state->left = XI_HASH_1600_SIZE;
  return XI_RETURN_CODE_SUCCESS;
}


void xi_crypto_random_state_destroy(xi_crypto_random_state *state)
{
  xi_memory_clear((xi_byte_t*)state->bytes, XI_HASH_1600_SIZE);
  free(state);
}

int xi_crypto_random_bytes_from_state(xi_byte_t *out, size_t count, xi_crypto_random_state *state)
{
  if(state == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  } else if(count == 0) {
    return XI_RETURN_CODE_SUCCESS;
  } else {
    size_t generated = 0;
    do {
      if(state->left == 0) {
        xi_crypto_random_state_permutation(state);
      }

      const size_t nLeft = count - generated;
      const size_t nCopy = state->left < nLeft ? state->left : nLeft;
      memcpy(out + generated, state->bytes + (XI_HASH_1600_SIZE - state->left), nCopy);
      state->left -= nCopy;
      generated += nCopy;
    } while(generated < count);
    return XI_RETURN_CODE_SUCCESS;
  }
}

void xi_crypto_random_state_permutation(xi_crypto_random_state *state)
{
  xi_crypto_hash_keccakf(state->bytes, 24U);
  state->left = XI_HASH_1600_SIZE;
}

int xi_crypto_random_bytes(xi_byte_t *out, size_t count)
{
  int ec = XI_RETURN_CODE_SUCCESS;
  if(xi_crypto_random_state_instance == NULL) {
    xi_crypto_random_state_instance = xi_crypto_random_state_create();
    XI_RETURN_EC_IF(xi_crypto_random_state_instance == NULL, XI_RETURN_CODE_NO_SUCCESS);
    ec = xi_crypto_random_state_init(xi_crypto_random_state_instance);
    if(ec != XI_RETURN_CODE_SUCCESS) {
      xi_crypto_random_state_destroy(xi_crypto_random_state_instance);
      xi_crypto_random_state_instance = NULL;
      return ec;
    }
  }

  ec = xi_crypto_random_bytes_from_state(out, count, xi_crypto_random_state_instance);
  XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
  return XI_RETURN_CODE_SUCCESS;
}

int xi_crypto_random_bytes_determenistic(xi_byte_t *out, size_t count, const xi_byte_t *seed, size_t seedLength)
{
  int ec = XI_RETURN_CODE_SUCCESS;
  if(xi_crypto_random_state_instance == NULL) {
    xi_crypto_random_state_instance = xi_crypto_random_state_create();
    XI_RETURN_EC_IF(xi_crypto_random_state_instance == NULL, XI_RETURN_CODE_NO_SUCCESS);
    ec = xi_crypto_random_state_init_determentistic(xi_crypto_random_state_instance, seed, seedLength);
    if(ec != XI_RETURN_CODE_SUCCESS) {
      xi_crypto_random_state_destroy(xi_crypto_random_state_instance);
      xi_crypto_random_state_instance = NULL;
      return ec;
    }
  }

  ec = xi_crypto_random_bytes_from_state(out, count, xi_crypto_random_state_instance);
  XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
  return XI_RETURN_CODE_SUCCESS;
}

