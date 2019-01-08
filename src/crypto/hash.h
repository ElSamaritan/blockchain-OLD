// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2014-2018, The Aeon Project
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <stddef.h>

#include <crypto/CryptoTypes.h>
#include "hash-extra-ops.h"

// Standard Cryptonight Definitions
#define CN_PAGE_SIZE 2097152
#define CN_SCRATCHPAD 2097152
#define CN_ITERATIONS 1048576

// Standard CryptoNight Lite Definitions
#define CN_LITE_PAGE_SIZE 2097152
#define CN_LITE_SCRATCHPAD 1048576
#define CN_LITE_ITERATIONS 524288

namespace Crypto {

extern "C" {
#include "hash-ops.h"
}

/*
  Cryptonight hash functions
*/

inline void cn_fast_hash(const void *data, size_t length, Hash &hash) {
  cn_fast_hash(data, length, reinterpret_cast<char *>(&hash));
}

inline Hash cn_fast_hash(const void *data, size_t length) {
  Hash h;
  cn_fast_hash(data, length, reinterpret_cast<char *>(&h));
  return h;
}

inline void tree_hash(const Hash *hashes, size_t count, Hash &root_hash) {
  tree_hash(reinterpret_cast<const char(*)[HASH_SIZE]>(hashes), count, reinterpret_cast<char *>(&root_hash));
}

inline void tree_branch(const Hash *hashes, size_t count, Hash *branch) {
  tree_branch(reinterpret_cast<const char(*)[HASH_SIZE]>(hashes), count, reinterpret_cast<char(*)[HASH_SIZE]>(branch));
}

inline void tree_hash_from_branch(const Hash *branch, size_t depth, const Hash &leaf, const void *path,
                                  Hash &root_hash) {
  tree_hash_from_branch(reinterpret_cast<const char(*)[HASH_SIZE]>(branch), depth,
                        reinterpret_cast<const char *>(&leaf), path, reinterpret_cast<char *>(&root_hash));
}
}  // namespace Crypto
