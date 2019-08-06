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

#include <Xi/App/Application.h>

#include <Common/Base58.h>
#include <Mnemonics/Mnemonics.h>
#include <Xi/FileSystem.h>
#include <Serialization/ConsoleOutputSerializer.hpp>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/CryptoNoteTools.h>

#include "WalletOptions.h"
#include "ZedWallet.h"

using namespace Xi::App;

namespace {
int generate(Logging::ILogger& logger, const CryptoNote::Currency& currency, const std::string& seed = "") {
  CryptoNote::CurrencyBuilder builder{logger};
  CryptoNote::AccountKeys account;
  if (seed.empty()) {
    Crypto::generate_keys(account.address.spendPublicKey, account.spendSecretKey);
  } else {
    Crypto::generate_deterministic_keys(account.address.spendPublicKey, account.spendSecretKey, Xi::asByteSpan(seed));
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

namespace XiWallet {

class WalletApplication : public Application {
 public:
  WalletApplication() : Application("xi-wallet", "cli interface to manage/create wallets and transactions.") {
    useLogging(Logging::None);
    useCurrency();
    useNode();
    useRemoteRpc();
  }

  WalletOptions Options{};

  void loadEnvironment(Xi::App::Environment& env) override;
  void makeOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;
  int run() override;
};

void WalletApplication::loadEnvironment(Environment& env) {
  Application::loadEnvironment(env);
  Options.loadEnvironment(env);
}

void WalletApplication::makeOptions(cxxopts::Options& options) {
  this->Application::makeOptions(options);
  Options.emplaceOptions(options);
}

bool WalletApplication::evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) {
  XI_RETURN_SC_IF(this->Application::evaluateParsedOptions(options, result), true);
  XI_RETURN_SC_IF(Options.evaluateParsedOptions(options, result), true);
  XI_RETURN_SC(false);
}

int WalletApplication::run() {
  if (Options.generate() && Options.wallet().empty()) {
    return generate(logger(), intermediateCurrency()->immediateState(), Options.generateSeed());
  }

  auto remote = rpcNode(true, !Options.useRemote());

  if (!Options.useRemote()) {
    auto zedWalletRun = std::async(std::launch::async, [this, remote]() -> int {
      const int ec = execZedWallet(nullptr, *remote, Options, *currency(), logger());
      node()->sendStopSignal();
      return ec;
    });
    if (!node()->run()) {
      return EXIT_FAILURE;
    }
    return zedWalletRun.get();
  } else {
    return execZedWallet(std::addressof(dispatcher()), *remote, Options, *currency(), logger());
  }
}

}  // namespace XiWallet

XI_DECLARE_APP(XiWallet::WalletApplication)
