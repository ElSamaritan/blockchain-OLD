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

#pragma once

#include <cinttypes>
#include <thread>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/App/Environment.h>
#include <Serialization/ISerializer.h>

namespace XiMiner {
class MinerOptions {
 public:
  std::string Address = "";
  uint32_t Threads = static_cast<uint32_t>(std::thread::hardware_concurrency());
  uint16_t UpdateInterval = 2000;
  bool ReportShow = false;
  uint16_t ReportInterval = 5;  ///< Hashrate report interval in seconds.
  uint32_t BlockLimit = 0;
  bool Panic = false;  ///< Forces the application to abort if healthy checks fail.
  bool NoneInteractive = false;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(Address)
  KV_MEMBER(Threads)
  KV_MEMBER(UpdateInterval)
  KV_MEMBER(ReportInterval)
  KV_MEMBER(BlockLimit)
  KV_MEMBER(Panic)
  KV_MEMBER(NoneInteractive)
  KV_END_SERIALIZATION

  void loadEnvironment(Xi::App::Environment& env);
  void emplaceOptions(cxxopts::Options& options);
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result);
};
}  // namespace XiMiner
