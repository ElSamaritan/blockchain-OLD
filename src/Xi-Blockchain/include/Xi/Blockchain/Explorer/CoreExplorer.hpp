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

#include <memory>
#include <atomic>

#include <Xi/Concurrent/ReadersWriterLock.h>
#include <CryptoNoteCore/ICore.h>
#include <CryptoNoteCore/Transactions/ITransactionPoolObserver.h>

#include "Xi/Blockchain/Explorer/IExplorer.hpp"

namespace Xi {
namespace Blockchain {
namespace Explorer {

class CoreExplorer : public IExplorer, private CryptoNote::IBlockchainObserver, CryptoNote::ITransactionPoolObserver {
 public:
  explicit CoreExplorer(CryptoNote::ICore& core);
  ~CoreExplorer() override;

  // IExplorer
 public:
  // General
  Result<CurrencyInfo> queryCurrencyInfo() override;
  Result<Block::Height> mainChainHeight() override;

  // Blocks
  // -- ShortBlockInfo
  SingleResult<ShortBlockInfo> queryTopShortBlockInfo() override;
  VectorResult<ShortBlockInfo> queryShortBlockInfo(Block::ConstHashSpan hashes) override;
  VectorResult<ShortBlockInfo> queryShortBlockInfo(Block::ConstHeightSpan heights) override;

  // -- BlockInfo
  SingleResult<BlockInfo> queryTopBlockInfo() override;
  VectorResult<BlockInfo> queryBlockInfo(Block::ConstHashSpan hashes) override;
  VectorResult<BlockInfo> queryBlockInfo(Block::ConstHeightSpan heights) override;

  // -- DetailedBlockInfo
  SingleResult<DetailedBlockInfo> queryTopDetailedBlockInfo() override;
  VectorResult<DetailedBlockInfo> queryDetailedBlockInfo(Block::ConstHashSpan hashes) override;
  VectorResult<DetailedBlockInfo> queryDetailedBlockInfo(Block::ConstHeightSpan height) override;

  // Transactions
  VectorResult<ShortTransactionInfo> queryShortTransactionInfo(ConstTransactionHashSpan hashes) override;
  VectorResult<TransactionInfo> queryTransactionInfo(ConstTransactionHashSpan hashes) override;
  VectorResult<DetailedTransactionInfo> queryDetailedTransactionInfo(ConstTransactionHashSpan hashes) override;

  // Pool
  Result<ShortPoolInfo> queryShortPoolInfo() override;
  Result<PoolInfo> queryPoolInfo() override;

  // IBlockchainObserver
 private:
  void blockAdded(uint32_t index, const Block::Hash& hash) override;
  void mainChainSwitched(const CryptoNote::IBlockchainCache& previous, const CryptoNote::IBlockchainCache& current,
                         uint32_t splitIndex) override;

  // ITransactionPoolObserver
 private:
  void transactionDeletedFromPool(const TransactionHash& hash, DeletionReason reason) override;
  void transactionAddedToPool(const TransactionHash& hash, AdditionReason reason) override;

 private:
  ShortTransactionInputInfo fromCore(const CryptoNote::TransactionInput& input) const;
  ShortTransactionOutputInfo fromCore(const CryptoNote::TransactionOutput& output) const;
  TransactionInfo fromCore(const CryptoNote::CachedTransaction& transaction, bool isStaticReward,
                           TransactionContainer container) const;
  DetailedTransactionInfo fromCore(const CryptoNote::IBlockchainCache* segment,
                                   const CryptoNote::CachedTransactionInfo* info,
                                   const CryptoNote::CachedTransaction& transaction, bool isStaticReward,
                                   TransactionContainer container) const;

  ShortBlockInfo fromCore(const CryptoNote::CachedBlockInfo& info, const Block::Height& height,
                          bool isAlternative) const;
  BlockInfo fromCore(const CryptoNote::CachedRawBlock& raw, const CryptoNote::CachedBlockInfo& info,
                     const Block::Height& height, bool isAlternative) const;
  DetailedBlockInfo fromCore(const CryptoNote::IBlockchainCache* segment, const CryptoNote::CachedRawBlock& raw,
                             const CryptoNote::CachedBlockInfo& info, const Block::Height& height,
                             bool isAlternative) const;

 private:
  /// Guarantess to be not a nullptr iff no error is returned.
  Result<std::shared_ptr<DetailedBlockInfo>> topBlockInfo();
  /// Guarantess to be not a nullptr iff no error is returned.
  Result<std::shared_ptr<PoolInfo>> poolInfo();

  template <typename _InfoT>
  Result<std::vector<std::shared_ptr<_InfoT>>> doQueryBlockInfoByHashes(Block::ConstHashSpan hashes);

  Result<std::vector<std::shared_ptr<ShortBlockInfo>>> doQueryShortBlockInfo(Block::ConstHashSpan hash);
  Result<std::vector<std::shared_ptr<ShortBlockInfo>>> doQueryShortBlockInfo(const Block::ConstHeightSpan& height);

  Result<std::vector<std::shared_ptr<BlockInfo>>> doQueryBlockInfo(Block::ConstHashSpan hash);
  Result<std::vector<std::shared_ptr<BlockInfo>>> doQueryBlockInfo(Block::ConstHeightSpan height);

  Result<std::vector<std::shared_ptr<DetailedBlockInfo>>> doQueryDetailedBlockInfo(Block::ConstHashSpan hash);
  Result<std::vector<std::shared_ptr<DetailedBlockInfo>>> doQueryDetailedBlockInfo(Block::ConstHeightSpan height);

  Result<std::vector<std::shared_ptr<ShortTransactionInfo>>> doQueryShortTransactionInfo(
      ConstTransactionHashSpan hashes);
  Result<std::vector<std::shared_ptr<TransactionInfo>>> doQueryTransactionInfo(ConstTransactionHashSpan hashes,
                                                                               bool skipBlockReferences);
  Result<std::vector<std::shared_ptr<DetailedTransactionInfo>>> doQueryDetailedTransactionInfo(
      ConstTransactionHashSpan hashes);

 private:
  CryptoNote::ICore& m_core;

  using guard_type = Xi::Concurrent::ReadersWriterLock;

  // Cached values
  std::atomic<uint32_t> m_mainChainHeight;

  std::shared_ptr<DetailedBlockInfo> m_topBlock;
  guard_type m_topBlockGuard;

  std::shared_ptr<PoolInfo> m_poolInfo;
  guard_type m_poolInfoGuard;
};

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi
