// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Karai Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "DaemonConfiguration.h"
#include "DaemonCommandsHandler.h"

#include <Logging/LoggerMessage.h>

#include "Common/ScopeExit.h"
#include "Common/SignalHandler.h"
#include "Common/StdOutputStream.h"
#include "Common/StdInputStream.h"
#include "Common/PathTools.h"
#include "Common/Util.h"
#include "CommonCLI/CommonCLI.h"

#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Core.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "CryptoNoteCore/Currency.h"
#include "CryptoNoteCore/DatabaseBlockchainCache.h"
#include "CryptoNoteCore/DatabaseBlockchainCacheFactory.h"
#include "CryptoNoteCore/MainChainStorage.h"
#include "CryptoNoteCore/RocksDBWrapper.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
#include "P2p/NetNode.h"
#include "P2p/NetNodeConfig.h"
#include "Rpc/RpcServer.h"
#include "Serialization/BinaryInputStreamSerializer.h"
#include "Serialization/BinaryOutputStreamSerializer.h"

#include <Xi/Algorithm/String.h>
#include <Xi/FileSystem.h>
#include <Xi/Config/Checkpoints.h>

#include <Logging/LoggerManager.h>

#if defined(WIN32)
#include <crtdbg.h>
#include <io.h>
#else
#include <unistd.h>
#endif

using Common::JsonValue;
using namespace CryptoNote;
using namespace Logging;
using namespace boost::filesystem;

void print_genesis_tx_hex(const std::vector<std::string> rewardAddresses, LoggerManager& logManager) {
  std::vector<CryptoNote::AccountPublicAddress> rewardTargets;
  CryptoNote::CurrencyBuilder currencyBuilder(logManager);

  for (const auto& rewardAddress : rewardAddresses) {
    CryptoNote::AccountPublicAddress address;
    uint64_t prefix = 0;
    if (!parseAccountAddressString(prefix, address, rewardAddress) ||
        prefix != Xi::Config::Coin::addressBas58Prefix()) {
      std::cout << "Failed to parse genesis reward address: " << rewardAddress << std::endl;
      return;
    }
    rewardTargets.emplace_back(std::move(address));
  }
  CryptoNote::Transaction transaction;
  if (rewardTargets.empty()) {
    if (Xi::Config::Coin::amountOfPremine() > 0) {
      std::cout << "Error: Genesis Block Reward Addresses are not defined" << std::endl;
      return;
    }
    transaction = CryptoNote::CurrencyBuilder(logManager).generateGenesisTransaction();
  } else {
    transaction = CryptoNote::CurrencyBuilder(logManager).generateGenesisTransaction(rewardTargets);
  }
  auto transactionBlob = CryptoNote::toBinaryArray(transaction);
  std::string transactionHex = Common::toHex(transactionBlob);
  std::cout << CommonCLI::header() << std::endl
            << std::endl
            << "Replace the current genesisTransactionHash line in src/Xi/Config/Coin.h with this one:" << std::endl
            << "\"" << transactionHex << "\";" << std::endl;

  return;
}

JsonValue buildLoggerConfiguration(Level level, const std::string& logfile) {
  JsonValue loggerConfiguration(JsonValue::OBJECT);
  loggerConfiguration.insert("globalLevel", JsonValue{static_cast<int64_t>(level)});

  JsonValue& cfgLoggers = loggerConfiguration.insert("loggers", JsonValue{JsonValue::ARRAY});

  JsonValue& fileLogger = cfgLoggers.pushBack(JsonValue{JsonValue::OBJECT});
  fileLogger.insert("type", JsonValue{"file"});
  fileLogger.insert("filename", JsonValue{logfile});
  fileLogger.insert("level", JsonValue{static_cast<int64_t>(TRACE)});

  JsonValue& consoleLogger = cfgLoggers.pushBack(JsonValue{JsonValue::OBJECT});
  consoleLogger.insert("type", JsonValue{"console"});
  consoleLogger.insert("level", JsonValue{static_cast<int64_t>(TRACE)});
  consoleLogger.insert("pattern", JsonValue{"%D %T %L "});

  return loggerConfiguration;
}

/* Wait for input so users can read errors before the window closes if they
   launch from a GUI rather than a terminal */
void pause_for_input(int argc) {
  /* if they passed arguments they're probably in a terminal so the errors will
     stay visible */
  if (argc == 1) {
#if defined(WIN32)
    if (_isatty(_fileno(stdout)) && _isatty(_fileno(stdin))) {
#else
    if (isatty(fileno(stdout)) && isatty(fileno(stdin))) {
#endif
      std::cout << "Press any key to close the program: ";
      getchar();
    }
  }
}

int main(int argc, char* argv[]) {
  DaemonConfiguration config = initConfiguration(argv[0]);

#ifdef WIN32
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  LoggerManager logManager;
  LoggerRef logger(logManager, "daemon");

  // Initial loading of CLI parameters
  handleSettings(argc, argv, config);
  if (config.printGenesisTx)  // Do we weant to generate the Genesis Tx?
  {
    print_genesis_tx_hex(config.genesisAwardAddresses, logManager);
    exit(0);
  }

  // If the user passed in the --config-file option, we need to handle that first
  if (!config.configFile.empty()) {
    try {
      handleSettings(config.configFile, config);
    } catch (std::exception& e) {
      std::cout << std::endl
                << "There was an error parsing the specified configuration file. Please check the file and try again"
                << std::endl
                << e.what() << std::endl;
      exit(1);
    }
  }
  // Load in the CLI specified parameters again to overwrite anything from the config file
  handleSettings(argc, argv, config);

  if (config.dumpConfig) {
    std::cout << CommonCLI::header() << asString(config) << std::endl;
    exit(0);
  } else if (!config.outputFile.empty()) {
    try {
      asFile(config, config.outputFile);
      std::cout << CommonCLI::header() << "Configuration saved to: " << config.outputFile << std::endl;
      exit(0);
    } catch (std::exception& e) {
      std::cout << CommonCLI::header() << "Could not save configuration to: " << config.outputFile << std::endl
                << e.what() << std::endl;
      exit(1);
    }
  }

  XI_UNUSED_REVAL(CommonCLI::make_crash_dumper("xi-daemon"));

  try {
    auto modulePath = Common::NativePathToGeneric(argv[0]);
    auto cfgLogFile = Common::NativePathToGeneric(config.logFile);

    if (cfgLogFile.empty()) {
      cfgLogFile = Common::ReplaceExtenstion(modulePath, ".log");
    } else {
      if (!Common::HasParentPath(cfgLogFile)) {
        cfgLogFile = Common::CombinePath(Common::GetPathDirectory(modulePath), cfgLogFile);
      }
    }

    Level cfgLogLevel = static_cast<Level>(config.logLevel);

    // configure logging
    logManager.configure(buildLoggerConfiguration(cfgLogLevel, cfgLogFile));
    logger(INFO) << CommonCLI::header(true) << std::endl;
    if (config.ssl.isInsecure(::Xi::Http::SSLConfiguration::Usage::Server)) {
      logger(WARNING) << "\n" << CommonCLI::insecureServerWarning() << std::endl;
    }
    logger(INFO) << "Program Working Directory: " << argv[0];

    // create objects and link them
    CryptoNote::CurrencyBuilder currencyBuilder(logManager);
    // clang-format off
    currencyBuilder
      .network(config.network);
    // clang-format on
    try {
      currencyBuilder.currency();
    } catch (std::exception&) {
      std::cout << "genesisTransactionHash() constant has an incorrect value. Please launch: "
                << "xi-daemon --print-genesis-tx" << std::endl;
      return 1;
    }
    CryptoNote::Currency currency = currencyBuilder.currency();

    bool use_checkpoints = !config.checkPoints.empty();

    CryptoNote::Checkpoints checkpoints(logManager);
    if (use_checkpoints) {
      logger(INFO) << "Loading Checkpoints for faster initial sync...";
      if (config.checkPoints == "default") {
        for (const auto& cp : Xi::Config::CHECKPOINTS) {
          checkpoints.addCheckpoint(cp.index, cp.blockId);
        }
        logger(INFO) << "Loaded " << Xi::Config::CHECKPOINTS.size() << " default checkpoints";
      } else {
        bool results = checkpoints.loadCheckpointsFromFile(config.checkPoints);
        if (!results) {
          throw std::runtime_error("Failed to load checkpoints");
        }
      }
    }

    NetNodeConfig netNodeConfig;
    netNodeConfig.setNetwork(config.network);
    netNodeConfig.setBlockDuration(std::chrono::minutes{std::max<int64_t>(0, config.p2pBanDurationMinutes)});
    netNodeConfig.setAutoBlock(config.p2pAutoBan);
    netNodeConfig.init(config.p2pInterface, config.p2pPort, config.p2pExternalPort, config.localIp, config.hideMyPort,
                       config.dataDirectory, config.peers, config.exclusiveNodes, config.priorityNodes,
                       config.seedNodes);

    DataBaseConfig dbConfig;
    dbConfig.init(config.dataDirectory, config.dbThreads, config.dbMaxOpenFiles, config.dbWriteBufferSize,
                  config.dbReadCacheSize);
    dbConfig.setNetwork(config.network);
    dbConfig.setCompression(config.dbCompression);
    Xi::FileSystem::ensureDirectoryExists(dbConfig.getDataDir()).throwOnError();

    RocksDBWrapper database(logManager);
    database.init(dbConfig);
    Tools::ScopeExit dbShutdownOnExit([&database]() { database.shutdown(); });

    if (!DatabaseBlockchainCache::checkDBSchemeVersion(database, logManager)) {
      dbShutdownOnExit.cancel();
      database.shutdown();

      database.destoy(dbConfig);

      database.init(dbConfig);
      dbShutdownOnExit.resume();
    }

    System::Dispatcher dispatcher;
    logger(INFO) << "Initializing core...";
    CryptoNote::Core ccore(
        currency, logManager, checkpoints, dispatcher, config.lightNode,
        std::unique_ptr<IBlockchainCacheFactory>(new DatabaseBlockchainCacheFactory(database, logger.getLogger())),
        createSwappedMainChainStorage(config.dataDirectory, currency));

    if (!ccore.load()) {
      logger(FATAL) << "Core initialization failed";
      return EXIT_FAILURE;
    }
    logger(INFO) << "Core initialized OK";

    // ------------------------------------------ Transaction Pool
    logger(INFO) << "Initializing transaction pool...";
    const auto transactionPoolFile = path(config.dataDirectory) / currency.txPoolFileName();
    if (!exists(transactionPoolFile)) {
      logger(INFO) << "no transaction pool file present, skipping import.";
    } else {
      std::ifstream poolFileStream{transactionPoolFile.string(), std::ios::in | std::ios::binary};
      Common::StdInputStream poolInputStream{poolFileStream};
      BinaryInputStreamSerializer poolSerializer(poolInputStream);
      if (!ccore.transactionPool().serialize(poolSerializer)) {
        logger(ERROR) << "Transaction pool load failed, cleaning state...";
        ccore.transactionPool().forceFlush();
      } else {
        logger(INFO) << "Imported " << ccore.transactionPool().size() << " pending pool transactions.";
      }
    }
    logger(INFO) << "Transaction Pool initialized OK";
    // ------------------------------------------ Transaction Pool

    CryptoNote::CryptoNoteProtocolHandler cprotocol(currency, dispatcher, ccore, nullptr, logManager);
    CryptoNote::NodeServer p2psrv(dispatcher, config.network, cprotocol, logManager);
    auto rpcServer = std::make_shared<CryptoNote::RpcServer>(dispatcher, logManager, ccore, p2psrv, cprotocol);
    if (!config.enableCors.empty()) rpcServer->enableCors(config.enableCors);

    cprotocol.set_p2p_endpoint(&p2psrv);
    DaemonCommandsHandler dch(ccore, p2psrv, logManager, rpcServer.get());
    logger(INFO) << "Initializing p2p server...";
    if (!p2psrv.init(netNodeConfig)) {
      logger(ERROR) << "Failed to initialize p2p server.";
      return 1;
    }

    logger(INFO) << "P2p server initialized OK";

    if (!config.noConsole) {
      dch.start_handling();
    }

    // Fire up the RPC Server
    logger(INFO) << "Starting core rpc server on address " << config.rpcInterface << ":" << config.rpcPort;
    rpcServer->setHandler(rpcServer);
    rpcServer->setSSLConfiguration(config.ssl);
    if (!config.feeAddress.empty()) {
      rpcServer->setFeeAddress(config.feeAddress);
      rpcServer->setFeeAmount(config.feeAmount);
    }
    rpcServer->enableCors(config.enableCors);
    rpcServer->setBlockexplorer(config.enableBlockExplorer | config.onlyBlockExplorer);
    rpcServer->setBlockexplorerOnly(config.onlyBlockExplorer);

    if (!config.disableRpc) {
      rpcServer->start(config.rpcInterface, config.rpcPort);
      logger(INFO) << "Core rpc server started ok";
    } else {
      logger(INFO) << "Core rpc server disabled";
    }

    Tools::SignalHandler::install([&dch, &p2psrv] {
      dch.stop_handling();
      p2psrv.sendStopSignal();
    });

    logger(INFO) << "Starting p2p net loop...";
    if (!p2psrv.run()) {
      logger(FATAL) << "p2p server initialization failed";
    }
    logger(INFO) << "p2p net loop stopped";

    dch.stop_handling();

    // stop components
    if (!config.disableRpc) {
      logger(INFO) << "Stopping core rpc server...";
      rpcServer->stop();
    }
    rpcServer->setHandler(nullptr);

    // deinitialize components
    logger(INFO) << "Deinitializing p2p...";
    if (!p2psrv.deinit()) {
      logger(FATAL) << "p2p shutdown failed, p2p files may be corrupted.";
    }

    // ------------------------------------------ Transaction Pool
    logger(INFO) << "Exporting transaction pool...";
    {
      std::ofstream poolFileStream{transactionPoolFile.string(), std::ios::out | std::ios::binary | std::ios::trunc};
      Common::StdOutputStream poolInputStream{poolFileStream};
      BinaryOutputStreamSerializer poolSerializer(poolInputStream);
      if (!ccore.transactionPool().serialize(poolSerializer)) {
        logger(ERROR)
            << "Transaction pool save failed, your transaction pool may be corrupted and discarded on next start";
      }
      logger(INFO) << "Exported " << ccore.transactionPool().size() << " pending pool transactions.";
    }
    logger(INFO) << "Transaction Pool epxported OK";
    // ------------------------------------------ Transaction Pool

    cprotocol.set_p2p_endpoint(nullptr);
    if (!ccore.save()) {
      logger(FATAL) << "Blockchain save failed, your chain may be corrupted next time you start the daemon.";
    }

  } catch (const std::exception& e) {
    logger(ERROR) << "Exception: " << e.what();
    throw e;
  }

  logger(INFO) << "Node stopped.";
  return 0;
}
