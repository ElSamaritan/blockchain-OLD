﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "ConfigurationManager.h"

#include <iostream>
#include <fstream>

#include <Xi/Crypto/FastHash.hpp>
#include <Xi/Crypto/PublicKey.hpp>
#include <Xi/Crypto/SecretKey.hpp>
#include <Xi/Config.h>

#include "Common/CommandLine.h"
#include "Common/Util.h"
#include "CommonCLI.h"

#include "Logging/ILogger.h"

namespace PaymentService {

ConfigurationManager::ConfigurationManager() {}

bool ConfigurationManager::init(int argc, char** argv) {
  serviceConfig = initConfiguration();

  // Load in the initial CLI options
  handleSettings(argc, argv, serviceConfig);

  // If the user passed in the --config-file option, we need to handle that first
  if (!serviceConfig.configFile.empty()) {
    try {
      handleSettings(serviceConfig.configFile, serviceConfig);
    } catch (std::exception& e) {
      std::cout << std::endl
                << "There was an error parsing the specified configuration file. Please check the file and try again"
                << std::endl
                << e.what() << std::endl;
      exit(1);
    }
  }

  // Load in the CLI specified parameters again to overwrite any given in the config file
  handleSettings(argc, argv, serviceConfig);

  if (serviceConfig.dumpConfig) {
    std::cout << asString(serviceConfig) << std::endl;
    exit(0);
  } else if (!serviceConfig.outputFile.empty()) {
    try {
      asFile(serviceConfig, serviceConfig.outputFile);
      std::cout << "Configuration saved to: " << serviceConfig.outputFile << std::endl;
      exit(0);
    } catch (std::exception& e) {
      std::cout << "Unable to save configuration to: " << serviceConfig.outputFile << std::endl
                << e.what() << std::endl;
      exit(1);
    }
  }

  if (serviceConfig.registerService && serviceConfig.unregisterService) {
    throw std::runtime_error(
        "It's impossible to use both --register-service and --unregister-service at the same time");
  }

  if (serviceConfig.logLevel > Logging::Trace) {
    throw std::runtime_error("log-level must be between " + std::to_string(Logging::Fatal) + ".." +
                             std::to_string(Logging::Trace));
  }

  if (serviceConfig.containerFile.empty()) {
    throw std::runtime_error("You must specify a wallet file to open!");
  }

  if (!std::ifstream(serviceConfig.containerFile) && !serviceConfig.generateNewContainer) {
    if (std::ifstream(serviceConfig.containerFile + ".wallet")) {
      throw std::runtime_error(
          "The wallet file you specified does not exist. Did you mean: " + serviceConfig.containerFile + ".wallet?");
    } else {
      throw std::runtime_error(
          "The wallet file you specified does not exist; please check your spelling and try again.");
    }
  }

  if ((!serviceConfig.secretViewKey.empty() || !serviceConfig.secretSpendKey.empty()) &&
      !serviceConfig.generateNewContainer) {
    throw std::runtime_error("--generate-container is required");
  }

  if (!serviceConfig.mnemonicSeed.empty() && !serviceConfig.generateNewContainer) {
    throw std::runtime_error("--generate-container is required");
  }

  if (!serviceConfig.mnemonicSeed.empty() &&
      (!serviceConfig.secretViewKey.empty() || !serviceConfig.secretSpendKey.empty())) {
    throw std::runtime_error("You cannot specify import from both Mnemonic seed and private keys");
  }

  if ((serviceConfig.registerService || serviceConfig.unregisterService) && serviceConfig.containerFile.empty()) {
    throw std::runtime_error("--container-file parameter is required");
  }

  // If we are generating a new container, we can skip additional checks
  if (serviceConfig.generateNewContainer) {
    return true;
  }

  // Run authentication checks

  if (serviceConfig.rpcPassword.empty() && !serviceConfig.legacySecurity) {
    throw std::runtime_error("Please specify either an RPC password or use the --rpc-legacy-security flag");
  }

  if (!serviceConfig.rpcPassword.empty()) {
    rpcSecret = std::make_unique<Xi::Crypto::PasswordContainer>(serviceConfig.rpcPassword);
    serviceConfig.rpcPassword = "";
  }

  return true;
}

}  // namespace PaymentService
