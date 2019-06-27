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

#include "CryptoNoteCore/Blockchain/RawBlock.h"

#include <utility>
#include <numeric>

#include <Xi/Exceptions.hpp>

#include "CryptoNoteCore/CryptoNoteTools.h"

CryptoNote::CachedRawBlock::CachedRawBlock(RawBlock _raw) : m_raw(std::move(_raw)) {
  m_transactions.resize(m_raw.transactions.size(), std::nullopt);
}

CryptoNote::CachedRawBlock::CachedRawBlock(CryptoNote::CachedRawBlock &&other)
    : m_raw{std::move(other.raw())},
      m_block{std::move(other.m_block)},
      m_transactions{std::move(other.m_transactions)} {
  /* */
}

CryptoNote::CachedRawBlock &CryptoNote::CachedRawBlock::operator=(CryptoNote::CachedRawBlock &&other) {
  XI_CONCURRENT_LOCK_WRITE(m_guard);
  m_raw = std::move(other.m_raw);

  if (other.m_block) {
    m_block.emplace(std::move(*other.m_block));
  }
  m_transactions = std::move(other.m_transactions);
  return *this;
}

const CryptoNote::RawBlock &CryptoNote::CachedRawBlock::raw() const { return m_raw; }

const CryptoNote::CachedBlock &CryptoNote::CachedRawBlock::block() const {
  XI_CONCURRENT_LOCK_PREPARE_WRITE(m_guard);
  if (!m_block.has_value()) {
    XI_CONCURRENT_LOCK_ACQUIRE_WRITE(m_guard);
    if (!m_block.has_value()) {
      m_block.emplace(fromBinaryArray<BlockTemplate>(raw().blockTemplate));
    }
  }
  return *m_block;
}

size_t CryptoNote::CachedRawBlock::blobSize() const {
  return block().coinbase().getBlobSize() +
         std::accumulate(begin(raw().transactions), end(raw().transactions), 0ULL,
                         [](auto acc, const auto &blob) { return acc + blob.size(); });
}

size_t CryptoNote::CachedRawBlock::transactionCount() const {
  return (block().getBlock().staticRewardHash ? 2 : 1) + m_raw.transactions.size();
}

size_t CryptoNote::CachedRawBlock::transferCount() const { return m_transactions.size(); }

const CryptoNote::CachedTransaction &CryptoNote::CachedRawBlock::operator[](size_t index) const {
  Xi::exceptional_if_not<Xi::OutOfRangeError>(index < m_transactions.size());
  XI_CONCURRENT_LOCK_PREPARE_WRITE(m_guard);
  if (!m_transactions[index].has_value()) {
    XI_CONCURRENT_LOCK_ACQUIRE_WRITE(m_guard);
    if (!m_transactions[index].has_value()) {
      m_transactions[index].emplace(CachedTransaction{fromBinaryArray<Transaction>(raw().transactions[index])});
    }
  }
  return *m_transactions[index];
}

CryptoNote::CachedRawBlock CryptoNote::cache(CryptoNote::RawBlock block) { return CachedRawBlock{std::move(block)}; }

CryptoNote::CachedRawBlockVector CryptoNote::cache(CryptoNote::RawBlockVector blocks) {
  CachedRawBlockVector reval{};
  reval.reserve(blocks.size());
  for (auto &block : blocks) {
    reval.emplace_back(std::move(block));
  }
  return reval;
}
