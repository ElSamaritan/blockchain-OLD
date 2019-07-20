// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

////////////////////////////////
#include <ZedWallet.h>
////////////////////////////////

#include <Common/SignalHandler.h>
#include <Common/Base58.h>
#include <Mnemonics/Mnemonics.h>
#include <Serialization/ConsoleOutputSerializer.hpp>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
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

namespace {
int generate(Logging::ILogger &logger, const Config &config) {
  CryptoNote::CurrencyBuilder builder{logger};
  const auto &currency = builder.networkDir(config.networkDir).network(config.network).immediateState();
  CryptoNote::AccountKeys account;
  if (config.generate_seed.empty()) {
    Crypto::generate_keys(account.address.spendPublicKey, account.spendSecretKey);
  } else {
    Crypto::generate_deterministic_keys(account.address.spendPublicKey, account.spendSecretKey,
                                        Xi::asByteSpan(config.generate_seed));
  }
  CryptoNote::AccountBase::generateViewFromSpend(account.spendSecretKey, account.viewSecretKey);
  if (!Crypto::secret_key_to_public_key(account.viewSecretKey, account.address.viewPublicKey)) {
    return EXIT_FAILURE;
  }

  auto binaryAddress = CryptoNote::toBinaryArray(account.address);
  auto address = Tools::Base58::encode_addr(currency.coin().prefix().base58(), Common::asString(binaryAddress));
  auto mnemonicSeed = Mnemonics::PrivateKeyToMnemonic(account.spendSecretKey);

  CryptoNote::ConsoleOutputSerializer ser{std::cout};
  XI_RETURN_EC_IF_NOT(ser.beginObject("wallet"), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser(address, "address"), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser.beginObject("public_keys"), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser(account.address.viewPublicKey, "view_key"), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser(account.address.spendPublicKey, "spend_key"), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser.endObject(), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser.beginObject("secret_keys"), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser(account.viewSecretKey, "view_key"), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser(account.spendSecretKey, "spend_key"), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser.endObject(), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser(mnemonicSeed, "mnemonics"), EXIT_FAILURE);
  XI_RETURN_EC_IF_NOT(ser.endObject(), EXIT_FAILURE);
  std::cout << std::endl;
  return EXIT_SUCCESS;
};
}  // namespace

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

  Logging::LoggerManager logManager;

  /* We'd like these lines to be in the below if(), but because some genius
     thought it was a good idea to pass everything by reference and then
     use them after the functions lifetime they go out of scope and break
     stuff */
  logManager.setMaxLevel(Logging::Trace);

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

  if (config.generate) {
    coutLogger.setMaxLevel(Logging::Level::Warning);
    return generate(coutLogger, config);
  }

  /* Currency contains our coin parameters, such as decimal places, supply */
  const CryptoNote::Currency currency =
      CryptoNote::CurrencyBuilder(logManager).networkDir(config.networkDir).network(config.network).currency();

  std::cout << InformationMsg(CommonCLI::header(currency)) << std::endl;

  if (config.ssl.isInsecure(::Xi::Http::SSLConfiguration::Usage::Client)) {
    std::cout << WarningMsg(CommonCLI::insecureClientWarning()) << std::endl;
  }

  System::Dispatcher localDispatcher;
  System::Dispatcher *dispatcher = &localDispatcher;

  /* Our connection to xi-daemon */
  std::unique_ptr<CryptoNote::NodeRpcProxy> node(
      new CryptoNote::NodeRpcProxy(config.host, config.port, config.ssl, currency, logger.getLogger()));
  if (!config.remoteRpcAccessToken.empty()) {
    node->httpClient().useAuthorization(Xi::Http::BearerCredentials{config.remoteRpcAccessToken});
  }

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
    if (const auto nodeFee = node->feeAddress(); nodeFee) {
      std::stringstream feemsg;

      feemsg << std::endl
             << "You have connected to a node that charges "
             << "a fee to send transactions." << std::endl
             << std::endl
             << "The fee for sending transactions is: " << node->currency().amountFormatter()(nodeFee->amount)
             << " per transaction." << std::endl
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
