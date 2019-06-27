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
#include <memory>

#include <Xi/ExternalIncludePush.h>
#include <cxxopts.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Logging/ILogger.h>
#include <Serialization/ISerializer.h>
#include <CryptoNoteCore/Checkpoints.h>

#include "Xi/App/IOptions.h"

namespace Xi {
namespace App {
struct CheckpointsOptions : public IOptions {
  std::string CheckpointsFile = "";
  bool UseCheckpoints = false;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(CheckpointsFile)
  KV_MEMBER(UseCheckpoints)
  KV_END_SERIALIZATION

  void emplaceOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;

  std::unique_ptr<CryptoNote::Checkpoints> getCheckpoints(Logging::ILogger& logger) const;
};
}  // namespace App
}  // namespace Xi
