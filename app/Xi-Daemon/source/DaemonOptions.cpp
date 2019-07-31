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

#include "DaemonOptions.h"

namespace XiDaemon {

void DaemonOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("genesis")
    ("genesis-generate-transaction", "Generates the first reward of the blockchain, embedded into the genesis block",
      cxxopts::value<bool>(printGenesis())->implicit_value("true"))

    ("genesis-reward-targets", "Public addresses that shall receive the genesis transaction reward.",
      cxxopts::value<std::vector<std::string>>(rewardTargets()), "<address>*")
  ;

  options.add_options("daemon")
    ("n,none-interactive", "disabled interactive command line",
          cxxopts::value<bool>(noneInteractive())->implicit_value("true"))
  ;
  // clang-format on
}

bool DaemonOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  XI_RETURN_SC(false);
}

void DaemonOptions::loadEnvironment(Xi::App::Environment &env) { env(noneInteractive(), "NONE_INTERACTIVE"); }

}  // namespace XiDaemon
