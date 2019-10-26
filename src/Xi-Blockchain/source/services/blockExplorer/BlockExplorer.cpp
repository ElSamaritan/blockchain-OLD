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

#include "Xi/Blockchain/Services/BlockExplorer/BlockExplorer.hpp"

#include <algorithm>

#define CHECK_EXPLORER                            \
  if (!m_explorer) {                              \
    return Xi::Rpc::ServiceError::NotInitialized; \
  }

#define CHECK_ARG(...)                             \
  if (!this->check(__VA_ARGS__)) {                 \
    return Xi::Rpc::ServiceError::InvalidArgument; \
  }

namespace Xi {
namespace Blockchain {
namespace Services {
namespace BlockExplorer {

using Error = Rpc::ServiceError;

std::shared_ptr<BlockExplorer> BlockExplorer::create(std::shared_ptr<Explorer::IExplorer> explorer,
                                                     Logging::ILogger &logger) {
  std::shared_ptr<BlockExplorer> reval{new BlockExplorer{explorer, logger}};
  reval->addService("info.limits", std::static_pointer_cast<InfoLimitsService>(reval));
  reval->addService("info.currency", std::static_pointer_cast<InfoCurrencyService>(reval));
  reval->addService("info.node", std::static_pointer_cast<InfoNodeStatusService>(reval));

  reval->addService("block.short", std::static_pointer_cast<ShortBlockInfoService>(reval));
  reval->addService("block.batch.short", std::static_pointer_cast<BatchShortBlockInfoService>(reval));
  reval->addService("block", std::static_pointer_cast<BlockInfoService>(reval));
  reval->addService("block.batch", std::static_pointer_cast<BatchBlockInfoService>(reval));
  reval->addService("block.detailed", std::static_pointer_cast<DetailedBlockInfoService>(reval));
  reval->addService("block.batch.detailed", std::static_pointer_cast<BatchDetailedBlockInfoService>(reval));

  reval->addService("transaction.short", std::static_pointer_cast<ShortTransactionInfoService>(reval));
  reval->addService("transaction.batch.short", std::static_pointer_cast<BatchShortTransactionInfoService>(reval));
  reval->addService("transaction", std::static_pointer_cast<TransactionInfoService>(reval));
  reval->addService("transaction.batch", std::static_pointer_cast<BatchTransactionInfoService>(reval));
  reval->addService("transaction.detailed", std::static_pointer_cast<DetailedTransactionInfoService>(reval));
  reval->addService("transaction.batch.detailed", std::static_pointer_cast<BatchDetailedTransactionInfoService>(reval));

  reval->addService("pool", std::static_pointer_cast<PoolShortInfoService>(reval));
  reval->addService("pool.detailed", std::static_pointer_cast<PoolDetailedInfoService>(reval));

  reval->addService("search", std::static_pointer_cast<SearchService>(reval));
  return reval;
}

const Limits &BlockExplorer::limits() {
  return m_limits;
}

BlockExplorer::BlockExplorer(std::shared_ptr<Explorer::IExplorer> explorer, Logging::ILogger &logger)
    : ServiceProviderCollection(logger),
      GenericServiceProviderCollection(logger),
      m_explorer{explorer},
      m_logger{logger, "Serivce.BlockExplorer"} {
  /* */
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const CryptoNote::Null &, Limits &response) {
  response = limits();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const CryptoNote::Null &, Explorer::CurrencyInfo &response) {
  response = m_explorer->queryCurrencyInfo().takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const CryptoNote::Null &, Explorer::NodeStatus &response) {
  response = m_explorer->queryNodeStatus().takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const std::optional<GetBlockRequest> &request,
                                         std::optional<Explorer::ShortBlockInfo> &response) {
  CHECK_EXPLORER
  if (!request.has_value()) {
    response = m_explorer->queryTopShortBlockInfo().takeOrThrow();
    return Error::Success;
  } else {
    const auto &rvalue = request.value();
    if (const auto height = std::get_if<Block::Height>(&rvalue)) {
      CHECK_ARG(*height)
      response = m_explorer->queryShortBlockInfo(*height).takeOrThrow();
      return Error::Success;
    } else if (const auto &hash = std::get_if<Block::Hash>(&rvalue)) {
      CHECK_ARG(*hash)
      response = m_explorer->queryShortBlockInfo(*hash).takeOrThrow();
      return Error::Success;
    } else {
      return Error::InvalidArgument;
    }
  }
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const std::optional<GetBlockRequest> &request,
                                         std::optional<Explorer::BlockInfo> &response) {
  CHECK_EXPLORER
  if (!request.has_value()) {
    response = m_explorer->queryTopBlockInfo().takeOrThrow();
    return Error::Success;
  } else {
    const auto &rvalue = request.value();
    if (const auto height = std::get_if<Block::Height>(&rvalue)) {
      CHECK_ARG(*height)
      response = m_explorer->queryBlockInfo(*height).takeOrThrow();
      return Error::Success;
    } else if (const auto &hash = std::get_if<Block::Hash>(&rvalue)) {
      CHECK_ARG(*hash)
      response = m_explorer->queryBlockInfo(*hash).takeOrThrow();
      return Error::Success;
    } else {
      return Error::InvalidArgument;
    }
  }
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const std::optional<GetBlockRequest> &request,
                                         std::optional<Explorer::DetailedBlockInfo> &response) {
  CHECK_EXPLORER
  if (!request.has_value()) {
    response = m_explorer->queryTopDetailedBlockInfo().takeOrThrow();
    return Error::Success;
  } else {
    const auto &rvalue = request.value();
    if (const auto height = std::get_if<Block::Height>(&rvalue)) {
      CHECK_ARG(*height)
      response = m_explorer->queryDetailedBlockInfo(*height).takeOrThrow();
      return Error::Success;
    } else if (const auto &hash = std::get_if<Block::Hash>(&rvalue)) {
      CHECK_ARG(*hash)
      response = m_explorer->queryDetailedBlockInfo(*hash).takeOrThrow();
      return Error::Success;
    } else {
      return Error::InvalidArgument;
    }
  }
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const GetBlocksRequest &request,
                                         GetBlocksResponse<Explorer::ShortBlockInfo> &response) {
  CHECK_EXPLORER
  if (const auto hashes = std::get_if<Block::HashVector>(&request)) {
    CHECK_ARG(*hashes, limits().short_blocks_limit)
    response = m_explorer->queryShortBlockInfo(*hashes).takeOrThrow();
    return Error::Success;
  } else if (const auto heights = std::get_if<Block::HeightVector>(&request)) {
    CHECK_ARG(*heights, limits().short_blocks_limit)
    response = m_explorer->queryShortBlockInfo(*heights).takeOrThrow();
    return Error::Success;
  } else if (const auto span = std::get_if<BlockHeightSpan>(&request)) {
    auto start = span->start.value_or(m_explorer->mainChainHeight().valueOrThrow());
    CHECK_ARG(start, span->count, limits().short_blocks_limit)
    response = m_explorer->queryShortBlockInfo(start, span->count).takeOrThrow();
    return Error::Success;
  } else if (const auto range = std::get_if<BlockHeightRange>(&request)) {
    CHECK_ARG(*range, limits().short_blocks_limit)
    response = m_explorer->queryShortBlockInfo(range->start, range->end).takeOrThrow();
    return Error::Success;
  } else {
    return Error::InvalidArgument;
  }
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const GetBlocksRequest &request,
                                         GetBlocksResponse<Explorer::BlockInfo> &response) {
  CHECK_EXPLORER
  if (const auto hashes = std::get_if<Block::HashVector>(&request)) {
    CHECK_ARG(*hashes, limits().blocks_limit)
    response = m_explorer->queryBlockInfo(*hashes).takeOrThrow();
    return Error::Success;
  } else if (const auto heights = std::get_if<Block::HeightVector>(&request)) {
    CHECK_ARG(*heights, limits().blocks_limit)
    response = m_explorer->queryBlockInfo(*heights).takeOrThrow();
    return Error::Success;
  } else if (const auto span = std::get_if<BlockHeightSpan>(&request)) {
    auto start = span->start.value_or(m_explorer->mainChainHeight().valueOrThrow());
    CHECK_ARG(start, span->count, limits().blocks_limit)
    response = m_explorer->queryBlockInfo(start, span->count).takeOrThrow();
    return Error::Success;
  } else if (const auto range = std::get_if<BlockHeightRange>(&request)) {
    CHECK_ARG(*range, limits().blocks_limit)
    response = m_explorer->queryBlockInfo(range->start, range->end).takeOrThrow();
    return Error::Success;
  } else {
    return Error::InvalidArgument;
  }
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const GetBlocksRequest &request,
                                         GetBlocksResponse<Explorer::DetailedBlockInfo> &response) {
  CHECK_EXPLORER
  if (const auto hashes = std::get_if<Block::HashVector>(&request)) {
    CHECK_ARG(*hashes, limits().detailed_blocks_limit)
    response = m_explorer->queryDetailedBlockInfo(*hashes).takeOrThrow();
    return Error::Success;
  } else if (const auto heights = std::get_if<Block::HeightVector>(&request)) {
    CHECK_ARG(*heights, limits().detailed_blocks_limit)
    response = m_explorer->queryDetailedBlockInfo(*heights).takeOrThrow();
    return Error::Success;
  } else if (const auto span = std::get_if<BlockHeightSpan>(&request)) {
    auto start = span->start.value_or(m_explorer->mainChainHeight().valueOrThrow());
    CHECK_ARG(start, span->count, limits().detailed_blocks_limit)
    response = m_explorer->queryDetailedBlockInfo(start, span->count).takeOrThrow();
    return Error::Success;
  } else if (const auto range = std::get_if<BlockHeightRange>(&request)) {
    CHECK_ARG(*range, limits().detailed_blocks_limit)
    response = m_explorer->queryDetailedBlockInfo(range->start, range->end).takeOrThrow();
    return Error::Success;
  } else {
    return Error::InvalidArgument;
  }
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const GetTransactionRequest &request,
                                         std::optional<Explorer::ShortTransactionInfo> &response) {
  CHECK_EXPLORER
  CHECK_ARG(request)
  response = m_explorer->queryShortTransactionInfo(request).takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const GetTransactionRequest &request,
                                         std::optional<Explorer::TransactionInfo> &response) {
  CHECK_EXPLORER
  CHECK_ARG(request)
  response = m_explorer->queryTransactionInfo(request).takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const GetTransactionRequest &request,
                                         std::optional<Explorer::DetailedTransactionInfo> &response) {
  CHECK_EXPLORER
  CHECK_ARG(request)
  response = m_explorer->queryDetailedTransactionInfo(request).takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const GetTransactionsRequest &request,
                                         GetTransactionsResponse<Explorer::ShortTransactionInfo> &response) {
  CHECK_EXPLORER
  CHECK_ARG(request, limits().short_transactions_limit)
  response = m_explorer->queryShortTransactionInfo(request).takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const GetTransactionsRequest &request,
                                         GetTransactionsResponse<Explorer::TransactionInfo> &response) {
  CHECK_EXPLORER
  CHECK_ARG(request, limits().transactions_limit)
  response = m_explorer->queryTransactionInfo(request).takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const GetTransactionsRequest &request,
                                         GetTransactionsResponse<Explorer::DetailedTransactionInfo> &response) {
  CHECK_EXPLORER
  CHECK_ARG(request, limits().detailed_transactions_limit)
  response = m_explorer->queryDetailedTransactionInfo(request).takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const CryptoNote::Null &,
                                         Explorer::ShortPoolInfo &response) {
  CHECK_EXPLORER
  response = m_explorer->queryShortPoolInfo().takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const CryptoNote::Null &, Explorer::PoolInfo &response) {
  CHECK_EXPLORER
  response = m_explorer->queryPoolInfo().takeOrThrow();
  return Error::Success;
}

Rpc::ServiceError BlockExplorer::process(std::string_view, const SearchRequest &request,
                                         std::optional<SearchResponse> &response) {
  CHECK_EXPLORER
  XI_RETURN_EC_IF_NOT(request.size() <= 64, Error::InvalidArgument);

  try {
    auto height = std::stoul(request);
    if (std::to_string(height).size() == request.size()) {
      if (height >= Block::Height::min().native() && height <= Block::Height::max().native()) {
        auto search =
            m_explorer->queryShortBlockInfo(Block::Height::fromNative(static_cast<Block::Height::value_type>(height)))
                .takeOrThrow();
        if (search.has_value()) {
          response.emplace(SearchResponse{*search});
          return Error::Success;
        } else {
          response = std::nullopt;
          return Error::Success;
        }
      }
    }
  } catch (std::invalid_argument &) {
    /* */
  } catch (std::out_of_range &) {
    /* */
  }

  if (auto hash = Block::Hash::fromString(request); hash.isError()) {
    return Error::InvalidArgument;
  } else {
    if (auto transaction = m_explorer->queryShortTransactionInfo(*hash).takeOrThrow(); transaction.has_value()) {
      response.emplace(SearchResponse{*transaction});
      return Error::Success;
    } else if (auto block = m_explorer->queryShortBlockInfo(*hash).takeOrThrow(); block.has_value()) {
      response.emplace(SearchResponse{*block});
      return Error::Success;
    } else {
      response = std::nullopt;
      return Error::Success;
    }
  }
}

bool BlockExplorer::check(const Block::Height &height) const {
  return !height.isNull();
}
bool BlockExplorer::check(const Block::Hash &hash) const {
  return !hash.isNull();
}
bool BlockExplorer::check(Block::ConstHeightSpan heights, uint32_t limit) const {
  XI_RETURN_EC_IF(heights.size() > limit, false);
  return !heights.empty() &&
         std::all_of(heights.begin(), heights.end(), [this](const auto &height) { return this->check(height); });
}
bool BlockExplorer::check(Block::ConstHashSpan hashes, uint32_t limit) const {
  XI_RETURN_EC_IF(hashes.size() > limit, false);
  return !hashes.empty() &&
         std::all_of(hashes.begin(), hashes.end(), [this](const auto &hash) { return this->check(hash); });
}
bool BlockExplorer::check(BlockHeightRange range, uint32_t limit) const {
  XI_RETURN_EC_IF_NOT(check(range.start), false);
  XI_RETURN_EC_IF_NOT(check(range.end), false);
  auto count = std::abs<int64_t>((range.start - range.end).native());
  XI_RETURN_EC_IF(count == 0, false);
  XI_RETURN_EC_IF(count > limit, false);
  return true;
}

bool BlockExplorer::check(const Block::Height &start, const Block::Offset &count, uint32_t limit) const {
  return check(BlockHeightRange{start, start + count}, limit);
}

}  // namespace BlockExplorer
}  // namespace Services
}  // namespace Blockchain
}  // namespace Xi

#undef CHECK_EXPLORER
#undef CHECK_ARG
