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
  template <uint8_t _Index>
  static inline bool isZeroMixinAllowed(uint8_t version);

  template <uint8_t _Index>
  static inline uint16_t minimum(uint8_t version);

  template <uint8_t _Index>
  static inline uint16_t maximum(uint8_t version);

  template <uint8_t _Index>
  static inline uint16_t defaultValue(uint8_t version);
};

template <>
inline bool MixinCheckpointResolver::isZeroMixinAllowed<0>(uint8_t) {
  return MixinCheckpoint<0>::minimum() == 0;
}
template <uint8_t _Index>
inline bool MixinCheckpointResolver::isZeroMixinAllowed(uint8_t version) {
  if (version >= MixinCheckpoint<_Index>::version())
    return MixinCheckpoint<_Index>::minimum() == 0;
  else
    return isZeroMixinAllowed<_Index - 1>(version);
}

template <>
inline uint16_t MixinCheckpointResolver::minimum<0>(uint8_t) {
  return MixinCheckpoint<0>::minimum();
}
template <uint8_t _Index>
inline uint16_t MixinCheckpointResolver::minimum(uint8_t version) {
  if (version >= MixinCheckpoint<_Index>::version())
    return MixinCheckpoint<_Index>::minimum();
  else
    return minimum<_Index - 1>(version);
}

template <>
inline uint16_t MixinCheckpointResolver::maximum<0>(uint8_t) {
  return MixinCheckpoint<0>::minimum();
}
template <uint8_t _Index>
inline uint16_t MixinCheckpointResolver::maximum(uint8_t version) {
  if (version >= MixinCheckpoint<_Index>::version())
    return MixinCheckpoint<_Index>::maximum();
  else
    return maximum<_Index - 1>(version);
}

template <>
inline uint16_t MixinCheckpointResolver::defaultValue<0>(uint8_t) {
  return MixinCheckpoint<0>::defaultValue();
}
template <uint8_t _Index>
inline uint16_t MixinCheckpointResolver::defaultValue(uint8_t version) {
  if (version >= MixinCheckpoint<_Index>::version())
    return MixinCheckpoint<_Index>::defaultValue();
  else
    return maximum<_Index - 1>(version);
}

inline bool isZeroMixinAllowed(uint8_t version) {
  return MixinCheckpointResolver::isZeroMixinAllowed<CURRENT_MIXIN_CHECKPOINT_INDEX>(version);
}
inline uint16_t minimum(uint8_t version) {
  return MixinCheckpointResolver::minimum<CURRENT_MIXIN_CHECKPOINT_INDEX>(version);
}
inline uint16_t maximum(uint8_t version) {
  return MixinCheckpointResolver::maximum<CURRENT_MIXIN_CHECKPOINT_INDEX>(version);
}
inline uint16_t defaultValue(uint8_t version) {
  return MixinCheckpointResolver::defaultValue<CURRENT_MIXIN_CHECKPOINT_INDEX>(version);
}

}  // namespace Mixin
}  // namespace Config
}  // namespace CryptoNote

#undef CURRENT_MIXIN_CHECKPOINT_INDEX
