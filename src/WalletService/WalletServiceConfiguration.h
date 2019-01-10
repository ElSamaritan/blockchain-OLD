// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <fstream>
#include <string>
#include <cinttypes>

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <nlohmann/json.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Http/SSLClientConfiguration.h>
#include <Xi/Http/SSLServerConfiguration.h>

#include "CommonCLI/CommonCLI.h"
#include <Xi/Config.h>
#include <Logging/ILogger.h>

using nlohmann::json;

namespace PaymentService {
struct WalletServiceConfiguration {
  std::string daemonAddress;
  std::string bindAddress;
  std::string rpcPassword;
  std::string containerFile;
  std::string containerPassword;
  std::string serverRoot;
  std::string corsHeader;
  std::string logFile;

  uint16_t daemonPort;
  uint16_t bindPort;
  int logLevel;

  bool legacySecurity;

  // Runtime online options
  bool help;
  bool version;
  bool dumpConfig;
  std::string configFile;
  std::string outputFile;

  std::string secretViewKey;
  std::string secretSpendKey;
  std::string mnemonicSeed;

  bool generateNewContainer;
  bool daemonize;
  bool registerService;
  bool unregisterService;
  bool printAddresses;
  bool syncFromZero;

  uint32_t scanHeight;

  ::Xi::Http::SSLClientConfiguration sslClient;
  ::Xi::Http::SSLServerConfiguration sslServer;
};

inline WalletServiceConfiguration initConfiguration() {
  WalletServiceConfiguration config;

  config.daemonAddress = "127.0.0.1";
  config.bindAddress = "127.0.0.1";
  config.logFile = "service.log";
  config.daemonPort = Xi::Config::Network::rpcPort();
  config.bindPort = Xi::Config::Network::pgPort();
  config.logLevel = Logging::INFO;
  config.legacySecurity = false;
  config.dumpConfig = false;
  config.generateNewContainer = false;
  config.daemonize = false;
  config.registerService = false;
  config.unregisterService = false;
  config.printAddresses = false;
  config.syncFromZero = false;

  return config;
};

inline void handleSettings(int argc, char* argv[], WalletServiceConfiguration& config) {
  cxxopts::Options options(argv[0], CommonCLI::header());
  CommonCLI::emplaceCLIOptions(options);

  // clang-format off
  options.add_options("Daemon")
    ("daemon-address", "The daemon host to use for node operations",cxxopts::value<std::string>()->default_value(config.daemonAddress), "<ip>")
    ("daemon-port", "The daemon RPC port to use for node operations", cxxopts::value<uint16_t>()->default_value(std::to_string(config.daemonPort)), "<port>");

  options.add_options("Service")
    ("c,config", "Specify the configuration <file> to use instead of CLI arguments", cxxopts::value<std::string>(), "<file>")
    ("dump-config", "Prints the current configuration to the screen", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
    ("log-file", "Specify log <file> location", cxxopts::value<std::string>()->default_value(config.logFile), "<file>")
    ("log-level", "Specify log level", cxxopts::value<int>()->default_value(std::to_string(config.logLevel)), "#")
    ("server-root", "The service will use this <path> as the working directory", cxxopts::value<std::string>(), "<path>")
    ("save-config", "Save the configuration to the specified <file>", cxxopts::value<std::string>(), "<file>");

  options.add_options("Wallet")
    ("address", "Print the wallet addresses and then exit", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
    ("w,container-file", "Wallet container <file>", cxxopts::value<std::string>(), "<file>")
    ("p,container-password", "Wallet container <password>", cxxopts::value<std::string>(), "<password>")
    ("g,generate-container", "Generate a new wallet container", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
    ("view-key", "Generate a wallet container with this secret view <key>", cxxopts::value<std::string>(), "<key>")
    ("spend-key", "Generate a wallet container with this secret spend <key>", cxxopts::value<std::string>(), "<key>")
    ("mnemonic-seed", "Generate a wallet container with this Mnemonic <seed>", cxxopts::value<std::string>(), "<seed>")
    ("scan-height", "Start scanning for transactions from this Blockchain height", cxxopts::value<uint32_t>()->default_value("0"), "#")
    ("sync-from-zero", "Force the wallet to sync from 0", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));

  options.add_options("Network")
    ("bind-address", "Interface IP address for the RPC service", cxxopts::value<std::string>()->default_value(config.bindAddress), "<ip>")
    ("bind-port", "TCP port for the RPC service", cxxopts::value<uint16_t>()->default_value(std::to_string(config.bindPort)), "<port>");

  options.add_options("RPC")
    ("enable-cors", "Adds header 'Access-Control-Allow-Origin' to the RPC responses. Uses the value specified as the domain. Use * for all.",
      cxxopts::value<std::string>(), "<domain>")
    ("rpc-legacy-security", "Enable legacy mode (no password for RPC). WARNING: INSECURE. USE ONLY AS A LAST RESORT.",
      cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
    ("rpc-password", "Specify the <password> to access the RPC server.", cxxopts::value<std::string>(), "<password>");

#ifdef WIN32
  options.add_options("Windows Only")
    ("daemonize", "Run the service as a daemon", cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
    ("register-service", "Registers this program as a Windows service",cxxopts::value<bool>()->default_value("false")->implicit_value("true"))
    ("unregister-service", "Unregisters this program from being a Windows service", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));
#endif

  config.sslClient.emplaceCLIConfiguration(options);
  config.sslServer.emplaceCLIConfiguration(options);
  // clang-format on

  try {
    auto cli = options.parse(argc, argv);

    if (cli.count("config") > 0) {
      config.configFile = cli["config"].as<std::string>();
    }

    if (cli.count("save-config") > 0) {
      config.outputFile = cli["save-config"].as<std::string>();
    }

    if (cli.count("dump-config") > 0) {
      config.dumpConfig = cli["dump-config"].as<bool>();
    }

    if (cli.count("daemon-address") > 0) {
      config.daemonAddress = cli["daemon-address"].as<std::string>();
    }

    if (cli.count("daemon-port") > 0) {
      config.daemonPort = cli["daemon-port"].as<uint16_t>();
    }

    if (cli.count("log-file") > 0) {
      config.logFile = cli["log-file"].as<std::string>();
    }

    if (cli.count("log-level") > 0) {
      config.logLevel = cli["log-level"].as<int>();
    }

    if (cli.count("container-file") > 0) {
      config.containerFile = cli["container-file"].as<std::string>();
    }

    if (cli.count("container-password") > 0) {
      config.containerPassword = cli["container-password"].as<std::string>();
    }

    if (cli.count("bind-address") > 0) {
      config.bindAddress = cli["bind-address"].as<std::string>();
    }

    if (cli.count("bind-port") > 0) {
      config.bindPort = cli["bind-port"].as<uint16_t>();
    }

    if (cli.count("enable-cors") > 0) {
      config.corsHeader = cli["enable-cors"].as<std::string>();
    }

    if (cli.count("rpc-legacy-security") > 0) {
      config.legacySecurity = cli["rpc-legacy-security"].as<bool>();
    }

    if (cli.count("rpc-password") > 0) {
      config.rpcPassword = cli["rpc-password"].as<std::string>();
    }

    if (cli.count("server-root") > 0) {
      config.serverRoot = cli["server-root"].as<std::string>();
    }

    if (cli.count("view-key") > 0) {
      config.secretViewKey = cli["view-key"].as<std::string>();
    }

    if (cli.count("spend-key") > 0) {
      config.secretSpendKey = cli["spend-key"].as<std::string>();
    }

    if (cli.count("mnemonic-seed") > 0) {
      config.mnemonicSeed = cli["mnemonic-seed"].as<std::string>();
    }

    if (cli.count("generate-container") > 0) {
      config.generateNewContainer = cli["generate-container"].as<bool>();
    }

    if (cli.count("daemonize") > 0) {
      config.daemonize = cli["daemonize"].as<bool>();
    }

    if (cli.count("register-service") > 0) {
      config.registerService = cli["register-service"].as<bool>();
    }

    if (cli.count("unregister-service") > 0) {
      config.unregisterService = cli["unregister-service"].as<bool>();
    }

    if (cli.count("address") > 0) {
      config.printAddresses = cli["address"].as<bool>();
    }

    if (cli.count("sync-from-zero") > 0) {
      config.syncFromZero = cli["sync-from-zero"].as<bool>();
    }

    if (cli.count("scan-height") > 0) {
      config.scanHeight = cli["scan-height"].as<uint32_t>();
    }

    if (CommonCLI::handleCLIOptions(options, cli)) exit(0);
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error: Unable to parse command line argument options: " << e.what() << std::endl
              << std::endl
              << options.help({}) << std::endl;
    exit(1);
  }
};

inline void handleSettings(const std::string configFile, WalletServiceConfiguration& config) {
  std::ifstream data(configFile.c_str());

  if (!data.good()) {
    throw std::runtime_error(
        "The --config-file you specified does not exist, please check the filename and try again.");
  }

  json j;
  data >> j;

  if (j.find("daemon-address") != j.end()) {
    config.daemonAddress = j["daemon-address"].get<std::string>();
  }

  if (j.find("daemon-port") != j.end()) {
    config.daemonPort = j["daemon-port"].get<uint16_t>();
  }

  if (j.find("log-file") != j.end()) {
    config.logFile = j["log-file"].get<std::string>();
  }

  if (j.find("log-level") != j.end()) {
    config.logLevel = j["log-level"].get<int>();
  }

  if (j.find("container-file") != j.end()) {
    config.containerFile = j["container-file"].get<std::string>();
  }

  if (j.find("container-password") != j.end()) {
    config.containerPassword = j["container-password"].get<std::string>();
  }

  if (j.find("bind-address") != j.end()) {
    config.bindAddress = j["bind-address"].get<std::string>();
  }

  if (j.find("bind-port") != j.end()) {
    config.bindPort = j["bind-port"].get<uint16_t>();
  }

  if (j.find("enable-cors") != j.end()) {
    config.corsHeader = j["enable-cors"].get<std::string>();
  }

  if (j.find("rpc-legacy-security") != j.end()) {
    config.legacySecurity = j["rpc-legacy-security"].get<bool>();
  }

  if (j.find("rpc-password") != j.end()) {
    config.rpcPassword = j["rpc-password"].get<std::string>();
  }

  if (j.find("server-root") != j.end()) {
    config.serverRoot = j["server-root"].get<std::string>();
  }

  if (j.find("ssl-client") != j.end()) {
    config.sslClient.load(j["ssl-client"]);
  }
  if (j.find("ssl-server") != j.end()) {
    config.sslServer.load(j["ssl-server"]);
  }
}

inline json asJSON(const WalletServiceConfiguration& config) {
  json sslClient = json{};
  config.sslClient.store(sslClient);
  json sslServer = json{};
  config.sslServer.store(sslServer);
  json j = json{{"daemon-address", config.daemonAddress},
                {"daemon-port", config.daemonPort},
                {"log-file", config.logFile},
                {"log-level", config.logLevel},
                {"container-file", config.containerFile},
                {"container-password", config.containerPassword},
                {"bind-address", config.bindAddress},
                {"bind-port", config.bindPort},
                {"enable-cors", config.corsHeader},
                {"rpc-legacy-security", config.legacySecurity},
                {"rpc-password", config.rpcPassword},
                {"server-root", config.serverRoot},
                {"ssl-client", sslClient},
                {"ssl-server", sslServer}};
  return j;
};

inline std::string asString(const WalletServiceConfiguration& config) {
  json j = asJSON(config);
  return j.dump(2);
};

inline void asFile(const WalletServiceConfiguration& config, const std::string& filename) {
  json j = asJSON(config);
  std::ofstream data(filename);
  data << std::setw(2) << j << std::endl;
};
}  // namespace PaymentService
