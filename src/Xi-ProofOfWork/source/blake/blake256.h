#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

typedef struct {
  uint32_t h[8], s[4], t[2];
  int buflen, nullt;
  uint8_t buf[64];
} blake_state;

typedef struct {
  blake_state inner;
  blake_state outer;
} blake_hmac_state;

void blake256_init(blake_state *);
void blake224_init(blake_state *);

void blake256_update(blake_state *, const uint8_t *, uint64_t);
void blake224_update(blake_state *, const uint8_t *, uint64_t);

void blake256_final(blake_state *, uint8_t *);
void blake224_final(blake_state *, uint8_t *);

void blake256_hash(uint8_t *, const uint8_t *, uint64_t);
void blake224_hash(uint8_t *, const uint8_t *, uint64_t);

/* HMAC functions: */

void hmac_blake256_init(blake_hmac_state *, const uint8_t *, uint64_t);
void hmac_blake224_init(blake_hmac_state *, const uint8_t *, uint64_t);

void hmac_blake256_update(blake_hmac_state *, const uint8_t *, uint64_t);
void hmac_blake224_update(blake_hmac_state *, const uint8_t *, uint64_t);

void hmac_blake256_final(blake_hmac_state *, uint8_t *);
void hmac_blake224_final(blake_hmac_state *, uint8_t *);

void hmac_blake256_hash(uint8_t *, const uint8_t *, uint64_t, const uint8_t *, uint64_t);
void hmac_blake224_hash(uint8_t *, const uint8_t *, uint64_t, const uint8_t *, uint64_t);

#if defined(__cplusplus)
}
#endif
