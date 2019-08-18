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

#include "Xi/Blockchain/Explorer/IExplorer.hpp"

#include <Xi/Exceptions.hpp>

namespace Xi {
namespace Blockchain {
namespace Explorer {

IExplorer::SingleResult<ShortBlockInfo> IExplorer::queryShortBlockInfo(Block::Height height) {
  if (auto result = queryShortBlockInfo(makeSpan(height)); !result.isError()) {
    XI_RETURN_EC_IF(result->empty(), emplaceSuccess<std::optional<ShortBlockInfo>>(std::nullopt));
    return success(result->front());
  } else {
    return result.error();
  }
}

IExplorer::SingleResult<ShortBlockInfo> IExplorer::queryShortBlockInfo(const Block::Hash& hash) {
  if (auto result = queryShortBlockInfo(makeSpan(hash)); !result.isError()) {
    XI_RETURN_EC_IF(result->empty(), emplaceSuccess<std::optional<ShortBlockInfo>>(std::nullopt));
    return success(result->front());
  } else {
    return result.error();
  }
}

IExplorer::VectorResult<ShortBlockInfo> IExplorer::queryShortBlockInfo(Block::Height start, Block::Offset count) {
  return queryShortBlockInfo(start, start + count);
}

IExplorer::VectorResult<ShortBlockInfo> IExplorer::queryShortBlockInfo(Block::Height start, Block::Height end) {
  XI_ERROR_TRY
  return queryShortBlockInfo(makeRange(start, end));
  XI_ERROR_CATCH
}

IExplorer::SingleResult<BlockInfo> IExplorer::queryBlockInfo(Block::Height height) {
  if (auto result = queryBlockInfo(makeSpan(height)); !result.isError()) {
    XI_RETURN_EC_IF(result->empty(), emplaceSuccess<std::optional<BlockInfo>>(std::nullopt));
    return success(result->front());
  } else {
    return result.error();
  }
}

IExplorer::SingleResult<BlockInfo> IExplorer::queryBlockInfo(const Block::Hash& hash) {
  if (auto result = queryBlockInfo(makeSpan(hash)); !result.isError()) {
    XI_RETURN_EC_IF(result->empty(), emplaceSuccess<std::optional<BlockInfo>>(std::nullopt));
    return success(result->front());
  } else {
    return result.error();
  }
}

IExplorer::VectorResult<BlockInfo> IExplorer::queryBlockInfo(Block::Height start, Block::Offset count) {
  return queryBlockInfo(start, start + count);
}

IExplorer::VectorResult<BlockInfo> IExplorer::queryBlockInfo(Block::Height start, Block::Height end) {
  XI_ERROR_TRY
  return queryBlockInfo(makeRange(start, end));
  XI_ERROR_CATCH
}

IExplorer::SingleResult<DetailedBlockInfo> IExplorer::queryDetailedBlockInfo(Block::Height height) {
  if (auto result = queryDetailedBlockInfo(makeSpan(height)); !result.isError()) {
    XI_RETURN_EC_IF(result->empty(), emplaceSuccess<std::optional<DetailedBlockInfo>>(std::nullopt));
    return success(result->front());
  } else {
    return result.error();
  }
}

IExplorer::SingleResult<DetailedBlockInfo> IExplorer::queryDetailedBlockInfo(const Block::Hash& hash) {
  if (auto result = queryDetailedBlockInfo(makeSpan(hash)); !result.isError()) {
    XI_RETURN_EC_IF(result->empty(), emplaceSuccess<std::optional<DetailedBlockInfo>>(std::nullopt));
    return success(result->front());
  } else {
    return result.error();
  }
}

IExplorer::VectorResult<DetailedBlockInfo> IExplorer::queryDetailedBlockInfo(Block::Height start, Block::Offset count) {
  return queryDetailedBlockInfo(start, start + count);
}

IExplorer::VectorResult<DetailedBlockInfo> IExplorer::queryDetailedBlockInfo(Block::Height start, Block::Height end) {
  XI_ERROR_TRY
  return queryDetailedBlockInfo(makeRange(start, end));
  XI_ERROR_CATCH
}

IExplorer::SingleResult<ShortTransactionInfo> IExplorer::queryShortTransactionInfo(const TransactionHash& hash) {
  if (auto result = queryShortTransactionInfo(makeSpan(hash)); !result.isError()) {
    XI_RETURN_EC_IF(result->empty(), emplaceSuccess<std::optional<ShortTransactionInfo>>(std::nullopt));
    return success(result->front());
  } else {
    return result.error();
  }
}

IExplorer::SingleResult<TransactionInfo> IExplorer::queryTransactionInfo(const TransactionHash& hash) {
  if (auto result = queryTransactionInfo(makeSpan(hash)); !result.isError()) {
    XI_RETURN_EC_IF(result->empty(), emplaceSuccess<std::optional<TransactionInfo>>(std::nullopt));
    return success(result->front());
  } else {
    return result.error();
  }
}

IExplorer::SingleResult<DetailedTransactionInfo> IExplorer::queryDetailedTransactionInfo(const TransactionHash& hash) {
  if (auto result = queryDetailedTransactionInfo(makeSpan(hash)); !result.isError()) {
    XI_RETURN_EC_IF(result->empty(), emplaceSuccess<std::optional<DetailedTransactionInfo>>(std::nullopt));
    return success(result->front());
  } else {
    return result.error();
  }
}

Block::HeightVector IExplorer::makeRange(Block::Height start, Block::Height end) const {
  exceptional_if<InvalidArgumentError>(start.isNull(), "start height is null");
  exceptional_if<InvalidArgumentError>(end.isNull(), "end height is null");

  if (start > end) {
    std::swap(start, end);
  }

  Block::HeightVector reval{};
  reval.reserve(end.native() - start.native());
  for (; start != end; start.advance(1)) {
    reval.push_back(start);
  }
  return reval;
}

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi
