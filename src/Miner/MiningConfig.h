// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <cstdint>
#include <string>

#include <Xi/Http/SSLConfiguration.h>

namespace CryptoNote {

struct MiningConfig {
  MiningConfig();

  void parse(int argc, char** argv);

  std::string miningAddress;
  std::string daemonHost;
  uint16_t daemonPort;
  size_t threadCount;
  size_t scanPeriod;
  size_t blocksLimit;
  uint64_t firstBlockTimestamp;
  int64_t blockTimestampInterval;
  ::Xi::Http::SSLConfiguration ssl;

  // Logging
  uint8_t logLevel;
  std::string logFile;
  uint8_t logFileLevel;
};

}  // namespace CryptoNote
