#pragma once

#include <cinttypes>

#include "config/Hashes.h"

#undef MakeHashCheckpoint

#ifndef CURRENT_HASH_CHECKPOINT_INDEX
#pragma error "CURRENT_HASH_CHECKPOINT_INDEX must be defined"
#endif

namespace CryptoNote {
namespace Config {
namespace Hashes {

struct HashCheckpointResolver {
  template<uint8_t>
  static inline void compute(const CachedBlock& block, Crypto::Hash& hash, uint8_t version);
};

template<> inline void HashCheckpointResolver::compute<0>(const CachedBlock& block, Crypto::Hash& hash, uint8_t)
{
  typename HashCheckpoint<0>::algorithm hashFn{};
  hashFn(block, hash);
}
template<uint8_t _Index> inline void HashCheckpointResolver::compute(
    const CachedBlock& block, Crypto::Hash& hash, uint8_t version) {
  if(version >= HashCheckpoint<_Index>::version()) {
    typename HashCheckpoint<_Index>::algorithm hashFn{};
    hashFn(block, hash);
  }
  else
    return compute<_Index - 1>(block, hash, version);
}

inline void compute(const CachedBlock& block, Crypto::Hash& hash, uint8_t version) {
  HashCheckpointResolver::compute<CURRENT_HASH_CHECKPOINT_INDEX>(block, hash, version);
}

}
}
}

#undef CURRENT_HASH_CHECKPOINT_INDEX
