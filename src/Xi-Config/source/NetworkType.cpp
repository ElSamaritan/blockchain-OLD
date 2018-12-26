#include "Xi/Config/NetworkType.h"

#include <stdexcept>

static_assert(
    static_cast<uint8_t>(Xi::Config::Network::Type::MainNet) == 0,
    "Their values are used as offset for fundamental configurations, changing those will break the entire chain.");
static_assert(
    static_cast<uint8_t>(Xi::Config::Network::Type::StageNet) == 1,
    "Their values are used as offset for fundamental configurations, changing those will break the entire chain.");
static_assert(
    static_cast<uint8_t>(Xi::Config::Network::Type::TestNet) == 2,
    "Their values are used as offset for fundamental configurations, changing those will break the entire chain.");
static_assert(
    static_cast<uint8_t>(Xi::Config::Network::Type::LocalTestNet) == 3,
    "Their values are used as offset for fundamental configurations, changing those will break the entire chain.");

std::string Xi::to_string(Xi::Config::Network::Type type) {
  switch (type) {
    case Config::Network::MainNet:
      return "MainNet";
    case Config::Network::StageNet:
      return "StageNet";
    case Config::Network::TestNet:
      return "TestNet";
    case Config::Network::LocalTestNet:
      return "LocalTestNet";
    default:
      return "Unknown";
  }
}

namespace Xi {
template <>
Xi::Config::Network::Type lexical_cast<::Xi::Config::Network::Type>(const std::string &str) {
  if (str == to_string(Config::Network::MainNet))
    return Config::Network::MainNet;
  else if (str == to_string(Config::Network::StageNet))
    return Config::Network::StageNet;
  else if (str == to_string(Config::Network::TestNet))
    return Config::Network::TestNet;
  else if (str == to_string(Config::Network::LocalTestNet))
    return Config::Network::LocalTestNet;
  else
    throw std::runtime_error{"Unknown network type string."};
}
}  // namespace Xi
