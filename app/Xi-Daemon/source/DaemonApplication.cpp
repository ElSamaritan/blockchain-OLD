/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include <thread>
#include <future>

#include <Xi/App/Application.h>

#include <Common/Base58.h>
#include <Common/SignalHandler.h>
#include <Mnemonics/Mnemonics.h>
#include <Xi/FileSystem.h>
#include <Serialization/ConsoleOutputSerializer.hpp>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/Core.h>
#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/CryptoNoteTools.h>

#include "DaemonOptions.h"
#include "DaemonCommandsHandler.h"

using namespace Xi::App;

namespace {
void print_genesis_tx_hex(std::vector<std::string> rewardAddresses, const CryptoNote::CurrencyBuilder& currency,
                          Logging::ILogger& logManager) {
  for (auto& rewardAddress : rewardAddresses) {
    rewardAddress = Xi::trim(rewardAddress);
  }
  rewardAddresses.erase(
      std::remove_if(rewardAddresses.begin(), rewardAddresses.end(), [](const auto& i) { return i.empty(); }),
      rewardAddresses.end());

  Logging::LoggerRef logger{logManager, "GenesisTX"};
  std::vector<CryptoNote::AccountPublicAddress> rewardTargets;

  for (const auto& rewardAddress : rewardAddresses) {
    CryptoNote::AccountPublicAddress address;
    uint64_t prefix = 0;
    if (!parseAccountAddressString(prefix, address, rewardAddress) ||
        prefix != currency.immediateState().coin().prefix().base58()) {
      logger(Logging::Fatal) << "Failed to parse genesis reward address: " << rewardAddress << std::endl;
      return;
    }
    rewardTargets.emplace_back(std::move(address));
  }
  CryptoNote::Transaction transaction;
  if (rewardTargets.empty()) {
    if (currency.immediateState().coin().premine() > 0) {
      logger(Logging::Fatal) << "Genesis Block Reward Addresses are not defined" << std::endl;
      return;
    }
    transaction = currency.generateGenesisTransaction();
  } else {
    transaction = currency.generateGenesisTransaction(rewardTargets);
  }
  auto transactionBlob = CryptoNote::toBinaryArray(transaction);
  std::string transactionHex = Common::toHex(transactionBlob);
  std::cout << "Replace the current genesis_transaction propertiy in your config file with this one:" << std::endl
            << "\"" << transactionHex << "\"" << std::endl;

  return;
}
}  // namespace

namespace XiDaemon {

class DaemonApplication : public Application {
 public:
  DaemonApplication() : Application("xi-daemon", "blockchain network server") {
    useLogging(Logging::Warning);
    useCurrency();
    useNode();
    useRpcServer();
  }

  DaemonOptions Options{};

  void loadEnvironment(Xi::App::Environment& env) override;
  void makeOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;
  int run() override;
};

void DaemonApplication::loadEnvironment(Environment& env) {
  Application::loadEnvironment(env);
  Options.loadEnvironment(env);
}

void DaemonApplication::makeOptions(cxxopts::Options& options) {
  Application::makeOptions(options);
  Options.emplaceOptions(options);
}

bool DaemonApplication::evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) {
  XI_RETURN_SC_IF(Application::evaluateParsedOptions(options, result), true);
  XI_RETURN_SC_IF(Options.evaluateParsedOptions(options, result), true);
  if (Options.printGenesis()) {
    print_genesis_tx_hex(Options.rewardTargets(), *intermediateCurrency(), logger());
    XI_RETURN_SC(true);
  }
  XI_RETURN_SC(false);
}

int DaemonApplication::run() {
  Logging::LoggerRef log{logger(), "daemon"};

  const auto nodeServer = node();
  rpcServer();

  std::unique_ptr<DaemonCommandsHandler> cli{};
  std::future<int> cliRun{};
  if (!Options.noneInteractive()) {
    auto& lcore = static_cast<CryptoNote::Core&>(*core());
    cli = std::make_unique<DaemonCommandsHandler>(lcore, *node(), *protocol(), logger(), consoleLogger());
    cliRun = std::async(std::launch::async, [this, &cli, nodeServer]() {
      cli->run(name(), dataDirectory());
      nodeServer->sendStopSignal();
      return EXIT_SUCCESS;
    });
  } else {
    Tools::SignalHandler::install([this]() mutable { node()->sendStopSignal(); });
    cliRun = std::async(std::launch::deferred, []() { return EXIT_SUCCESS; });
  }

  if (!nodeServer->run()) {
    log(Logging::Fatal) << "node server startup failed";
    return EXIT_FAILURE;
  }

  return cliRun.get();
}

}  // namespace XiDaemon

XI_DECLARE_APP(XiDaemon::DaemonApplication)
