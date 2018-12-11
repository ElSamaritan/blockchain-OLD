#pragma once

#include <cinttypes>

#include "config/_Impl/BeginBlockVersion.h"

// clang-format off
//                        (_Index, _Height,  _Version,  _IsFork)
MakeBlockVersionCheckpoint(    0,      0,         1,   false)
MakeBlockVersionCheckpoint(    1,      1,         2,   false)
MakeBlockVersionCheckpoint(    2,      2,         3,   false)
MakeBlockVersionCheckpoint(    3,      3,         4,   false)
// clang-format on

#define CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX 3

namespace CryptoNote {
namespace Config {
namespace BlockVersion {
inline constexpr uint8_t minorVersionNoVotingIndicator() { return 0; }
inline constexpr uint8_t minorVersionVotingIndicator() { return 1; }
inline constexpr bool validateMinorVersion(uint8_t minorVersion) {
  return minorVersion == minorVersionNoVotingIndicator() || minorVersion == minorVersionVotingIndicator();
}
}
}
}

#include "config/_Impl/EndBlockVersion.h"
