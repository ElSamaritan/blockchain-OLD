#include "Xi/Config/Coin.h"

std::string Xi::Config::Coin::genesisTransactionBlob(Network::Type network) {
  switch (network) {
    case Network::Type::TestNet:
      return "010a01ff00018090cad2c60e025f2a13a73e823492dd8e330c6b1abae00e9451c65d53bd01466f8947a51be05b2101d49c18673d6"
             "d5be6987d34002c32f0aef3c051cf72a7ecf8bd84c9dca753c9a6";
    case Network::Type::LocalTestNet:
      return "010a01ff00018090cad2c60e026b01359fb1eeb29512936b9a68399703a1d9f5777a4d4f2c47deb58a43a0b20b2101085c51a5428"
             "e1d65bde9c19510beccbc6fb47e9e618b6c5a412f97fbef72cf1e";
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
