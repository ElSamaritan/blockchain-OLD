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
          {0x2b, 0x87, 0x17, 0x0e, 0x0d, 0x78, 0x5e, 0xa1, 0x01, 0xd5, 0xa6, 0x38, 0x06, 0x7f, 0x26, 0x52}};
    case LocalTestNet:
      return boost::uuids::uuid{
          {0x84, 0xb9, 0x3f, 0xac, 0x31, 0xce, 0x11, 0x9c, 0x7f, 0x39, 0x86, 0xa7, 0xef, 0xd0, 0x3a, 0xb3}};
    default:
      throw std::runtime_error{"Unknown network type."};
  }
}

std::vector<std::string> Xi::Config::Network::seedNodes(Type network) {
  static const char* _SeedNodes[] = {"207.180.240.151:22868", "207.180.240.152:22868"};
  static const char* _LocalNodes[] = {"xi-daemon:22868"};
  if (network == LocalTestNet)
    return std::vector<std::string>(_LocalNodes, std::end(_LocalNodes));
  else
    return std::vector<std::string>(_SeedNodes, std::end(_SeedNodes));
}
