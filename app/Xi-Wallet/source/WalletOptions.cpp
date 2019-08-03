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

#include "WalletOptions.h"

#include <Xi/Exceptional.hpp>
#include <Xi/FileSystem.h>
#include <Xi/Algorithm/String.h>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(WalletOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(AlreadyExists, "specified wallet to generate already exists.", WalletOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InconsistentGenerate, "generate seed was specified but not the generate flag",
                                WalletOptions)
}  // namespace

namespace XiWallet {

void WalletOptions::loadEnvironment(Xi::App::Environment &env) {
  // clang-format off
  env
    (wallet(), "WALLET_FILE")
    (password(), "WALLET_PASSWORD")
    (useRemote(), "WALLET_REMOTE")
    (generate(), "WALLET_GENERATE")
    (generateSeed(), "WALLET_SEED")
  ;
  // clang-format on
}

void WalletOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("wallet")
    ("w,wallet", "wallet filepath to open/generate",
      cxxopts::value<std::string>(wallet())->default_value(wallet()), "<filepath>|<empty>")

    ("p,password", "wallet password for decryption",
      cxxopts::value<std::string>(password())->default_value(password()), "<password>")

    ("g,generate", "generates a new wallet, if wallet file is empty, keys are printed to console",
      cxxopts::value<bool>(generate())->default_value(generate() ? "true" : "false")->implicit_value("true"))

    ("s,seed", "uses the given seed on wallet generation (not mnemonics)",
      cxxopts::value<std::string>(generateSeed())->default_value(generateSeed()), "<string>")

    ("r,remote", "enables remote usage instead of an embedded node",
      cxxopts::value<bool>(useRemote())->default_value(useRemote() ? "true" : "false"), "<enabled>")
  ;
  // clang-format on
}

bool WalletOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  using namespace Xi;
  XI_UNUSED(options, result);

  if (generate() && !wallet().empty()) {
    if (const auto ec = FileSystem::exists(wallet()); !ec.isError() && ec.value()) {
      exceptional<AlreadyExistsError>();
    }
  }

  exceptional_if<InconsistentGenerateError>(!generateSeed().empty() && !generate());

  return false;
}

}  // namespace XiWallet
