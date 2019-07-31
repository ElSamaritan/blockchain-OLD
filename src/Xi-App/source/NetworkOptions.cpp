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

#include "Xi/App/NetworkOptions.h"

#include <Xi/Global.hh>
#include <Xi/Exceptional.hpp>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(NetworkOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(UnrecognizedNetwork, "given network is unregcognized.", NetworkOption)
}  // namespace

void Xi::App::NetworkOptions::loadEnvironment(Xi::App::Environment &env) {
  // clang-format off
  env
    (directory(), "NETWORK_DIR")
    (network(), "NETWORK")
  ;
  // clang-format on
}

void Xi::App::NetworkOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("network")
    ("network", "the xi network you want to operate on.", cxxopts::value<std::string>()->default_value(network()),
        "<NAME>.[MainNet|StageNet|TestNet|LocalTestNet]")
    ("network-dir", "Directory to search for additional network configuration files.",
     cxxopts::value<std::string>()->default_value(directory()))
  ;
  // clang-format on
}

bool Xi::App::NetworkOptions::evaluateParsedOptions(const cxxopts::Options &options,
                                                    const cxxopts::ParseResult &result) {
  XI_UNUSED(options);
  if (result.count("network")) {
    network() = result["network"].as<std::string>();
  }
  if (result.count("network-dir")) {
    directory() = result["network-dir"].as<std::string>();
  }
  return false;
}

#undef XI_NETWORK_CASE
