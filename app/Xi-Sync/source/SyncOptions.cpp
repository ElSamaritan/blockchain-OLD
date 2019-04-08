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

#include "SyncOptions.h"

#include <cassert>

#include <Xi/Exceptional.h>
#include <Xi/FileSystem.h>

namespace {
// clang-format off
XI_DECLARE_EXCEPTIONAL_CATEGORY(SyncOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(MethodRequired, "please specify whether you want to import or export.", SyncOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(MissingImportFile, "specified import file was not found.", SyncOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(ExportFileExists, "specified export file exists and the trunc flag was not provided.", SyncOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(AmbigiousMethod, "you may only choose to import or export not both", SyncOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidBatchSize, "batch size must be at least 100.", SyncOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidCheckpointsDensity, "checkpoints density must be at least 1.", SyncOption)
XI_DECLARE_EXCEPTIONAL_INSTANCE(UnexpectedFlag, "provided flags was not expected.", SyncOption)
// clang-format on
}  // namespace

void XiSync::SyncOptions::emplaceOptions(cxxopts::Options &options) {
  // clang-format off
  options.add_options("sync")
    ("i,import", "imports a dump to a local blockchain database", cxxopts::value<bool>()->implicit_value("true"))
    ("e,export", "exports a dump from a local database or a blockexplorer enabled remote daemon", cxxopts::value<bool>()->implicit_value("true"))
    ("f,file", "the blockchain dump file to import/export", cxxopts::value<std::string>(DumpFile)->default_value(DumpFile))
    ("r,remote", "uses a remote daemon to create an export instead of a local database", cxxopts::value<bool>()->implicit_value("true"))
    ("b,batch-size", "number of blocks to emplace in every batch", cxxopts::value<uint32_t>(BatchSize)->default_value(std::to_string(BatchSize)))
    ("d,checkpoints-density", "number of blocks pushed at max before adding a new checkpoint",
        cxxopts::value<uint32_t>(CheckpointsDensity)->default_value(std::to_string(CheckpointsDensity)), "# > 0")
    ("t,trunc", "overrites the export file if present.", cxxopts::value<bool>(TruncFile)->implicit_value("true"))
  ;
  // clang-format on
}

bool XiSync::SyncOptions::evaluateParsedOptions(const cxxopts::Options &options, const cxxopts::ParseResult &result) {
  XI_UNUSED(options);
  if (result.count("import") == 0 && result.count("export") == 0) {
    Xi::exceptional<MethodRequiredError>();
  }
  if (result.count("import") > 0 && result.count("export") > 0) {
    Xi::exceptional<AmbigiousMethodError>();
  }

  if (result.count("import") > 0) {
    Usage = Method::Import;
    if (!Xi::FileSystem::exists(DumpFile).valueOrThrow()) {
      Xi::exceptional<MissingImportFileError>();
    }
    if (result.count("remote") > 0) {
      Xi::exceptional<UnexpectedFlagError>("you may only specify the remote flag on exports.");
    }
    if (result.count("batch-size") > 0) {
      Xi::exceptional<UnexpectedFlagError>("you may only specify the batch-size flag on exports.");
    }
    if (result.count("checkpoints-density") > 0) {
      Xi::exceptional<UnexpectedFlagError>("you may only specify the checkpoints-density flag on exports.");
    }
    if (BatchSize < 10) {
      Xi::exceptional<InvalidBatchSizeError>();
    }
  } else {
    assert(result.count("export") > 0);
    if (result.count("checkpoints") > 0) {
      Xi::exceptional<UnexpectedFlagError>("you may only specify the checkpoints flag on imports.");
    }
    if (BatchSize < 100) {
      Xi::exceptional<InvalidBatchSizeError>();
    }
    if (CheckpointsDensity < 1) {
      Xi::exceptional<InvalidCheckpointsDensityError>();
    }
    if (result.count("remote") > 0) {
      Usage = Method::RemoteExport;
    } else {
      Usage = Method::LocalExport;
    }
    if (Xi::FileSystem::exists(DumpFile).valueOrThrow() && !TruncFile) {
      Xi::exceptional<ExportFileExistsError>();
    }
  }

  return false;
}
