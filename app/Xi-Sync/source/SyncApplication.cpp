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

#include <numeric>

#include <Xi/App/Application.h>
#include <CryptoNoteCore/Core.h>

#include "SyncOptions.h"
#include "DumpReader.h"
#include "DumpWriter.h"
#include "Importer.h"
#include "LocalExporter.h"
#include "RemoteExporter.h"

using namespace Xi::App;
using namespace XiSync;

namespace {
class SyncApplication : public Application {
 public:
  SyncApplication() : Application("xi-sync", "imports/exports the blockchain dumps") {
    useLogging();
    useDatabase();
    useCore();
    useRemoteRpc();
  }

  XiSync::SyncOptions Options{};

  void makeOptions(cxxopts::Options& options) override;
  bool evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) override;
  int run() override;

  void import();
  void localExport();
  void remoteExport();
};

}  // namespace

XI_DECLARE_APP(SyncApplication)

void SyncApplication::makeOptions(cxxopts::Options& options) {
  this->Application::makeOptions(options);
  Options.emplaceOptions(options);
}

bool SyncApplication::evaluateParsedOptions(const cxxopts::Options& options, const cxxopts::ParseResult& result) {
  return this->Application::evaluateParsedOptions(options, result) || Options.evaluateParsedOptions(options, result);
}

int SyncApplication::run() {
  if (Options.Usage == SyncOptions::Method::Import) {
    import();
  } else if (Options.Usage == SyncOptions::Method::LocalExport) {
    localExport();
  } else if (Options.Usage == SyncOptions::Method::RemoteExport) {
    remoteExport();
  }
  return 0;
}

void SyncApplication::import() {
  Importer import{*core(), *checkpoints(), logger()};
  auto reader = DumpReader::open(Options.DumpFile, import).takeOrThrow();
  reader->readAll().throwOnError();
}

void SyncApplication::localExport() {
  auto writer = DumpWriter::open(Options.DumpFile).takeOrThrow();
  writer->setCheckpointsDensity(Options.CheckpointsDensity);
  LocalExporter exporter{*core(), *writer};
  exporter.exportBlocks(0, std::numeric_limits<uint32_t>::max(), Options.BatchSize).throwOnError();
}

void SyncApplication::remoteExport() {
  auto writer = DumpWriter::open(Options.DumpFile).takeOrThrow();
  writer->setCheckpointsDensity(Options.CheckpointsDensity);
  RemoteExporter exporter{*remoteNode(), *writer};
  exporter.exportBlocks(0, std::numeric_limits<uint32_t>::max(), Options.BatchSize).throwOnError();
}
