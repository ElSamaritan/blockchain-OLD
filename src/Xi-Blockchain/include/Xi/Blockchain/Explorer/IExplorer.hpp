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
#include <optional>

#include <Xi/Result.h>

#include "Xi/Blockchain/Explorer/ExplorerError.hpp"
#include "Xi/Blockchain/Explorer/Data/Data.hpp"

namespace Xi {
namespace Blockchain {
namespace Explorer {

class IExplorer {
 public:
  template <typename _ResultT>
  using SingleResult = Result<std::optional<_ResultT>>;
  template <typename _ResultT>
  using VectorResult = Result<std::vector<std::optional<_ResultT>>>;

 public:
  virtual ~IExplorer() = default;

  // General
  virtual Result<CurrencyInfo> queryCurrencyInfo() = 0;
  virtual Result<Block::Height> mainChainHeight() = 0;

  // Blocks
  // -- ShortBlockInfo
  virtual SingleResult<ShortBlockInfo> queryTopShortBlockInfo() = 0;
  virtual VectorResult<ShortBlockInfo> queryShortBlockInfo(Block::ConstHashSpan hashes) = 0;
  virtual VectorResult<ShortBlockInfo> queryShortBlockInfo(Block::ConstHeightSpan heights) = 0;

  // -- BlockInfo
  virtual SingleResult<BlockInfo> queryTopBlockInfo() = 0;
  virtual VectorResult<BlockInfo> queryBlockInfo(Block::ConstHashSpan hashes) = 0;
  virtual VectorResult<BlockInfo> queryBlockInfo(Block::ConstHeightSpan heights) = 0;

  // -- DetailedBlockInfo
  virtual SingleResult<DetailedBlockInfo> queryTopDetailedBlockInfo() = 0;
  virtual VectorResult<DetailedBlockInfo> queryDetailedBlockInfo(Block::ConstHashSpan hashes) = 0;
  virtual VectorResult<DetailedBlockInfo> queryDetailedBlockInfo(Block::ConstHeightSpan height) = 0;

  // Transactions
  virtual VectorResult<ShortTransactionInfo> queryShortTransactionInfo(ConstTransactionHashSpan hashes) = 0;
  virtual VectorResult<TransactionInfo> queryTransactionInfo(ConstTransactionHashSpan hashes) = 0;
  virtual VectorResult<DetailedTransactionInfo> queryDetailedTransactionInfo(ConstTransactionHashSpan hashes) = 0;

  // Pool
  virtual Result<ShortPoolInfo> queryShortPoolInfo() = 0;
  virtual Result<PoolInfo> queryPoolInfo() = 0;

  // Boilerplate mapped to virtual functions, may override if better performing calls are possible
  virtual SingleResult<ShortBlockInfo> queryShortBlockInfo(Block::Height height);
  virtual SingleResult<ShortBlockInfo> queryShortBlockInfo(const Block::Hash& hash);
  virtual VectorResult<ShortBlockInfo> queryShortBlockInfo(Block::Height start, Block::Offset count);
  virtual VectorResult<ShortBlockInfo> queryShortBlockInfo(Block::Height start, Block::Height end);

  virtual SingleResult<BlockInfo> queryBlockInfo(Block::Height height);
  virtual SingleResult<BlockInfo> queryBlockInfo(const Block::Hash& hash);
  virtual VectorResult<BlockInfo> queryBlockInfo(Block::Height start, Block::Offset count);
  virtual VectorResult<BlockInfo> queryBlockInfo(Block::Height start, Block::Height end);

  virtual SingleResult<DetailedBlockInfo> queryDetailedBlockInfo(Block::Height height);
  virtual SingleResult<DetailedBlockInfo> queryDetailedBlockInfo(const Block::Hash& hash);
  virtual VectorResult<DetailedBlockInfo> queryDetailedBlockInfo(Block::Height start, Block::Offset count);
  virtual VectorResult<DetailedBlockInfo> queryDetailedBlockInfo(Block::Height start, Block::Height end);

  virtual SingleResult<ShortTransactionInfo> queryShortTransactionInfo(const TransactionHash& hash);
  virtual SingleResult<TransactionInfo> queryTransactionInfo(const TransactionHash& hash);
  virtual SingleResult<DetailedTransactionInfo> queryDetailedTransactionInfo(const TransactionHash& hash);

 private:
  Block::HeightVector makeRange(Block::Height start, Block::Height end) const;
};

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi
