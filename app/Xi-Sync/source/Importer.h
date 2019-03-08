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

#pragma once

#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>
#include <CryptoNoteCore/ICore.h>
#include <CryptoNoteCore/Checkpoints.h>

#include "DumpReader.h"

namespace XiSync {
class Importer : public DumpReader::Visitor {
 public:
  Importer(CryptoNote::ICore& core, CryptoNote::Checkpoints& checkpoints, Logging::ILogger& logger);
  ~Importer() override = default;

  BatchCommand onInfo(const BatchInfo& info) override;
  void onBatch(Batch batch) override;

 private:
  CryptoNote::ICore& m_core;
  CryptoNote::Checkpoints& m_checkpoints;
  Logging::LoggerRef m_logger;
};
}  // namespace XiSync
