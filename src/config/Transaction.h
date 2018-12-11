#pragma once

#include <cinttypes>

namespace CryptoNote {
namespace Config {
namespace Transaction {

/*!
 * \brief version yields the current version to be used, can later be adjusted to take height into account.
 */
inline constexpr uint8_t version() { return 1; }

inline constexpr uint8_t minimumVersion() { return 1; }
inline constexpr uint8_t maximumVersion() { return 1; }

inline constexpr uint32_t keyCheckingActivitationBlockIndex() { return 1; }

}
}
}
