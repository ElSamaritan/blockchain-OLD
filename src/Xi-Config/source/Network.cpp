#include "Xi/Config/Network.h"

#include <Xi/Version/BuildInfo.h>

Xi::Config::Network::Type Xi::Config::Network::defaultNetworkType() {
#if !defined(NDEBUG)
  return LocalTestNet;
#elif defined(BUILD_CHANNEL_CLUTTER) || defined(BUILD_CHANNEL_EDGE)
  return TestNet;
#elif defined(BUILD_CHANNEL_BETA)
  return StageNet;
#elif defined(BUILD_CHANNEL_STABLE)
  return MainNet;
#endif
}

std::string Xi::Config::Network::breakpadServer() { return "207.180.240.156"; }

std::string Xi::Config::Network::defaultNetworkName() { return "Galaxia"; }

std::string Xi::Config::Network::defaultNetworkIdentifier() {
  return defaultNetworkName() + "." + toString(defaultNetworkType());
}
