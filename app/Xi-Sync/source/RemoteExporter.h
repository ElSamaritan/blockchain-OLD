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

#include <vector>

#include <Xi/Result.h>
#include <CryptoNoteCore/CryptoNote.h>
#include <CryptoNoteCore/INode.h>

#include "Exporter.h"

namespace XiSync {
class RemoteExporter : public Exporter {
 public:
  RemoteExporter(CryptoNote::INode& remote, DumpWriter& writer, Logging::ILogger& logger);
  ~RemoteExporter() override = default;

  uint32_t topBlockIndex() const override;
  std::vector<CryptoNote::RawBlock> queryBlocks(uint32_t startIndex, uint32_t count) const override;

 private:
  CryptoNote::INode& m_remote;
};
}  // namespace XiSync
