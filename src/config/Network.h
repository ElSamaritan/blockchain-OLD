#pragma once

#include <string>
#include <vector>

#include <boost/uuid/uuid.hpp>

#include <Xi/Utils/Conversion.h>

#include "config/Limits.h"

namespace CryptoNote {
namespace Config {
namespace Network {
static inline const boost::uuids::uuid identifier() {
  return boost::uuids::uuid{
      {0x74, 0x68, 0x69, 0x73, 0x69, 0x73, 0x61, 0x74, 0x65, 0x73, 0x74, 0x66, 0x6f, 0x72, 0x63, 0x6e}};
}

static inline constexpr uint16_t p2pPort() { return 22868; }
static inline constexpr uint16_t rpcPort() { return 22869; }
static inline constexpr uint16_t pgPort() { return 38070; }

static inline std::vector<std::string> seedNodes() {
#if defined(_DEBUG)
  static const char* _SeedNodes[] = {"127.0.0.1:22868"};
#else
  static const char* _SeedNodes[] = {"207.180.240.151:22868", "207.180.240.152:22868"};
#endif
  return std::vector<std::string>(_SeedNodes, std::end(_SeedNodes));
}

static inline constexpr uint64_t blockIdentifiersSynchronizationBatchSize() { return 500; }
static inline constexpr uint64_t blocksSynchronizationBatchSize() { return 100; }
}  // namespace Network
}  // namespace Config
}  // namespace CryptoNote
