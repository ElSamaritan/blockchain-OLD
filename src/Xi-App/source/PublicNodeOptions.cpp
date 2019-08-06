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

#include "Xi/App/PublicNodeOptions.h"

#include <Xi/Exceptional.hpp>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(PublicNodeOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(Inconsistent, "public fee options provided are inconsistent", PublicNodeOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidFeeAmount, "public fee amount may not be negative", PublicNodeOptions)
}  // namespace

namespace Xi {
namespace App {

void PublicNodeOptions::loadEnvironment(Environment &env) {
  // clang-format off
  env
    (enabled(), "PUBLIC_NODE")
    (mining(), "PUBLIC_NODE_MINING")
    (fee().address(), "PUBLIC_NODE_FEE_ADDRESS")
    (fee().viewKey(), "PUBLIC_NODE_FEE_VIEW_KEY")
    (fee().amount(), "PUBLIC_NODE_FEE_AMOUNT")
  ;
  // clang-format on
}

void PublicNodeOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("public node")
    ("public-node-disable", "Disables public node capabilities of this instance.",
      cxxopts::value(enabled())->default_value(enabled() ?  "true" : "false")->implicit_value("false"))

    ("public-node-mining-disable", "Disables public node mining capabilities of this instance.",
      cxxopts::value<bool>()->default_value(!mining() ?  "true" : "false")->implicit_value("true"))

    ("public-node-fee-address", "Sets the convenience charge <address> for light wallets that use the daemon",
      cxxopts::value<std::string>(fee().address()), "<address>")

    ("public-node-fee-view-key", "Fee address private view key to validate fees are payed.",
      cxxopts::value<std::string>(fee().viewKey()), "<private-key>")

    ("public-node-fee-amount", "Sets the convenience charge amount for light wallets that use the daemon",
      cxxopts::value<int64_t>()->default_value(std::to_string(fee().amount())), "#");
  // clang-format on
}

bool PublicNodeOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options);
  XI_RETURN_SC_IF_NOT(enabled(), false);

  if (result.count("public-node-mining-disable") > 0) {
    mining(!result["public-node-mining-disable"].as<bool>());
  }

  if (result.count("public-node-fee-amount") > 0) {
    int64_t amount = result["public-node-fee-amount"].as<int64_t>();
    exceptional_if<InvalidFeeAmountError>(amount < 0);
    fee().amount(static_cast<uint64_t>(amount));
  }

  bool isPartiallyProvided = !fee().address().empty() || !fee().viewKey().empty() || fee().amount() > 0;

  exceptional_if<InconsistentError>(isPartiallyProvided && fee().address().empty(), "fee address is missing");
  exceptional_if<InconsistentError>(isPartiallyProvided && fee().amount() == 0, "fee amount is missing");

  XI_RETURN_SC(false);
}

}  // namespace App
}  // namespace Xi
