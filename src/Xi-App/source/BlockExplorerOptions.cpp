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

#include "Xi/App/BlockExplorerOptions.h"

namespace Xi {
namespace App {

void BlockExplorerOptions::loadEnvironment(Environment &env) {
  // clang-format off
  env
    (enabled(), "BLOCK_EXPLORER")
  ;
  // clang-format on
}

void BlockExplorerOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("block explorer")
    ("block-explorer-enable", "Enable the Blockchain Explorer RPC",
      cxxopts::value<bool>(enabled())->default_value(enabled() ? "true" : "false")->implicit_value("true"))
  ;
  // clang-format on
}

bool BlockExplorerOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  XI_RETURN_SC(false);
}

}  // namespace App
}  // namespace Xi
