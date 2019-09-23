// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

////////////////////////////////
#include <ZedWallet.h>
////////////////////////////////

#include <Common/SignalHandler.h>
#include <Common/Base58.h>
#include <Xi/FileSystem.h>
#include <Serialization/ConsoleOutputSerializer.hpp>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/Core.h>
#include <P2p/NetNode.h>
#include <CryptoNoteProtocol/CryptoNoteProtocolHandler.h>
#include <NodeRpcProxy/NodeErrors.h>
#include <NodeInProcess/NodeInProcess.hpp>
#include <Logging/FileLogger.h>
#include <Logging/ConsoleLogger.h>
#include <Logging/LoggerManager.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <ColouredMsg.h>
#include <Menu.h>
#include <Tools.h>
#include <Xi/Config/WalletConfig.h>

#include "CommonCLI.h"

int execZedWallet(System::Dispatcher* dispatcher, CryptoNote::INode& node, XiWallet::WalletOptions& config,
                  const CryptoNote::Currency& currency, Logging::ILogger& logger) {
  std::unique_ptr<System::Dispatcher> thisDispatcher{};
  if (dispatcher == nullptr) {
    thisDispatcher = std::make_unique<System::Dispatcher>();
    dispatcher = thisDispatcher.get();
  }

  std::promise<std::error_code> errorPromise;

  /* Once the function is complete, set the error value from the promise */
  auto callback = [&errorPromise](std::error_code e) { errorPromise.set_value(e); };

  /* Get the future of the result */
  auto initNode = errorPromise.get_future();

  node.init(callback);
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
    if (config.useRemote()) {
      std::cout << WarningMsg("Unable to connect to remote node.") << std::endl
                << WarningMsg(
                       "Confirm the remote node is functioning, "
                       "or try a different remote node.")
                << std::endl
                << std::endl;
    } else {
      std::cout << WarningMsg("Unable to launch node.") << std::endl << std::endl;
    }
  } else /* Ware connected and get fee information */ {
    /*
      This will check to see if the node responded to /feeinfo and actually
      returned something that it expects us to use for convenience charges
      for using that node to send transactions.
    */
    if (const auto nodeFee = node.feeAddress(); nodeFee) {
      std::stringstream feemsg;

      feemsg << std::endl
             << "You have connected to a node that charges "
             << "a fee to send transactions." << std::endl
             << std::endl
             << "The fee for sending transactions is: " << node.currency().amountFormatter()(nodeFee->amount)
             << " per transaction." << std::endl
             << std::endl
             << "If you don't want to pay the node fee, please "
             << "relaunch " << WalletConfig::walletName << " and specify a different node or run your own."
             << std::endl;

      std::cout << WarningMsg(feemsg.str()) << std::endl;
    }
  }

  /* Create the wallet instance */
  CryptoNote::WalletGreen wallet(*dispatcher, currency, node, logger);

  /* Run the interactive wallet interface */
  return run(wallet, node, config);
}

int run(CryptoNote::WalletGreen& wallet, CryptoNote::INode& node, XiWallet::WalletOptions& config) {
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
  return EXIT_SUCCESS;
}
