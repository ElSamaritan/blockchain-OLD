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

#include "DumpReader.h"

#include <cassert>
#include <utility>
#include <stdexcept>

#include "DumpHeader.h"

Xi::Result<std::unique_ptr<XiSync::DumpReader>> XiSync::DumpReader::open(const std::string &file,
                                                                         XiSync::DumpReader::Visitor &visitor) {
  XI_ERROR_TRY();
  return success(std::unique_ptr<DumpReader>{new DumpReader{file, visitor}});
  XI_ERROR_CATCH();
}

Xi::Result<bool> XiSync::DumpReader::next() {
  XI_ERROR_TRY();
  BatchInfo nfo;
  m_serializer(nfo, "");
  if (m_visitor.onInfo(nfo) == Visitor::BatchCommand::Skip) {
    m_stdStream.ignore(static_cast<int64_t>(nfo.BinarySize));
  } else {
    assert(m_visitor.onInfo(nfo) == Visitor::BatchCommand::Read);
    Batch batch;
    batch.Info = std::move(nfo);
    m_serializer(batch.Blocks, "");
    m_visitor.onBatch(std::move(batch));
  }
  return success(m_stdStream.peek() != EOF && !m_stdStream.eof());
  XI_ERROR_CATCH();
}

Xi::Result<void> XiSync::DumpReader::readAll() {
  XI_ERROR_TRY();
  while (next().valueOrThrow())
    ;
  return success();
  XI_ERROR_CATCH();
}

XiSync::DumpReader::DumpReader(const std::string &file, XiSync::DumpReader::Visitor &visitor)
    : m_visitor{visitor},
      m_stdStream{file, std::ios::binary | std::ios::in},
      m_streamWrapper{m_stdStream},
      m_serializer{m_streamWrapper} {
  if (!m_stdStream.good()) {
    throw std::runtime_error{std::string{"unable to open dump file: "} + file};
  }
  DumpHeader header;
  m_serializer(header, "");
  if (header.Version != 1) {
    throw std::runtime_error{std::string{"unsupported dump file version: "} + std::to_string(header.Version)};
  }
}
