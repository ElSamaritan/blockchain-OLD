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
 * ---------------------------------------------------------------------------------------------- *
 * Previous Work                                                                                  *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * - Copyright (c) 2014-2019, The Monero Project                                                  *
 *                                                                                                *
 * ============================================================================================== */

#include "Xi/Crypto/Hash/TreeHash.hh"

#include <alloca.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "Xi/Crypto/Hash/Keccak.hh"

size_t xi_crypto_hash_tree_depth(size_t count) {
  size_t i;
  size_t depth = 0;
  assert(count > 0);
  for (i = sizeof(size_t) << 2; i > 0; i >>= 1) {
    if (count >> i > 0) {
      count >>= i;
      depth += i;
    }
  }
  return depth;
}

int xi_crypto_hash_tree_hash(const xi_byte_t (*hashes)[XI_HASH_FAST_HASH_SIZE], size_t count,
                             xi_crypto_hash_fast rootHash) {
  XI_RETURN_EC_IF_NOT(count > 0, XI_RETURN_CODE_NO_SUCCESS);
  int ec = XI_RETURN_CODE_SUCCESS;
  if (count == 1) {
    memcpy(rootHash, hashes, XI_HASH_FAST_HASH_SIZE);
    return XI_RETURN_CODE_SUCCESS;
  } else if (count == 2) {
    ec = xi_crypto_hash_keccak_256((const xi_byte_t *)hashes, 2 * XI_HASH_FAST_HASH_SIZE, rootHash);
    XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
    return XI_RETURN_CODE_SUCCESS;
  } else {
    size_t i, j;
    size_t cnt = count - 1;
    xi_byte_t(*ints)[XI_HASH_FAST_HASH_SIZE];
    for (i = 1; i < 8 * sizeof(size_t); i <<= 1) {
      cnt |= cnt >> i;
    }
    cnt &= ~(cnt >> 1);
    ints = alloca(cnt * XI_HASH_FAST_HASH_SIZE);
    memcpy(ints, hashes, (2 * cnt - count) * XI_HASH_FAST_HASH_SIZE);
    for (i = 2 * cnt - count, j = 2 * cnt - count; j < cnt; i += 2, ++j) {
      ec = xi_crypto_hash_keccak_256(hashes[i], 2 * XI_HASH_FAST_HASH_SIZE, ints[j]);
      XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
    }
    assert(i == count);
    while (cnt > 2) {
      cnt >>= 1;
      for (i = 0, j = 0; j < cnt; i += 2, ++j) {
        ec = xi_crypto_hash_keccak_256(ints[i], 2 * XI_HASH_FAST_HASH_SIZE, ints[j]);
        XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
      }
    }
    ec = xi_crypto_hash_keccak_256(ints[0], 2 * XI_HASH_FAST_HASH_SIZE, rootHash);
    XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
    return XI_RETURN_CODE_SUCCESS;
  }
}

int xi_crypto_hash_tree_branch(const xi_byte_t (*hashes)[XI_HASH_FAST_HASH_SIZE], size_t count,
                               xi_byte_t (*branch)[XI_HASH_FAST_HASH_SIZE]) {
  size_t i, j;
  size_t cnt = 1;
  size_t depth = 0;
  xi_byte_t(*ints)[XI_HASH_FAST_HASH_SIZE];
  assert(count > 0);
  for (i = sizeof(size_t) << 2; i > 0; i >>= 1) {
    if (cnt << i <= count) {
      cnt <<= i;
      depth += i;
    }
  }
  XI_RETURN_EC_IF_NOT(cnt == 1ULL << depth, XI_RETURN_CODE_NO_SUCCESS);
  XI_RETURN_EC_IF_NOT(depth == xi_crypto_hash_tree_depth(count), XI_RETURN_CODE_NO_SUCCESS);
  ints = alloca((cnt - 1) * XI_HASH_FAST_HASH_SIZE);
  memcpy(ints, hashes + 1, (2 * cnt - count - 1) * XI_HASH_FAST_HASH_SIZE);
  int ec = XI_RETURN_CODE_SUCCESS;
  for (i = 2 * cnt - count, j = 2 * cnt - count - 1; j < cnt - 1; i += 2, ++j) {
    ec = xi_crypto_hash_keccak_256(hashes[i], 2 * XI_HASH_FAST_HASH_SIZE, ints[j]);
    XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
  }
  XI_RETURN_EC_IF_NOT(i == count, XI_RETURN_CODE_NO_SUCCESS);
  while (depth > 0) {
    XI_RETURN_EC_IF_NOT(cnt == 1ULL << depth, XI_RETURN_CODE_NO_SUCCESS);
    cnt >>= 1;
    --depth;
    memcpy(branch[depth], ints[0], XI_HASH_FAST_HASH_SIZE);
    for (i = 1, j = 0; j < cnt - 1; i += 2, ++j) {
      ec = xi_crypto_hash_keccak_256(ints[i], 2 * XI_HASH_FAST_HASH_SIZE, ints[j]);
      XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
    }
  }
  return XI_RETURN_CODE_SUCCESS;
}

int xi_crypto_hashtree_hash_from_branch(const xi_byte_t (*branch)[XI_HASH_FAST_HASH_SIZE], size_t depth,
                                        const xi_byte_t *leaf, const xi_byte_t *path, xi_byte_t *root_hash) {
  if (depth == 0) {
    memcpy(root_hash, leaf, XI_HASH_FAST_HASH_SIZE);
    return XI_RETURN_CODE_SUCCESS;
  } else {
    int ec = XI_RETURN_CODE_SUCCESS;
    xi_byte_t buffer[2][XI_HASH_FAST_HASH_SIZE];
    int from_leaf = 1;
    xi_byte_t *leaf_path, *branch_path;
    while (depth > 0) {
      --depth;
      if (path && (((const xi_byte_t *)path)[depth >> 3] & (1 << (depth & 7))) != 0) {
        leaf_path = buffer[1];
        branch_path = buffer[0];
      } else {
        leaf_path = buffer[0];
        branch_path = buffer[1];
      }
      if (from_leaf) {
        memcpy(leaf_path, leaf, XI_HASH_FAST_HASH_SIZE);
        from_leaf = 0;
      } else {
        ec = xi_crypto_hash_keccak_256((const xi_byte_t *)buffer, 2 * XI_HASH_FAST_HASH_SIZE, leaf_path);
        XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
      }
      memcpy(branch_path, branch[depth], XI_HASH_FAST_HASH_SIZE);
    }
    ec = xi_crypto_hash_keccak_256((const xi_byte_t *)buffer, 2 * XI_HASH_FAST_HASH_SIZE, root_hash);
    XI_RETURN_EC_IF_NOT(ec == XI_RETURN_CODE_SUCCESS, ec);
    return XI_RETURN_CODE_SUCCESS;
  }
}
