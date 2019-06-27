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

#include <optional>

#include <Xi/Global.hh>
#include <Xi/Concurrent/ReadersWriterLock.h>

#include "CryptoNoteCore/CachedBlock.h"
#include "CryptoNoteCore/Transactions/CachedTransaction.h"

namespace CryptoNote {
struct RawBlock {
  BinaryArray blockTemplate;  // BlockTemplate
  std::vector<BinaryArray> transactions;
};

using RawBlockVector = std::vector<RawBlock>;
XI_DECLARE_SPANS(RawBlock)

class CachedRawBlock {
 public:
  explicit CachedRawBlock(RawBlock raw);
  XI_DELETE_COPY(CachedRawBlock);
  CachedRawBlock(CachedRawBlock&& other);
  CachedRawBlock& operator=(CachedRawBlock&& other);
  ~CachedRawBlock() = default;

  const RawBlock& raw() const;

  const CachedBlock& block() const;

  size_t blobSize() const;

  size_t transactionCount() const;
  size_t transferCount() const;
  const CachedTransaction& operator[](size_t index) const;

 private:
  RawBlock m_raw;
  Xi::Concurrent::ReadersWriterLock m_guard;
  mutable std::optional<CachedBlock> m_block;
  mutable std::vector<std::optional<CachedTransaction>> m_transactions;
};

using CachedRawBlockVector = std::vector<CachedRawBlock>;
XI_DECLARE_SPANS(CachedRawBlock)

CachedRawBlock cache(RawBlock block);
CachedRawBlockVector cache(RawBlockVector blocks);

}  // namespace CryptoNote
