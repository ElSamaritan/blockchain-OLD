#include "Xi/Config/Coin.h"

std::string Xi::Config::Coin::genesisTransactionHash(Network::Type network) {
  switch (network) {
    case Network::Type::TestNet:
      return "010a01ff000180d0cfba856002137ac827c55c37f9c5efa93ea3971584e50e3189e4fbe8429fb0dba2422ee62b21012cee5c95535"
             "fe90d92a80bf6b2c7c6f33491be5c8f311c9fd517debd87d79561";
    case Network::Type::LocalTestNet:
      return "010a01ff000180d0cfba856002dd217151a9b4a8eb649f7a3b4f9e6297edeb18f660ae7d37f6ccf2d670a66e9621013dd05540b96"
             "b429d0afa30d2767120f1b4e5236cedc15c238dd8820a97fca06d";
    case Network::Type::StageNet:
      return "010a01ff000180d0cfba8560026a98f4c224b44c0b7f8d2691506e463106dcf156123d12cead028148c47c817221014f56085f13a"
             "7fd31c8643e1e3eca5d4a6bf14110f82fe035507abd94decc290a";

    default:
      break;
  }
  throw std::runtime_error{"unexpected network type, cannot provide genesis transaction hash."};
}
