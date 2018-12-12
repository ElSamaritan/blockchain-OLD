#pragma once

#include <cinttypes>

#include "config/Dust.h"

#undef MakeDustCheckpoint

#ifndef CURRENT_DUST_CHECKPOINT_INDEX
#pragma error "CURRENT_DUST_CHECKPOINT_INDEX must be defines"
#endif

namespace CryptoNote {
namespace Config {
namespace Dust {

struct DustCheckpointResolver {
  template<uint8_t>
  static inline uint64_t dust(uint8_t version);
};

template<> inline uint64_t DustCheckpointResolver::dust<0>(uint8_t) {
  return DustCheckpoint<0>::dust();
}
template<uint8_t _Index> inline uint64_t DustCheckpointResolver::dust(uint8_t version) {
  if(version >= DustCheckpoint<_Index>::version())
    return DustCheckpoint<_Index>::dust();
  else
    return dust<_Index - 1>(version);
}

inline uint64_t dust(uint8_t version) {
  return DustCheckpointResolver::dust<CURRENT_DUST_CHECKPOINT_INDEX>(version);
}

}
}
}

#undef CURRENT_DUST_CHECKPOINT_INDEX

#ifndef CURRENT_FUSION_DUST_CHECKPOINT_INDEX
#pragma error "CURRENT_FUSION_DUST_CHECKPOINT_INDEX must be defines"
#endif

#undef MakeFusionDustCheckpoint

namespace CryptoNote {
namespace Config {
namespace Dust {

struct FusionDustCheckpointResolver {
  template<uint8_t>
  static inline uint64_t dust(uint8_t version);
};

template<> inline uint64_t FusionDustCheckpointResolver::dust<0>(uint8_t) {
  return FusionDustCheckpoint<0>::dust();
}
template<uint8_t _Index> inline uint64_t FusionDustCheckpointResolver::dust(uint8_t version) {
  if(version >= FusionDustCheckpoint<_Index>::version())
    return FusionDustCheckpoint<_Index>::dust();
  else
    return dust<_Index - 1>(version);
}

inline uint64_t fusionDust(uint8_t version) {
  return FusionDustCheckpointResolver::dust<CURRENT_FUSION_DUST_CHECKPOINT_INDEX>(version);
}

}
}
}

#undef CURRENT_FUSION_DUST_CHECKPOINT_INDEX
