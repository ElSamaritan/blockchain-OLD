// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "PaymentGateService.h"

#include <future>

#include "Common/SignalHandler.h"
#include "Common/Util.h"
#include "Logging/LoggerRef.h"
#include "PaymentServiceJsonRpcServer.h"

#include "Common/ScopeExit.h"
#include "NodeRpcProxy/NodeRpcProxy.h"
#include <System/Context.h>
#include "Wallet/WalletGreen.h"

#ifdef ERROR
#undef ERROR
#endif

#ifdef _WIN32  // why is this still here?
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace PaymentService;

void changeDirectory(const std::string& path) {
  if (
#if defined(WIN32)
      _chdir(path.c_str())
#else
      chdir(path.c_str())
#endif
  ) {
    throw std::runtime_error("Couldn't change directory to \'" + path + "\': " + strerror(errno));
  }
}

void stopSignalHandler(PaymentGateService* pg) { pg->stop(); }

PaymentGateService::PaymentGateService()
    : dispatcher(nullptr),
      stopEvent(nullptr),
      config(),
      service(nullptr),
      logger(),
      fileLogger(Logging::Trace),
      consoleLogger(Logging::Info) {
  consoleLogger.setPattern("%D %T %L ");
  fileLogger.setPattern("%D %T %L ");
}

bool PaymentGateService::init(int argc, char** argv) {
  if (!config.init(argc, argv)) {
    return false;
  }

  logger.setMaxLevel(static_cast<Logging::Level>(config.serviceConfig.logLevel));
  logger.setPattern("%D %T %L ");
  logger.addLogger(consoleLogger);

  Logging::LoggerRef log(logger, "main");

  if (!config.serviceConfig.serverRoot.empty()) {
    changeDirectory(config.serviceConfig.serverRoot);
    log(Logging::Info) << "Current working directory now is " << config.serviceConfig.serverRoot;
  }

  fileStream.open(config.serviceConfig.logFile, std::ofstream::app);

  if (!fileStream) {
    throw std::runtime_error("Couldn't open log file");
  }

  fileLogger.attachToStream(fileStream);
  logger.addLogger(fileLogger);

  CryptoNote::CurrencyBuilder builder{log.getLogger()};
  m_currency = std::make_unique<CryptoNote::Currency>(builder.network(config.serviceConfig.network).currency());

  return true;
}

WalletConfiguration PaymentGateService::getWalletConfig() const {
  return WalletConfiguration{
      config.serviceConfig.containerFile, config.serviceConfig.containerPassword, config.serviceConfig.syncFromZero,
      config.serviceConfig.secretViewKey, config.serviceConfig.secretSpendKey,    config.serviceConfig.mnemonicSeed,
      config.serviceConfig.scanHeight,
  };
}

const CryptoNote::Currency& PaymentGateService::getCurrency() const { return *m_currency; }

void PaymentGateService::run() {
  System::Dispatcher localDispatcher;
  System::Event localStopEvent(localDispatcher);

  this->dispatcher = &localDispatcher;
  this->stopEvent = &localStopEvent;

  Tools::SignalHandler::install(std::bind(&stopSignalHandler, this));

  Logging::LoggerRef log(logger, "run");
  if (config.serviceConfig.ssl.isInsecure(::Xi::Http::SSLConfiguration::Usage::Both)) {
    log(Logging::Warning) << "\n" << CommonCLI::insecureClientWarning() << std::endl;
  }

  runRpcProxy(log);

  this->dispatcher = nullptr;
  this->stopEvent = nullptr;
}

void PaymentGateService::stop() {
  Logging::LoggerRef log(logger, "stop");

  log(Logging::Info) << "Stop signal caught";

  if (dispatcher != nullptr) {
    dispatcher->remoteSpawn([&]() {
      if (stopEvent != nullptr) {
        stopEvent->set();
      }
    });
  }
}

void PaymentGateService::runRpcProxy(Logging::LoggerRef& log) {
  log(Logging::Info) << "Starting Payment Gate with remote node";
  const CryptoNote::Currency& currency = getCurrency();

  std::unique_ptr<CryptoNote::INode> node(PaymentService::NodeFactory::createNode(
      config.serviceConfig.daemonAddress, config.serviceConfig.daemonPort, config.serviceConfig.rpcAccessToken,
      config.serviceConfig.ssl, getCurrency(), log.getLogger()));

  runWalletService(currency, *node);
}

void PaymentGateService::runWalletService(const CryptoNote::Currency& currency, CryptoNote::INode& node) {
  PaymentService::WalletConfiguration walletConfiguration{
      config.serviceConfig.containerFile, config.serviceConfig.containerPassword, config.serviceConfig.syncFromZero};

  std::unique_ptr<CryptoNote::WalletGreen> wallet(new CryptoNote::WalletGreen(*dispatcher, currency, node, logger));

  service =
      new PaymentService::WalletService(currency, *dispatcher, node, *wallet, *wallet, walletConfiguration, logger);
  std::unique_ptr<PaymentService::WalletService> serviceGuard(service);
  try {
    service->init();
  } catch (std::exception& e) {
    Logging::LoggerRef(logger, "run")(Logging::Error) << "Failed to init walletService reason: " << e.what();
    return;
  }

  if (config.serviceConfig.printAddresses) {
    // print addresses and exit
    std::vector<std::string> addresses;
    service->getAddresses(addresses);
    for (const auto& address : addresses) {
      std::cout << "Address: " << address << std::endl;
    }
  } else {
    auto rpcServer = std::make_shared<PaymentService::PaymentServiceJsonRpcServer>(*dispatcher, *stopEvent, *service,
                                                                                   logger, config);
    rpcServer->setHandler(rpcServer);
    rpcServer->setSSLConfiguration(config.serviceConfig.ssl);
    rpcServer->start(config.serviceConfig.bindAddress, config.serviceConfig.bindPort);

    Logging::LoggerRef(logger, "PaymentGateService")(Logging::Info)
        << "JSON-RPC server stopped, stopping wallet service...";

    try {
      service->saveWallet();
    } catch (std::exception& ex) {
      Logging::LoggerRef(logger, "saveWallet")(Logging::Warning) << "Couldn't save container: " << ex.what();
    }
  }
}
