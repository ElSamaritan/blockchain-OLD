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

#include <fstream>
#include <memory>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Common/StdInputStream.h>
#include <Serialization/BinaryInputStreamSerializer.h>

#include "Batch.h"

namespace XiSync {
class DumpReader final {
 public:
  class Visitor {
   public:
    enum struct BatchCommand { Skip, Read };

   public:
    virtual ~Visitor() = default;

    virtual BatchCommand onInfo(const BatchInfo& info) = 0;
    virtual void onBatch(Batch info) = 0;
  };

 public:
  static Xi::Result<std::unique_ptr<DumpReader>> open(const std::string& file, Visitor& visitor);

 public:
  XI_DELETE_COPY(DumpReader);
  XI_DELETE_MOVE(DumpReader);
  ~DumpReader() = default;

  Xi::Result<bool> next();
  Xi::Result<void> readAll();

 private:
  DumpReader(const std::string& file, Visitor& visitor);

 private:
  Visitor& m_visitor;
  std::ifstream m_stdStream;
  Common::StdInputStream m_streamWrapper;
  CryptoNote::BinaryInputStreamSerializer m_serializer;
};
}  // namespace XiSync
