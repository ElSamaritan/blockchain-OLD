﻿/* ============================================================================================== *
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

#include "MinerOptions.h"

#include <Xi/Exceptional.hpp>
#include <Xi/Config/Coin.h>
#include <crypto/crypto-ops.h>
#include <Common/Base58.h>
#include <Common/StringTools.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/Currency.h>

namespace {
// clang-format off
XI_DECLARE_EXCEPTIONAL_CATEGORY(MinerOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidAddress, "invalid public address provided", MinerOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidThreadCount, "thread count may not be 0 or succeed local hardware currency", MinerOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidUpateInterval, "update interval may not be less than 50ms", MinerOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidReportInterval, "report interval may not be less than 1s", MinerOptions)
// clang-format on
}  // namespace

void XiMiner::MinerOptions::loadEnvironment(Xi::App::Environment &env) {
  // clang-format off
  env
    (Address, "MINER_ADDRESS")
    (Threads, "MINER_THREADS")
    (UpdateInterval, "MINER_UPDATE_INTERVAL")
    (ReportShow, "MINER_REPORT_SHOW")
    (ReportInterval, "MINER_REPORT_INTERVAL")
    (BlockLimit, "MINER_BLOCK_LIMIT")
    (Panic, "MINER_PANIC")
    (NoneInteractive, "NONE_INTERACTIVE")
  ;
  // clang-format on
}

void XiMiner::MinerOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("miner")
    ("a,address", "public address receiving block rewards.", cxxopts::value<std::string>(Address)->default_value(Address))
    ("t,threads", "number of threads to use.", cxxopts::value<uint32_t>(Threads)->default_value(std::to_string(Threads)), "# > 0")
    ("u,update-interval", "number of milliseconds to wait before checking for an update",
        cxxopts::value<uint16_t>(UpdateInterval)->default_value(std::to_string(UpdateInterval)), "ms >= 50")
    ("i,report-interval", "number of seconds between consecutive hashrate reports",
        cxxopts::value<uint16_t>(ReportInterval)->default_value(std::to_string(ReportInterval)), "s >= 1")
    ("r,report-show", "enables status reporting by default",
        cxxopts::value<bool>(ReportShow)->default_value(ReportShow ? "true": "false")->implicit_value("true"))
    ("l,block-limit", "maximum number of blocks to mine, after n blocks have been mined by this instance the aplication exits",
        cxxopts::value<uint32_t>(BlockLimit)->default_value(std::to_string(BlockLimit)))
    ("p,panic", "forces the application to exit if a health check fails",
        cxxopts::value<bool>(Panic)->default_value(Panic ? "true" : "false")->implicit_value("true"))
    ("n,none-interactive", "disabled interactive command line",
          cxxopts::value<bool>(NoneInteractive)->default_value(NoneInteractive ? "true" : "false")->implicit_value("true"))
  ;
  // clang-format on
}

bool XiMiner::MinerOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  uint64_t prefix = 0;
  std::string data;
  CryptoNote::AccountPublicAddress adr;
  if (!Tools::Base58::decode_addr(Address, prefix, data)) {
    Xi::exceptional<InvalidAddressError>();
  }
  if (!CryptoNote::fromBinaryArray(adr, Common::asBinaryArray(data))) {
    Xi::exceptional<InvalidAddressError>();
  }
  if (!Crypto::check_key(adr.spendPublicKey) || !Crypto::check_key(adr.viewPublicKey)) {
    Xi::exceptional<InvalidAddressError>();
  }

  if (Threads == 0 || Threads > std::thread::hardware_concurrency()) {
    Xi::exceptional<InvalidThreadCountError>();
  }

  if (UpdateInterval < 50) {
    Xi::exceptional<InvalidUpateIntervalError>();
  }

  if (ReportInterval < 1) {
    Xi::exceptional<InvalidReportIntervalError>();
  }

  return false;
}
