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

#include "Importer.h"

#include <Xi/Exceptional.hpp>

namespace {
XI_DECLARE_EXCEPTIONAL_CATEGORY(SyncImport)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidBlock, "a block in the dump file is invalid", SyncImport)
}  // namespace

XiSync::Importer::Importer(CryptoNote::ICore &core, CryptoNote::Checkpoints &checkpoints, Logging::ILogger &logger)
    : m_core{core}, m_checkpoints{checkpoints}, m_logger{logger, "Sync-Importer"} {}

XiSync::DumpReader::Visitor::BatchCommand XiSync::Importer::onInfo(const XiSync::BatchInfo &info) {
  auto currentTopIndex = m_core.getTopBlockIndex();
  if (currentTopIndex >= info.StartIndex + info.Count) {
    return BatchCommand::Skip;
  } else {
    return BatchCommand::Read;
  }
}

void XiSync::Importer::onBatch(XiSync::Batch batch) {
  auto currentTopIndex = m_core.getTopBlockIndex();

  for (const auto &checkpoint : batch.Info.Checkpoints) {
    m_checkpoints.addCheckpoint(checkpoint.first, checkpoint.second);
  }

  for (uint32_t index = 0; index < batch.Blocks.size(); ++index) {
    if (currentTopIndex >= batch.Info.StartIndex + index) {
      continue;
    }
    auto res = m_core.addBlock(std::move(batch.Blocks[index]));
    if (res != CryptoNote::error::AddBlockErrorCondition::BLOCK_ADDED) {
      Xi::exceptional<InvalidBlockError>();
    }
  }
}
