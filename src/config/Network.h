#pragma once

#include <boost/uuid/uuid.hpp>

namespace CryptoNote {
namespace Config {
namespace Network {
static inline const boost::uuids::uuid identifier() {
  return boost::uuids::uuid{
    {0x74, 0x68, 0x69, 0x73, 0x69, 0x73, 0x61, 0x74, 0x65, 0x73, 0x74, 0x66, 0x6f, 0x72, 0x63, 0x6e}
  };
}

static inline constexpr uint16_t p2pPort() { return 22868; }
static inline constexpr uint16_t rpcPort() { return 22869; }
static inline constexpr uint16_t pgPort() { return 38070; }
}  // namespace Network
}
}
