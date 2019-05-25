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

#include "DumpWriter.h"

#include <stdexcept>
#include <numeric>

#include <Xi/Exceptional.hpp>
#include <Common/VectorOutputStream.h>
#include <CryptoNoteCore/CryptoNoteTools.h>

#include "Batch.h"
#include "DumpHeader.h"

namespace {
// clang-format off
XI_DECLARE_EXCEPTIONAL_CATEGORY(InvalidWrite)
XI_DECLARE_EXCEPTIONAL_INSTANCE(InsufficientBlocks, "you need to provide at least one block", InvalidWrite);
XI_DECLARE_EXCEPTIONAL_INSTANCE(TooManyBlocks, "you may not exceed a height of 2^32-1", InvalidWrite);
XI_DECLARE_EXCEPTIONAL_INSTANCE(InvalidRawBlock, "provided raw black cannot be deserialized", InvalidWrite);
XI_DECLARE_EXCEPTIONAL_INSTANCE(StreamCorrupted, "stream corrupted while writing", InvalidWrite);
// clang-format on
}  // namespace

Xi::Result<std::unique_ptr<XiSync::DumpWriter>> XiSync::DumpWriter::open(const std::string &file) {
  XI_ERROR_TRY();
  return Xi::make_result<std::unique_ptr<DumpWriter>>(new DumpWriter{file});
  XI_ERROR_CATCH();
}

uint32_t XiSync::DumpWriter::checkpointsDensity() const { return m_checkpointDensity; }

void XiSync::DumpWriter::setCheckpointsDensity(uint32_t density) { m_checkpointDensity = density; }

Xi::Result<void> XiSync::DumpWriter::write(uint32_t startIndex, std::vector<CryptoNote::RawBlock> blockBatch) {
  XI_ERROR_TRY();
  if (blockBatch.empty()) {
    Xi::exceptional<InsufficientBlocksError>();
  }
  if (static_cast<uint64_t>(startIndex) + blockBatch.size() > std::numeric_limits<uint32_t>::max()) {
    Xi::exceptional<TooManyBlocksError>();
  }

  Batch batch;
  batch.Info.StartIndex = startIndex;
  batch.Info.Count = static_cast<uint32_t>(blockBatch.size());
  batch.Info.Checkpoints[startIndex] = checkBlock(blockBatch[0]);
  batch.Info.Checkpoints[startIndex + batch.Info.Count - 1] = checkBlock(*blockBatch.rbegin());
  batch.Blocks = std::move(blockBatch);

  const uint32_t checkpointDensity = checkpointsDensity();
  for (uint32_t i = checkpointDensity; i < batch.Info.Count; i += checkpointDensity) {
    batch.Info.Checkpoints[startIndex + i] = checkBlock(batch.Blocks[i]);
  }

  CryptoNote::BinaryArray preSerializedRawBlocks{};
  {
    Common::VectorOutputStream intermediateStream{preSerializedRawBlocks};
    CryptoNote::BinaryOutputStreamSerializer rawBlockSerializer{intermediateStream};
    rawBlockSerializer(batch.Blocks, "");
    batch.Info.BinarySize = preSerializedRawBlocks.size();
  }

  m_serializer(batch.Info, "");
  m_stdStream.write(reinterpret_cast<char *>(preSerializedRawBlocks.data()),
                    static_cast<int64_t>(preSerializedRawBlocks.size()));

  if (!m_stdStream.good()) {
    Xi::exceptional<StreamCorruptedError>();
  }

  return Xi::make_result<void>();
  XI_ERROR_CATCH();
}

XiSync::DumpWriter::DumpWriter(const std::string &file)
    : m_stdStream{file, std::ios::binary | std::ios::out | std::ios::trunc},
      m_streamWrapper{m_stdStream},
      m_serializer{m_streamWrapper} {
  if (!m_stdStream.good()) {
    throw std::runtime_error{std::string{"unable to open dump file: "} + file};
  }
  DumpHeader header;
  m_serializer(header, "");
}

Crypto::Hash XiSync::DumpWriter::checkBlock(const CryptoNote::RawBlock &block) const {
  CryptoNote::BlockTemplate bt;
  if (!CryptoNote::fromBinaryArray(bt, block.block)) {
    Xi::exceptional<InvalidRawBlockError>();
  }
  CryptoNote::CachedBlock cb{bt};
  return cb.getBlockHash();
}
