// keccak.h
// 19-Nov-11  Markku-Juhani O. Saarinen <mjos@iki.fi>

#ifndef KECCAK_H
#define KECCAK_H

#include <stdint.h>
#include <string.h>

#ifndef KECCAK_ROUNDS
#define KECCAK_ROUNDS 24
#endif

#ifndef ROTL64
#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))
#endif

// compute a keccak hash (md) of given byte length from "in"
int keccak(const uint8_t *in, int inlen, uint8_t *md, int mdlen);

// update the state
void keccakf(uint64_t st[25], int norounds);

void keccak1600(const uint8_t *in, int inlen, uint8_t *md);

#if !defined(KECCAK_1600_MD_SIZE)
#define KECCAK_1600_MD_SIZE (1600 / 8)
#endif  // !defined(KECCAK_1600_MD_SIZE)

typedef uint8_t keccak1600_hash[KECCAK_1600_MD_SIZE];

#endif
