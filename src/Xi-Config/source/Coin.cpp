#include "Xi/Config/Coin.h"

std::string Xi::Config::Coin::genesisTransactionBlob(Network::Type network) {
  switch (network) {
      // clang-format off
    case Network::Type::LocalTestNet:
    case Network::Type::TestNet:
    case Network::Type::StageNet:
    case Network::Type::MainNet:
      return "010a010101018090cad2c60e01bc6ad371bba99ff013493a859e5cb2e4d3e9737ab4dee629c630c582c531b4ed2101170dea227bd20c6630036dcff07eeb7095991fc6ac67a15d68594d67e69593ae";
      // clang-format on
  }
  throw std::runtime_error{"unexpected network type, cannot provide genesis transaction hash."};
}

uint64_t Xi::Config::Coin::genesisTimestamp(Xi::Config::Network::Type network) {
  switch (network) {
    case Network::Type::StageNet:
      return 1549720565;
    case Network::Type::TestNet:
      return 1558439939;
    case Network::Type::LocalTestNet:
      return 1544396293;
    default:
      return 0;
  }
}
