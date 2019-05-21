#include "Xi/Config/Coin.h"

std::string Xi::Config::Coin::genesisTransactionBlob(Network::Type network) {
  switch (network) {
    case Network::Type::TestNet:
      return "010a01ff000180c0ee8ed20b0223e9a390c61bacc2d3f9446209edc4435a4b78950bac0c00fed5693b098999b6210192fc7c22e5f"
             "b0b95dc561229fa7ddff7ba8a92ea27066ed85ea64ea71557d5d8";
    case Network::Type::LocalTestNet:
      return "010a01ff000180d0cfba85600238da436fd268a778fb6327d666ac21f109cd7769a1407893fa83823cc93eb9212101ebd59b2d595"
             "e95ce2753e6a4f9f3da4e1abf3d4a285b4996cbe343a9d47b3933";
    case Network::Type::StageNet:
      return "010a01ff000180d0cfba8560026293a2293d0f5be345ec2d1e9c9eef0dd977fa6243cc9d003fa0ae5a83d040cc2101a9cccb50808"
             "fbb05d3a498880f261c7c106ee9e356b94174c117c816171f6215";

    default:
      break;
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
