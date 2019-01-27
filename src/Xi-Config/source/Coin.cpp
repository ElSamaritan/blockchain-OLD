#include "Xi/Config/Coin.h"

std::string Xi::Config::Coin::genesisTransactionHash(Network::Type network) {
  switch (network) {
    case Network::Type::TestNet:
      return "010a01ff000180d0cfba8560025b2d2ee86de45e3ba741e074959a3e8287492496128bfd7faec62d42d917f0b821018fa97999f30"
             "ef64ca6f7b82c989893af503539b5d27e8855c4435ada875b8c22";
    case Network::Type::LocalTestNet:
      return "010a01ff000180d0cfba856002dd217151a9b4a8eb649f7a3b4f9e6297edeb18f660ae7d37f6ccf2d670a66e9621013dd05540b96"
             "b429d0afa30d2767120f1b4e5236cedc15c238dd8820a97fca06d";

    default:
      break;
  }
  throw std::runtime_error{"unexpected network type, cannot provide genesis transaction hash."};
}
