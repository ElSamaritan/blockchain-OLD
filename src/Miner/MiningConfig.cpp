// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "MiningConfig.h"

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <limits>
#include <sstream>

#include <Xi/Utils/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Version/Version.h>
#include <Xi/Config.h>

#include "Common/StringTools.h"
#include <CommonCLI.h>
#include <Common/Util.h>

#include "Logging/ILogger.h"

namespace CryptoNote {

namespace {

const size_t CONCURRENCY_LEVEL = std::thread::hardware_concurrency();

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
    } catch (...) {
      return false;
    }
  }

  host = parts.at(0);
  port = Xi::Config::Network::rpcPort();
  return true;
}

}  // namespace

MiningConfig::MiningConfig() {}

void MiningConfig::parse(int argc, char** argv) {
  cxxopts::Options options(argv[0], CommonCLI::header());

  // clang-format off
  CommonCLI::emplaceCLIOptions(options);

  std::string daemonAddress;
  options.add_options("Daemon")
    ("daemon-address", "The daemon [host:port] combination to use for node operations. This option overrides --daemon-host and --daemon-rpc-port",
      cxxopts::value<std::string>(daemonAddress), "<host:port>")
    ("daemon-host", "The daemon host to use for node operations", cxxopts::value<std::string>(daemonHost)->default_value("127.0.0.1"), "<host>")
    ("daemon-rpc-port", "The daemon RPC port to use for node operations", cxxopts::value<uint16_t>(daemonPort)->default_value(std::to_string(Xi::Config::Network::rpcPort())), "#")
    ("scan-time", "Blockchain polling interval (seconds). How often miner will check the Blockchain for updates", cxxopts::value<size_t>(scanPeriod)->default_value("30"), "#");

  options.add_options("Mining")
    ("address", "The valid CryptoNote miner's address", cxxopts::value<std::string>(miningAddress), "<address>")
    ("block-timestamp-interval", "Timestamp incremental step for each subsequent block. May be set only if --first-block-timestamp has been set.",
      cxxopts::value<int64_t>(blockTimestampInterval) ->default_value("0"), "#")
    ("first-block-timestamp", "Set timestamp to the first mined block. 0 means leave timestamp unchanged", cxxopts::value<uint64_t>(firstBlockTimestamp)->default_value("0"), "#")
    ("limit", "Mine this exact quantity of blocks and then stop. 0 means no limit", cxxopts::value<size_t>(blocksLimit)->default_value("0"), "#")
    ("log-level", "Specify log level. Must be 0 - 5", cxxopts::value<uint8_t>(logLevel)->default_value("3"), "#")
    ("threads", "The mining threads count. Must not exceed hardware capabilities.", cxxopts::value<size_t>(threadCount)->default_value(std::to_string(CONCURRENCY_LEVEL)), "#");

  ssl.emplaceCLIConfiguration(options);
  // clang-format on

  try {
    const auto result = options.parse(argc, argv);
    if (CommonCLI::handleCLIOptions(options, result)) exit(0);
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error: Unable to parse command line argument options: " << e.what() << std::endl << std::endl;
    std::cout << options.help({}) << std::endl;
    exit(1);
  }

  if (miningAddress.empty()) {
    throw std::runtime_error("Specify --address option");
  }

  if (!daemonAddress.empty()) {
    if (!parseDaemonAddressFromString(daemonHost, daemonPort, daemonAddress)) {
      throw std::runtime_error("Could not parse --daemon-address option");
    }
  }

  if (threadCount == 0 || threadCount > CONCURRENCY_LEVEL) {
    throw std::runtime_error("--threads option must be 1.." + std::to_string(CONCURRENCY_LEVEL));
  }

  if (scanPeriod == 0) {
    throw std::runtime_error("--scan-time must not be zero");
  }

  if (logLevel > static_cast<uint8_t>(Logging::TRACE)) {
    throw std::runtime_error("--log-level value is too big");
  }

  if (firstBlockTimestamp == 0 && blockTimestampInterval != 0) {
    throw std::runtime_error("If you specify --block-timestamp-interval you must also specify --first-block-timestamp");
  }
}

}  // namespace CryptoNote
