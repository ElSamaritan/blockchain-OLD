#include "Xi/Config/Coin.h"

std::string Xi::Config::Coin::genesisTransactionBlob(Network::Type network) {
  switch (network) {
      // clang-format off
    case Network::Type::LocalTestNet:
    case Network::Type::TestNet:
    case Network::Type::StageNet:
    case Network::Type::MainNet:
      return "010a0101010180f0efe8812001f0d5cfd8dd80412a15b95e38d5956f95b3beee7c36a799306e43de4db6f7c387210118b94aa5be09d291adbe9362c2bac77a4289e06c8107d29721800aed49868f8c";
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
