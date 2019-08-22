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

#include "Xi/App/CheckpointsOptions.h"

#include <Xi/Global.hh>
#include <Xi/Exceptional.hpp>
#include <Xi/FileSystem.h>
#include <Xi/Config/Network.h>
#include <Xi/Resources/Checkpoints.hpp>
#include <CryptoNoteCore/Checkpoints.h>

namespace {
// clang-format off
XI_DECLARE_EXCEPTIONAL_CATEGORY(CheckpointsOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(CheckpointFileMissing, "provided checkpoints import file was not found.", CheckpointsOptions)
XI_DECLARE_EXCEPTIONAL_INSTANCE(CheckpointImport, "import of checkpoints failed", CheckpointsOptions)
// clang-format on
}  // namespace

void Xi::App::CheckpointsOptions::loadEnvironment(Xi::App::Environment &env) {
  // clang-format off
  env
    (UseCheckpoints, "CHECKPOINTS")
    (CheckpointsFile, "CHECKPOINTS_IMPORT")
  ;
  // clang-format on
}

void Xi::App::CheckpointsOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("checkpoints")
      ("checkpoints-disable", "enables the usage checkpoints, making synchronization faster",
        cxxopts::value<bool>(UseCheckpoints)->default_value(UseCheckpoints ? "true" : "false")
                                            ->implicit_value("false"))

      ("checkpoints-import", "imports additional checkpoints from a csv file",
        cxxopts::value<std::string>(CheckpointsFile)->default_value(CheckpointsFile))
  ;
  // clang-format on
}

bool Xi::App::CheckpointsOptions::evaluateParsedOptions(const cxxopts::Options &options,
                                                        const cxxopts::ParseResult &result) {
  XI_UNUSED(options, result);
  if (CheckpointsFile.size() > 0) {
    if (!FileSystem::exists(CheckpointsFile).valueOrThrow()) {
      exceptional<CheckpointFileMissingError>();
    }
  }
  return false;
}

std::unique_ptr<CryptoNote::Checkpoints> Xi::App::CheckpointsOptions::getCheckpoints(
    const CryptoNote::Currency &currency, Logging::ILogger &logger) const {
  Resources::loadCheckpoints();
  auto reval = std::make_unique<CryptoNote::Checkpoints>(logger);
  reval->setEnabled(UseCheckpoints);
  if (UseCheckpoints) {
    std::string checkpointPath = CheckpointsFile;
    if (CheckpointsFile.empty()) {
      checkpointPath = Xi::Config::Network::checkpoints(currency.network().type());
    }
    if (!checkpointPath.empty()) {
      exceptional_if_not<CheckpointImportError>(reval->loadCheckpoints(checkpointPath));
    }
  }
  return reval;
}
