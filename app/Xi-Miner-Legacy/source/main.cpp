﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "Common/SignalHandler.h"
#include "CommonCLI.h"

#include <Common/Util.h>

#include "Logging/LoggerGroup.h"
#include "Logging/ConsoleLogger.h"
#include "Logging/LoggerRef.h"
#include "Logging/FileLogger.h"

#include "MinerManager.h"

#include <System/Dispatcher.h>

int main(int argc, char** argv) {
  try {
    CryptoNote::MiningConfig config;
    config.parse(argc, argv);

    auto crashDumper = CommonCLI::make_crash_dumper("xi-miner");
    (void)crashDumper;

    if (config.ssl.isInsecure(::Xi::Http::SSLConfiguration::Usage::Client)) {
      std::cout << CommonCLI::insecureClientWarning() << std::endl;
    }

    Logging::LoggerGroup loggerGroup;
    Logging::ConsoleLogger consoleLogger(static_cast<Logging::Level>(config.logLevel));
    loggerGroup.addLogger(consoleLogger);
    std::unique_ptr<Logging::FileLogger> fileLogger;
    if (!config.logFile.empty()) {
      fileLogger = std::make_unique<Logging::FileLogger>(static_cast<Logging::Level>(config.logFileLevel));
      fileLogger->init(config.logFile);
      loggerGroup.addLogger(*fileLogger);
    }

    System::Dispatcher dispatcher;
    Miner::MinerManager app(dispatcher, config, loggerGroup);

    app.start();
  } catch (std::exception& e) {
    std::cerr << "Fatal: " << e.what() << std::endl;
    throw e;
  }

  return 0;
}