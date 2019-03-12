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

#include <vector>

#include <Xi/Result.h>
#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>
#include <CryptoNoteCore/CryptoNote.h>

#include "DumpWriter.h"

namespace XiSync {
class Exporter {
 protected:
  Exporter(DumpWriter& writer, Logging::ILogger& logger);

 public:
  virtual ~Exporter() = default;

  virtual uint32_t topBlockIndex() const = 0;
  virtual std::vector<CryptoNote::RawBlock> queryBlocks(uint32_t startIndex, uint32_t count) const = 0;

 public:
  Xi::Result<void> exportBlocks(uint32_t startIndex, uint32_t count, uint32_t batchSize);

 private:
  DumpWriter& m_writer;
  Logging::LoggerRef m_logger;
};
}  // namespace XiSync
