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

#include <string>
#include <cinttypes>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/App/Environment.h>

namespace XiSync {
class SyncOptions {
 public:
  enum struct Method { Unknown, Import, LocalExport, RemoteExport };

 public:
  std::string DumpFile = "./xi-blockchain.dump";
  Method Usage = Method::Unknown;
  uint32_t BatchSize = 1000;
  uint32_t CheckpointsDensity = 100;
  bool TruncFile = false;
  bool UseRemote = false;

  void loadEnvironment(Xi::App::Environment& env);
  void emplaceOptions(cxxopts::Options& options);
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result);
};
}  // namespace XiSync
