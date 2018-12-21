﻿// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

/////////////////////////////////////
#include <zedwallet/ParseArguments.h>
/////////////////////////////////////

#include <vector>
#include <string>
#include <limits>

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <CommonCLI/CommonCLI.h>
#include <config/CryptoNoteConfig.h>
#include <config/WalletConfig.h>
#include <zedwallet/Tools.h>
#include "version.h"

template <class Container>
void split(const std::string& str, Container& cont, char delim = ' ') {
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delim)) {
    cont.push_back(token);
  }
}

bool parseDaemonAddressFromString(std::string& host, uint16_t& port, const std::string& address) {
  std::vector<std::string> parts;
  split(address, parts, ':');

  if (parts.empty()) {
    return false;
  } else if (parts.size() >= 2) {
    try {
      host = parts.at(0);
      auto uncheckedPort = std::stoul(parts.at(1));
      if (uncheckedPort > std::numeric_limits<uint16_t>::max()) return false;
      port = static_cast<uint16_t>(uncheckedPort);
      return true;
    } catch (std::exception&) {
      return false;
    }
  }

  host = parts.at(0);
  port = CryptoNote::Config::Network::rpcPort();
  return true;
}

Config parseArguments(int argc, char** argv) {
  Config config;

  std::stringstream defaultRemoteDaemon;
  defaultRemoteDaemon << config.host << ":" << config.port;

  cxxopts::Options options(argv[0], CommonCLI::header());

  bool help, version;
  std::string remoteDaemon;

  // clang-format off
  options.add_options("Core")
    ("h,help", "Display this help message", cxxopts::value<bool>(help)->implicit_value("true"))
    ("v,version", "Output software version information", cxxopts::value<bool>(version)->default_value("false")->implicit_value("true"))
    ("debug", "Enable " + WalletConfig::walletdName + " debugging to "+ WalletConfig::walletName + ".log",
      cxxopts::value<bool>(config.debug)->default_value("false")->implicit_value("true"));

  options.add_options("Daemon")
    ("r,remote-daemon", "The daemon <host:port> combination to use for node operations.",
      cxxopts::value<std::string>(remoteDaemon)->default_value(defaultRemoteDaemon.str()), "<host:port>");

  options.add_options("Wallet")
    ("w,wallet-file", "Open the wallet <file>", cxxopts::value<std::string>(config.walletFile), "<file>")
    ("p,password", "Use the password <pass> to open the wallet", cxxopts::value<std::string>(config.walletPass), "<pass>");

  config.ssl.emplaceCLIConfiguration(options);
  // clang-format on

  try {
    options.parse(argc, argv);
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error: Unable to parse command line argument options: " << e.what() << std::endl << std::endl;
    std::cout << options.help({}) << std::endl;
    exit(1);
  }

  if (help)  // Do we want to display the help message?
  {
    std::cout << options.help({}) << std::endl;
    exit(0);
  } else if (version)  // Do we want to display the software version?
  {
    std::cout << CommonCLI::header() << std::endl;
    exit(0);
  }

  if (!config.walletFile.empty()) {
    config.walletGiven = true;
  }

  if (!config.walletPass.empty()) {
    config.passGiven = true;
  }

  if (!remoteDaemon.empty()) {
    if (!parseDaemonAddressFromString(config.host, config.port, remoteDaemon)) {
      std::cout << "There was an error parsing the --remote-daemon you specified" << std::endl;
      exit(1);
    }
  }

  return config;
}
