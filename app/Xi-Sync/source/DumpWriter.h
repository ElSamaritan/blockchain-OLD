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

#include <fstream>
#include <cinttypes>
#include <memory>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Common/StdOutputStream.h>
#include <Serialization/BinaryOutputStreamSerializer.h>
#include <CryptoNoteCore/CachedBlock.h>
#include <CryptoNoteCore/Blockchain/RawBlock.h>

namespace XiSync {
class DumpWriter final {
 public:
  static Xi::Result<std::unique_ptr<DumpWriter>> open(const std::string& file);

 public:
  XI_DELETE_COPY(DumpWriter);
  XI_DELETE_MOVE(DumpWriter);
  ~DumpWriter() = default;

  uint32_t checkpointsDensity() const;
  void setCheckpointsDensity(uint32_t density);

  Xi::Result<void> write(uint32_t startIndex, std::vector<CryptoNote::RawBlock> blockBatch);

 private:
  DumpWriter(const std::string& file);

  Crypto::Hash checkBlock(const CryptoNote::RawBlock& block) const;

 private:
  std::ofstream m_stdStream;
  Common::StdOutputStream m_streamWrapper;
  CryptoNote::BinaryOutputStreamSerializer m_serializer;
  uint32_t m_checkpointDensity = 100;
};
}  // namespace XiSync
