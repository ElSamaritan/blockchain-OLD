#pragma once

#include <cinttypes>

#ifndef CURRENT_MIXIN_CHECKPOINT_INDEX
#pragma error "CURRENT_MIXIN_CHECKPOINT_INDEX must be defined."
#endif

#undef MakeMixinCheckpoint

namespace CryptoNote {
namespace Config {
namespace Mixin {

struct MixinCheckpointResolver {
  template<uint8_t _Index>
  static inline bool isZeroMixinAllowed(uint32_t height);

  template<uint8_t _Index>
  static inline uint16_t minimum(uint32_t height);

  template<uint8_t _Index>
  static inline uint16_t maximum(uint32_t height);

  template<uint8_t _Index>
  static inline uint16_t defaultValue(uint32_t height);
};

template<> inline bool MixinCheckpointResolver::isZeroMixinAllowed<0>(uint32_t) {
  return MixinCheckpoint<0>::minimum() == 0;
}
template<uint8_t _Index> inline bool MixinCheckpointResolver::isZeroMixinAllowed(uint32_t height) {
  if(height >= MixinCheckpoint<_Index>::height())
    return MixinCheckpoint<_Index>::minimum() == 0;
  else
    return isZeroMixinAllowed<_Index - 1>(height);
}

template<> inline uint16_t MixinCheckpointResolver::minimum<0>(uint32_t) { return MixinCheckpoint<0>::minimum(); }
template<uint8_t _Index> inline uint16_t MixinCheckpointResolver::minimum(uint32_t height) {
  if(height >= MixinCheckpoint<_Index>::height())
    return MixinCheckpoint<_Index>::minimum();
  else
    return minimum<_Index - 1>(height);
}

template<> inline uint16_t MixinCheckpointResolver::maximum<0>(uint32_t) { return MixinCheckpoint<0>::minimum(); }
template<uint8_t _Index> inline uint16_t MixinCheckpointResolver::maximum(uint32_t height) {
  if(height >= MixinCheckpoint<_Index>::height())
    return MixinCheckpoint<_Index>::maximum();
  else
    return maximum<_Index - 1>(height);
}

template<> inline uint16_t MixinCheckpointResolver::defaultValue<0>(uint32_t) {
  return MixinCheckpoint<0>::defaultValue();
}
template<uint8_t _Index> inline uint16_t MixinCheckpointResolver::defaultValue(uint32_t height) {
  if(height >= MixinCheckpoint<_Index>::height())
    return MixinCheckpoint<_Index>::defaultValue();
  else
    return maximum<_Index - 1>(height);
}

inline bool isZeroMixinAllowed(uint32_t height) {
  return MixinCheckpointResolver::isZeroMixinAllowed<CURRENT_MIXIN_CHECKPOINT_INDEX>(height);
}
inline uint16_t minimum(uint32_t height) {
  return MixinCheckpointResolver::minimum<CURRENT_MIXIN_CHECKPOINT_INDEX>(height);
}
inline uint16_t maximum(uint32_t height) {
  return MixinCheckpointResolver::maximum<CURRENT_MIXIN_CHECKPOINT_INDEX>(height);
}
inline uint16_t defaultValue(uint32_t height) {
  return MixinCheckpointResolver::defaultValue<CURRENT_MIXIN_CHECKPOINT_INDEX>(height);
}

}
}
}

#undef CURRENT_MIXIN_CHECKPOINT_INDEX
