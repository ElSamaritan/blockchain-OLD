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
 *                                                                                                *
 * ============================================================================================== */

#pragma once

#include "Xi/Crypto/Hash/FastHash.hh"

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdlib.h>

size_t xi_crypto_hash_tree_depth(size_t count);
int xi_crypto_hash_tree_hash(const xi_byte_t (*hashes)[XI_HASH_FAST_HASH_SIZE], size_t count,
                             xi_crypto_hash_fast rootHash);
int xi_crypto_hash_tree_branch(const xi_byte_t (*hashes)[XI_HASH_FAST_HASH_SIZE], size_t count,
                               xi_byte_t (*branch)[XI_HASH_FAST_HASH_SIZE]);
int xi_crypto_hashtree_hash_from_branch(const xi_byte_t (*branch)[XI_HASH_FAST_HASH_SIZE], size_t depth,
                                        const xi_byte_t *leaf, const xi_byte_t *path, xi_byte_t *root_hash);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

namespace Xi {
namespace Crypto {
namespace Hash {

void treeHash(const ConstByteSpan &data, size_t count, ByteSpan out);

}
}  // namespace Crypto
}  // namespace Xi

#endif
