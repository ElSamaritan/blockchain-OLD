#include "Xi/Config/Network.h"

#include <Xi/Version/BuildInfo.h>
#include <Xi/Resource/Resource.hpp>

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

std::string Xi::Config::Network::breakpadServer() {
  return "207.180.240.156";
}

std::string Xi::Config::Network::defaultNetwork() {
  using namespace Xi::Resource;

  switch (defaultNetworkType()) {
    case LocalTestNet:
      return resourcePath("chains/galaxia/local");
    case TestNet:
      return resourcePath("chains/galaxia/test");
    case StageNet:
      exceptional<NotFoundError>("staging network is currently not available");
    case MainNet:
      return resourcePath("chains/galaxia");
  }

  exceptional<InvalidEnumValueError>("Unknown network type.");
}

std::string Xi::Config::Network::checkpoints(const Xi::Config::Network::Type network) {
  using namespace Xi::Resource;

  switch (network) {
    case LocalTestNet:
      return resourcePath("checkpoints/galaxia/local");
    case TestNet:
      return resourcePath("checkpoints/galaxia/test");
    case StageNet:
      return resourcePath("checkpoints/galaxia/staging");
    case MainNet:
      return resourcePath("checkpoints/galaxia");
  }

  exceptional<InvalidEnumValueError>("Unknown network type.");
}
