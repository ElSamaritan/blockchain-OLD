#include "Xi/Config/Network.h"

#include <Xi/Version.h>

Xi::Config::Network::Type Xi::Config::Network::defaultNetworkType() {
#ifndef XI_DEV_VERSION
  return MainNet;
#elif defined(_DEBUG)
  return LocalTestNet;
#else
  return TestNet;
#endif
}

boost::uuids::uuid Xi::Config::Network::identifier(Xi::Config::Network::Type network) {
  switch (network) {
    case MainNet:
      return boost::uuids::uuid{
          {0x73, 0x87, 0xd0, 0xc6, 0x6f, 0xde, 0x4f, 0x8c, 0x4a, 0xb2, 0x12, 0xb7, 0x5e, 0x3f, 0xfb, 0x17}};
    case StageNet:
      return boost::uuids::uuid{
          {0x2d, 0x63, 0x08, 0x48, 0x86, 0x8e, 0x74, 0x23, 0xa8, 0x02, 0x3e, 0x85, 0xeb, 0xf4, 0xab, 0xf1}};
    case TestNet:
      return boost::uuids::uuid{
          {0xb2, 0x51, 0x1c, 0x75, 0x61, 0x56, 0x21, 0x55, 0xf7, 0xd7, 0x19, 0x36, 0xe2, 0x61, 0xf7, 0xbb}};
    case LocalTestNet:
      return boost::uuids::uuid{
          {0x84, 0xb9, 0x3f, 0xac, 0x31, 0xce, 0x11, 0x9c, 0x7f, 0x39, 0x86, 0xa7, 0xef, 0xd0, 0x3a, 0xb3}};
    default:
      throw std::runtime_error{"Unknown network type."};
  }
}

std::vector<std::string> Xi::Config::Network::seedNodes(Type network) {
  static const char* _SeedNodes[] = {"207.180.240.151:22868", "207.180.240.152:22868"};
  static const char* _LocalNodes[] = {"127.0.0.1:22868"};
  if (network == LocalTestNet)
    return std::vector<std::string>(_LocalNodes, std::end(_LocalNodes));
  else
    return std::vector<std::string>(_SeedNodes, std::end(_SeedNodes));
}

std::string Xi::Config::Network::breakpadServer() { return "207.180.240.156"; }
