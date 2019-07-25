// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <memory>

#include <Xi/Crypto/PasswordContainer.h>

#include "WalletServiceConfiguration.h"
#include "P2p/NetNodeConfig.h"

namespace PaymentService {

class ConfigurationManager {
 public:
  ConfigurationManager();
  bool init(int argc, char** argv);

  WalletServiceConfiguration serviceConfig;

  std::unique_ptr<Xi::Crypto::PasswordContainer> rpcSecret;
};

}  // namespace PaymentService
