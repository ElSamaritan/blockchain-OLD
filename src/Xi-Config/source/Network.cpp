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

boost::uuids::uuid Xi::Config::Network::identifier(Xi::Config::Network::Type network) {
  switch (network) {
    case MainNet:
      return boost::uuids::uuid{
          {0x73, 0x87, 0xd0, 0xc6, 0x6f, 0xde, 0x4f, 0x8c, 0x4a, 0xb2, 0x12, 0xb7, 0x5e, 0x3f, 0xfb, 0x17}};
    case StageNet:
      return boost::uuids::uuid{
          {0xd0, 0x00, 0x29, 0xb2, 0xea, 0x8f, 0x17, 0xe3, 0xf9, 0x02, 0x74, 0xe3, 0x02, 0x12, 0x30, 0x73}};
    case TestNet:
      return boost::uuids::uuid{
          {0xdc, 0x58, 0x03, 0x66, 0x6d, 0xf5, 0x5b, 0x0f, 0xb8, 0x38, 0x89, 0xcb, 0x40, 0xd9, 0x1f, 0x2e}};
    case LocalTestNet:
      return boost::uuids::uuid{
          {0x84, 0xb9, 0x3f, 0xac, 0x31, 0xce, 0x11, 0x9c, 0x7f, 0x39, 0x86, 0xa7, 0xef, 0xd0, 0x3a, 0xb3}};
    default:
      throw std::runtime_error{"Unknown network type."};
  }
}

std::vector<std::string> Xi::Config::Network::seedNodes(Type network) {
  if (network == LocalTestNet)
    return std::vector<std::string>({"127.0.0.1:22868"});
  else if (network == TestNet)
    return std::vector<std::string>({"207.180.228.241:22868", "207.180.224.162:22868"});
  else if (network == StageNet)
    return std::vector<std::string>({
        "51.75.88.135:22868", /* seed 001: frankfurt */
        "54.39.182.201:22868" /* seed 002: canada    */
    });
  else if (network == MainNet)
    return std::vector<std::string>({
        "207.180.228.241:22868", /* seed 001: frankfurt */
        "207.180.224.162:22868"  /* seed 002: frankfurt */
    });
  else
    return {};
}

std::string Xi::Config::Network::breakpadServer() { return "207.180.240.156"; }
