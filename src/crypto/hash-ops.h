// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#define XI_CRYPTO_HASH_KECCAK_HASH_SIZE 32
#define XI_CRYPTO_HASH_KECCAK_HASH_DATA_AREA 136

enum { HASH_SIZE = XI_CRYPTO_HASH_KECCAK_HASH_SIZE, HASH_DATA_AREA = XI_CRYPTO_HASH_KECCAK_HASH_DATA_AREA };

void tree_hash(const char (*hashes)[HASH_SIZE], size_t count, char *root_hash);
size_t tree_depth(size_t count);
void tree_branch(const char (*hashes)[HASH_SIZE], size_t count, char (*branch)[HASH_SIZE]);
void tree_hash_from_branch(const char (*branch)[HASH_SIZE], size_t depth, const char *leaf, const void *path,
                           char *root_hash);

#if defined(__cplusplus)
}
#endif
