﻿// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

////////////////////////////////
#include <ZedWallet.h>
////////////////////////////////

#include <Common/SignalHandler.h>
#include <CryptoNoteCore/Currency.h>
#include <NodeRpcProxy/NodeErrors.h>
#include <Logging/FileLogger.h>
#include <Logging/ConsoleLogger.h>
#include <Logging/LoggerManager.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <ColouredMsg.h>
#include <Menu.h>
#include <ParseArguments.h>
#include <Tools.h>
#include <Xi/Config/WalletConfig.h>

#include "CommonCLI.h"

int main(int argc, char **argv) {
/* On ctrl+c the program seems to throw "xi-wallet.exe has stopped
   working" when calling exit(0)... I'm not sure why, this is a bit of
   a hack, it disables that - possibly some deconstructers calling
   terminate() */
#ifdef _WIN32
  SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif

  Config config = parseArguments(argc, argv);

  auto crashDumper = CommonCLI::make_crash_dumper("xi-wallet");
  (void)crashDumper;

  std::cout << InformationMsg(CommonCLI::header()) << std::endl;

  if (config.ssl.isInsecure(::Xi::Http::SSLConfiguration::Usage::Client)) {
    std::cout << WarningMsg(CommonCLI::insecureClientWarning()) << std::endl;
  }

  Logging::LoggerManager logManager;

  /* We'd like these lines to be in the below if(), but because some genius
     thought it was a good idea to pass everything by reference and then
     use them after the functions lifetime they go out of scope and break
     stuff */
  logManager.setMaxLevel(Logging::DEBUGGING);

  Logging::FileLogger fileLogger;
  Logging::ConsoleLogger coutLogger;

  if (config.debug) {
    fileLogger.init(WalletConfig::walletName + ".log");
    logManager.addLogger(fileLogger);
  }
  if (config.verbose) {
    logManager.addLogger(coutLogger);
  }

  Logging::LoggerRef logger(logManager, WalletConfig::walletName);

  /* Currency contains our coin parameters, such as decimal places, supply */
  const CryptoNote::Currency currency = CryptoNote::CurrencyBuilder(logManager).network(config.network).currency();

  System::Dispatcher localDispatcher;
  System::Dispatcher *dispatcher = &localDispatcher;

  /* Our connection to xi-daemon */
  std::unique_ptr<CryptoNote::INode> node(
      new CryptoNote::NodeRpcProxy(config.host, config.port, config.ssl, logger.getLogger()));

  std::promise<std::error_code> errorPromise;

  /* Once the function is complete, set the error value from the promise */
  auto callback = [&errorPromise](std::error_code e) { errorPromise.set_value(e); };

  /* Get the future of the result */
  auto initNode = errorPromise.get_future();

  node->init(callback);
  std::error_code initEc{};
  const std::chrono::high_resolution_clock::time_point endWaitForInitialization =
      std::chrono::high_resolution_clock::now() + std::chrono::seconds{20};
  while (initNode.wait_for(std::chrono::seconds{1}) != std::future_status::ready) {
    if (std::chrono::high_resolution_clock::now() > endWaitForInitialization) {
      initEc = make_error_code(CryptoNote::error::CONNECT_ERROR);
      break;
    }
    std::cout << InformationMsg("Waiting for remote connection...") << std::endl;
  }

  /* Connection took to long to remote node, let program continue regardless
     as they could perform functions like export_keys without being
     connected */
  if (initEc) {
    if (config.host != "127.0.0.1") {
      std::cout << WarningMsg("Unable to connect to remote node.") << std::endl
                << WarningMsg(
                       "Confirm the remote node is functioning, "
                       "or try a different remote node.")
                << std::endl
                << std::endl;
    } else {
      std::cout << WarningMsg("Unable to connect to node.") << std::endl << std::endl;
    }
  } else /* Ware connected and get fee information */ {
    /*
      This will check to see if the node responded to /feeinfo and actually
      returned something that it expects us to use for convenience charges
      for using that node to send transactions.
    */
    if (node->feeAmount() != 0 && !node->feeAddress().empty()) {
      std::stringstream feemsg;

      feemsg << std::endl
             << "You have connected to a node that charges "
             << "a fee to send transactions." << std::endl
             << std::endl
             << "The fee for sending transactions is: " << formatAmount(node->feeAmount()) << " per transaction."
             << std::endl
             << std::endl
             << "If you don't want to pay the node fee, please "
             << "relaunch " << WalletConfig::walletName << " and specify a different node or run your own."
             << std::endl;

      std::cout << WarningMsg(feemsg.str()) << std::endl;
    }
  }

  /* Create the wallet instance */
  CryptoNote::WalletGreen wallet(*dispatcher, currency, *node, logger.getLogger());

  /* Run the interactive wallet interface */
  run(wallet, *node, config);
}

void run(CryptoNote::WalletGreen &wallet, CryptoNote::INode &node, Config &config) {
  std::shared_ptr<WalletInfo> walletInfo;

  bool quit;

  std::tie(quit, walletInfo) = selectionScreen(config, wallet, node);

  bool alreadyShuttingDown = false;

  if (!quit) {
    /* Call shutdown on ctrl+c */
    Tools::SignalHandler::install([&] {
      /* If we're already shutting down let control flow continue
         as normal */
      if (shutdown(walletInfo, node, alreadyShuttingDown)) {
        exit(0);
      }
    });

    mainLoop(walletInfo, node);
  }

  shutdown(walletInfo, node, alreadyShuttingDown);
}