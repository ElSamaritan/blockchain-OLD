#pragma once

#include <cinttypes>
#include <chrono>
#include <string>

#include "config/Network.h"

namespace CryptoNote {
namespace Config {
namespace P2P {
static inline constexpr uint16_t defaultPort() { return Network::p2pPort(); }

static inline constexpr uint8_t minimumVersion() { return 1; }
static inline constexpr uint8_t currentVersion() { return 1; }
static_assert (currentVersion() >= minimumVersion(), "The current P2P version must satisfy the minimum version.");

static inline constexpr uint8_t upgradeNotificationWindow() { return 2; }
static inline constexpr uint32_t connectionsCount() { return 8; }
static inline std::string trustedPublicKey() { return ""; }

static inline constexpr size_t maximumWhiteListedPeers() { return 1000; }
static inline constexpr uint8_t whiteListPreferenceThreshold() { return 70; }
static inline constexpr size_t maximumGrayListedPeers() { return 5000; }

static inline constexpr size_t maximumWriteBufferSize() { return 32_MB; }
static inline constexpr uint32_t maximumPackageSize() { return static_cast<uint32_t>(50_MB); }

static inline constexpr std::chrono::seconds handshakeInterval() { return std::chrono::seconds{60}; }
static inline constexpr std::chrono::seconds handshakeTimeout() { return std::chrono::seconds{5}; }

static inline std::string stateBackupFilename() { return "xi-p2p.bin"; }

/*!
 * \brief handshakePeersCount is the amount of known peers exchanged during a handshake.
 */
static inline constexpr uint32_t handshakePeersCount() { return 250; }

static inline constexpr std::chrono::seconds connectionTimeout() { return std::chrono::seconds{5}; }
static inline constexpr std::chrono::seconds pingTimeout() { return std::chrono::seconds{2}; }
static inline constexpr std::chrono::minutes invokeTimeout() { return std::chrono::minutes{2}; }

static_assert(handshakeTimeout().count() > 0, "Timeouts must be non-zer and non-negative.");
static_assert(connectionTimeout().count() > 0, "Timeouts must be non-zer and non-negative.");
static_assert(pingTimeout().count() > 0, "Timeouts must be non-zer and non-negative.");
static_assert(invokeTimeout().count() > 0, "Timeouts must be non-zer and non-negative.");
}
}
}
