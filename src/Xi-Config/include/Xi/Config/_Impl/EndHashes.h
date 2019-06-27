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

#pragma once

#include <cinttypes>

#include <crypto/CryptoTypes.h>

#include "Xi/Config/Hashes.h"

#undef MakeHashCheckpoint

#ifndef CURRENT_HASH_CHECKPOINT_INDEX
#pragma error "CURRENT_HASH_CHECKPOINT_INDEX must be defined"
#endif

namespace Xi {
namespace Config {
namespace Hashes {

struct HashCheckpointResolver {
  template <Blockchain::Block::Version::value_type>
  static inline void compute(const CryptoNote::CachedBlock& block, ::Crypto::Hash& hash,
                             Blockchain::Block::Version version);
};

template <>
inline void HashCheckpointResolver::compute<0>(const CryptoNote::CachedBlock& block, ::Crypto::Hash& hash,
                                               Blockchain::Block::Version) {
  typename HashCheckpoint<0>::algorithm hashFn{};
  hashFn(block, hash);
}
template <Blockchain::Block::Version::value_type _Index>
inline void HashCheckpointResolver::compute(const CryptoNote::CachedBlock& block, ::Crypto::Hash& hash,
                                            Blockchain::Block::Version version) {
  if (version >= HashCheckpoint<_Index>::version()) {
    typename HashCheckpoint<_Index>::algorithm hashFn{};
    hashFn(block, hash);
  } else
    return compute<_Index - 1>(block, hash, version);
}

inline void compute(const CryptoNote::CachedBlock& block, ::Crypto::Hash& hash, Blockchain::Block::Version version) {
  HashCheckpointResolver::compute<CURRENT_HASH_CHECKPOINT_INDEX>(block, hash, version);
}

}  // namespace Hashes
}  // namespace Config
}  // namespace Xi

#undef CURRENT_HASH_CHECKPOINT_INDEX
