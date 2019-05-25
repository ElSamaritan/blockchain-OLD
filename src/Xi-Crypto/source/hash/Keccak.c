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
 * ---------------------------------------------------------------------------------------------- *
 * Previous Work                                                                                  *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * - Copyright (c) 2014-2019, The Monero Project                                                  *
 * - Markku-Juhani O. Saarinen <mjos@iki.fi>                                                      *
 *                                                                                                *
 * ============================================================================================== */

#include "Xi/Crypto/Hash/Keccak.hh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <Xi/Global.hh>
#include <Xi/Endianess.hh>

#if !defined(KECCAK_ROUNDS)
#define KECCAK_ROUNDS 24
#endif

#if !defined(ROTL64)
#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
#endif

static const uint64_t keccakf_rndc[24] =
{
        0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
        0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
        0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
        0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
        0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
        0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
        0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
        0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

static const int keccakf_rotc[24] =
{
        1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
        27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
};

static const int keccakf_piln[24] =
{
        10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
        15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1
};

typedef uint64_t state_t[25];

void xi_crypto_hash_keccakf(uint64_t st[], int rounds)
{
  int i, j, round;
  uint64_t t, bc[5];

  for (round = 0; round < rounds; round++) {

    // Theta
    for (i = 0; i < 5; i++)
      bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];

    for (i = 0; i < 5; i++) {
      t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
      for (j = 0; j < 25; j += 5)
        st[j + i] ^= t;
    }

    // Rho Pi
    t = st[1];
    for (i = 0; i < 24; i++) {
      j = keccakf_piln[i];
      bc[0] = st[j];
      st[j] = ROTL64(t, keccakf_rotc[i]);
      t = bc[0];
    }

    //  Chi
    for (j = 0; j < 25; j += 5) {
      for (i = 0; i < 5; i++)
        bc[i] = st[j + i];
      for (i = 0; i < 5; i++)
        st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
    }

    //  Iota
    st[0] ^= keccakf_rndc[round];
  }
}

int xi_crypto_hash_keccak(const uint8_t *in, size_t inlen, uint8_t *md, size_t mdlen)
{
  state_t st;
  uint8_t temp[144];
  size_t i, rsiz, rsizw;

  if (mdlen <= 0 || (mdlen > 100 && sizeof(st) != (size_t)mdlen))
  {
    return XI_RETURN_CODE_NO_SUCCESS;
  }

  rsiz = sizeof(state_t) == mdlen ? XI_CRYPTO_HASH_KECCAK_HASH_DATA_AREA : 200U - 2U * mdlen;
  rsizw = rsiz / 8;

  memset(st, 0, sizeof(st));

  for ( ; inlen >= rsiz; inlen -= rsiz, in += rsiz) {
    for (i = 0; i < rsizw; i++)
      st[i] ^= xi_endianess_little_64(((const uint64_t *) in)[i]);
    xi_crypto_hash_keccakf(st, KECCAK_ROUNDS);
  }

  // last block and padding
  if (inlen + 1 >= sizeof(temp) || inlen > rsiz || rsiz - inlen + inlen + 1 >= sizeof(temp) || rsiz == 0 || rsiz - 1 >= sizeof(temp) || rsizw * 8 > sizeof(temp))
  {
    return  XI_RETURN_CODE_NO_SUCCESS;
  }

  memcpy(temp, in, inlen);
  temp[inlen++] = 6;
  memset(temp + inlen, 0, rsiz - inlen);
  temp[rsiz - 1] |= 0x80;

  for (i = 0; i < rsizw; i++)
    st[i] ^= xi_endianess_little_64(((const uint64_t *) temp)[i]);

  xi_crypto_hash_keccakf(st, KECCAK_ROUNDS);

  if (((size_t)mdlen % sizeof(uint64_t)) != 0)
  {
    return  XI_RETURN_CODE_NO_SUCCESS;
  }

  for(i = 0; i < mdlen / sizeof(uint64_t); ++i) {
    ((uint64_t *) md)[i] = xi_endianess_little_64(((const uint64_t *) st)[i]);
  }
  return XI_RETURN_CODE_SUCCESS;
}

#define KECCAK_FINALIZED 0x80000000
#define KECCAK_BLOCKLEN 136
#define KECCAK_WORDS 17
#define KECCAK_DIGESTSIZE 32
#define IS_ALIGNED_64(p) ((0 == (7 & ((const char*)(p) - (const char*)0))) ? XI_TRUE : XI_FALSE)
#define KECCAK_PROCESS_BLOCK(st, block) { \
    for (int i_ = 0; i_ < KECCAK_WORDS; i_++){ \
        ((st))[i_] ^= xi_endianess_little_64(((block))[i_]); \
    }; \
    xi_crypto_hash_keccakf(st, KECCAK_ROUNDS); }



int xi_crypto_hash_keccak_init(xi_crypto_hash_keccak_state *ctx)
{
  if(ctx == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  } else {
    memset(ctx, 0, sizeof(xi_crypto_hash_keccak_state));
    return XI_RETURN_CODE_SUCCESS;
  }
}

int xi_crypto_hash_keccak_update(xi_crypto_hash_keccak_state *ctx, const xi_byte_t *in, size_t inlen)
{
  if (ctx->rest & KECCAK_FINALIZED) {
    return  XI_RETURN_CODE_NO_SUCCESS;
  }

  const size_t idx = ctx->rest;
  ctx->rest = (ctx->rest + inlen) % KECCAK_BLOCKLEN;

  // fill partial block
  if (idx) {
    size_t left = KECCAK_BLOCKLEN - idx;
    memcpy((char*)ctx->message + idx, in, (inlen < left ? inlen : left));
    if (inlen < left) return XI_RETURN_CODE_SUCCESS;

    KECCAK_PROCESS_BLOCK(ctx->hash, ctx->message);

    in  += left;
    inlen -= left;
  }

  const int is_aligned = IS_ALIGNED_64(in);
  while (inlen >= KECCAK_BLOCKLEN) {
    const uint64_t* aligned_message_block;
    if (is_aligned == XI_TRUE) {
      aligned_message_block = (const uint64_t*)in;
    } else {
      memcpy(ctx->message, in, KECCAK_BLOCKLEN);
      aligned_message_block = ctx->message;
    }

    KECCAK_PROCESS_BLOCK(ctx->hash, aligned_message_block);
    in  += KECCAK_BLOCKLEN;
    inlen -= KECCAK_BLOCKLEN;
  }
  if (inlen) {
    memcpy(ctx->message, in, inlen);
  }
  return XI_RETURN_CODE_SUCCESS;
}

int xi_crypto_hash_keccak_finish(xi_crypto_hash_keccak_state *ctx, xi_byte_t *md)
{
  if(md == NULL) {
    return XI_RETURN_CODE_NO_SUCCESS;
  }

  if (!(ctx->rest & KECCAK_FINALIZED))
  {
    memset((char*)ctx->message + ctx->rest, 0, KECCAK_BLOCKLEN - ctx->rest);
    ((char*)ctx->message)[ctx->rest] |= 6;
    ((char*)ctx->message)[KECCAK_BLOCKLEN - 1] |= 0x80;

    KECCAK_PROCESS_BLOCK(ctx->hash, ctx->message);
    ctx->rest = KECCAK_FINALIZED;
  }

  size_t i;
  for(i = 0; i < KECCAK_DIGESTSIZE / sizeof(uint64_t); ++i) {
    ((uint64_t *) md)[i] = xi_endianess_little_64(((const uint64_t *) ctx->hash)[i]);
  }
  return XI_RETURN_CODE_SUCCESS;
}

int xi_crypto_hash_keccak_256(const xi_byte_t *in, size_t inlen, xi_byte_t *md)
{
  return xi_crypto_hash_keccak(in, inlen, md, XI_HASH_256_SIZE);
}

int xi_crypto_hash_keccak_1600(const xi_byte_t *in, size_t inlen, xi_byte_t *md)
{
  return xi_crypto_hash_keccak(in, inlen, md, XI_HASH_1600_SIZE);
}
