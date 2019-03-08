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

#include "Exporter.h"

#include <algorithm>

#include <Xi/Exceptional.h>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(XiSyncExporter)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidIndex, "start index is greater than available top block index", XiSyncExporter)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidBatchSize, "batch size must be at least 100", XiSyncExporter)
}  // namespace

XiSync::Exporter::Exporter(XiSync::DumpWriter &writer) : m_writer{writer} {}

Xi::Result<void> XiSync::Exporter::exportBlocks(uint32_t startIndex, uint32_t count, uint32_t batchSize) {
  XI_ERROR_TRY();
  auto topIndex = topBlockIndex();
  if (topIndex < startIndex) {
    Xi::exceptional<InvalidIndexError>();
  }
  if (batchSize < 100) {
    Xi::exceptional<InvalidBatchSizeError>();
  }

  uint32_t totalWritten = 0;
  while (startIndex <= topIndex && totalWritten < count) {
    uint32_t querySize = std::min(batchSize, count - totalWritten);
    auto blocks = queryBlocks(startIndex, querySize);
    m_writer.write(0, std::move(blocks)).throwOnError();
    startIndex += querySize;
    totalWritten += querySize;
  }
  return Xi::make_result<void>();
  XI_ERROR_CATCH();
}
