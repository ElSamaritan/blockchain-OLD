// Copyright (c) 2018, The TurtleCoin Developers
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
#include <Xi/Version.h>
#include <Xi/Config.h>
#include <Xi/Config/WalletConfig.h>
#include <zedwallet/Tools.h>

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
  port = Xi::Config::Network::rpcPort();
  return true;
}

Config parseArguments(int argc, char** argv) {
  Config config;

  std::stringstream defaultRemoteDaemon;
  defaultRemoteDaemon << config.host << ":" << config.port;

  cxxopts::Options options(argv[0], CommonCLI::header());
  CommonCLI::emplaceCLIOptions(options);

  std::string remoteDaemon;

  // clang-format off
  options.add_options("Development")
    ("debug", "Enable " + WalletConfig::walletdName + " debugging to "+ WalletConfig::walletName + ".log",
      cxxopts::value<bool>(config.debug)->default_value("false")->implicit_value("true"))
    ("verbose", "Enables verbose logging for debugging purposes.",
      cxxopts::value<bool>(config.verbose)->default_value("false")->implicit_value("true"))
    ("network", "The network type you want to connect to, mostly you want to use 'MainNet' here.",
     cxxopts::value<std::string>()->default_value(Xi::to_string(Xi::Config::Network::defaultNetworkType())),
     "[MainNet|StageNet|TestNet|LocalTestNet]");

  options.add_options("Daemon")
    ("r,remote-daemon", "The daemon <host:port> combination to use for node operations.",
      cxxopts::value<std::string>(remoteDaemon)->default_value(defaultRemoteDaemon.str()), "<host:port>");

  options.add_options("Wallet")
    ("w,wallet-file", "Open the wallet <file>", cxxopts::value<std::string>(config.walletFile), "<file>")
    ("p,password", "Use the password <pass> to open the wallet", cxxopts::value<std::string>(config.walletPass), "<pass>");


  config.ssl.emplaceCLIConfiguration(options);
  // clang-format on

  try {
    const auto result = options.parse(argc, argv);
    if (CommonCLI::handleCLIOptions(options, result)) exit(0);

    if (result.count("network")) {
      config.network = Xi::lexical_cast<Xi::Config::Network::Type>(result["network"].as<std::string>());
    }
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error: Unable to parse command line argument options: " << e.what() << std::endl << std::endl;
    std::cout << options.help({}) << std::endl;
    exit(1);
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
