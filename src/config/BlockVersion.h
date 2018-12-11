#pragma once

#include <cinttypes>

#include "config/_Impl/BeginBlockVersion.h"

namespace CryptoNote {
namespace Config {
namespace BlockVersion {
inline constexpr uint8_t minorVersionNoVotingIndicator() { return 0; }
inline constexpr uint8_t minorVersionVotingIndicator() { return 1; }
inline constexpr bool validateMinorVersion(uint8_t minorVersion) {
  return minorVersion == minorVersionNoVotingIndicator() || minorVersion == minorVersionVotingIndicator();
}
}  // namespace BlockVersion
}  // namespace Config
}  // namespace CryptoNote

// clang-format off
//                        (_Index, _Height,  _Version,  _IsFork)
MakeBlockVersionCheckpoint(     0,       0,         1,    false)
MakeBlockVersionCheckpoint(     1,       1,         2,    false)
MakeBlockVersionCheckpoint(     2,       2,         3,    false)
MakeBlockVersionCheckpoint(     3,       3,         4,    false)
MakeBlockVersionCheckpoint(     4,    1000,         5,    false)
MakeBlockVersionCheckpoint(     5,    3000,         6,     true)
MakeBlockVersionCheckpoint(     6,   10000,         7,    false)
// clang-format on

#define CURRENT_BLOCK_VERSION_CHECKPOINT_INDEX 6

#include "config/_Impl/EndBlockVersion.h"
