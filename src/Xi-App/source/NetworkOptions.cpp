/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include "Xi/App/NetworkOptions.h"

#include <Xi/Global.h>
#include <Xi/Exceptional.h>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(NetworkOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(UnrecognizedNetwork, "given network is unregcognized.", NetworkOption)
}  // namespace

void Xi::App::NetworkOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("network")
    ("network", "the xi network you want to operate on.", cxxopts::value<std::string>()->default_value(Xi::to_string(Network)),
        "MainNet,StageNet,TestNet,LocalTestNet")
  ;
  // clang-format on
}

#define XI_NETWORK_CASE(NET)                                                  \
  if (network == Xi::to_lower(Xi::to_string(Xi::Config::Network::Type::NET))) \
    Network = Xi::Config::Network::Type::NET;                                 \
  else

bool Xi::App::NetworkOptions::evaluateParsedOptions(const cxxopts::Options &options,
                                                    const cxxopts::ParseResult &result) {
  XI_UNUSED(options);
  if (result.count("network") == 0) {
    return false;
  }
  auto network = Xi::to_lower(result["network"].as<std::string>());
  XI_NETWORK_CASE(MainNet)
  XI_NETWORK_CASE(StageNet)
  XI_NETWORK_CASE(TestNet)
  XI_NETWORK_CASE(LocalTestNet)
  Xi::exceptional<UnrecognizedNetworkError>();
  return false;
}

#undef XI_NETWORK_CASE
