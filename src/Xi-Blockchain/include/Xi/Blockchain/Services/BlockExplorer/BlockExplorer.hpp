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

#include <functional>
#include <cinttypes>

#include <Xi/Rpc/ServiceProviderCollection.hpp>
#include <Xi/Rpc/TypedServiceProvider.hpp>
#include <Xi/Blockchain/Explorer/IExplorer.hpp>
#include <Logging/LoggerRef.h>

#include "Xi/Blockchain/Services/BlockExplorer/Commands.hpp"

namespace Xi {
namespace Blockchain {
namespace Services {
namespace BlockExplorer {

// clang-format off
using InfoLimitsService = Rpc::TypedServiceProvider<CryptoNote::Null, Limits>;
using InfoCurrencyService = Rpc::TypedServiceProvider<CryptoNote::Null, Explorer::CurrencyInfo>;

using ShortBlockInfoService = Rpc::TypedServiceProvider<std::optional<GetBlockRequest>, std::optional<Explorer::ShortBlockInfo>>;
using BlockInfoService = Rpc::TypedServiceProvider<std::optional<GetBlockRequest>, std::optional<Explorer::BlockInfo>>;
using DetailedBlockInfoService = Rpc::TypedServiceProvider<std::optional<GetBlockRequest>, std::optional<Explorer::DetailedBlockInfo>>;
using BatchShortBlockInfoService = Rpc::TypedServiceProvider<GetBlocksRequest, GetBlocksResponse<Explorer::ShortBlockInfo>>;
using BatchBlockInfoService = Rpc::TypedServiceProvider<GetBlocksRequest, GetBlocksResponse<Explorer::BlockInfo>>;
using BatchDetailedBlockInfoService = Rpc::TypedServiceProvider<GetBlocksRequest, GetBlocksResponse<Explorer::DetailedBlockInfo>>;

using ShortTransactionInfoService = Rpc::TypedServiceProvider<GetTransactionRequest, std::optional<Explorer::ShortTransactionInfo>>;
using TransactionInfoService = Rpc::TypedServiceProvider<GetTransactionRequest, std::optional<Explorer::TransactionInfo>>;
using DetailedTransactionInfoService = Rpc::TypedServiceProvider<GetTransactionRequest, std::optional<Explorer::DetailedTransactionInfo>>;
using BatchShortTransactionInfoService = Rpc::TypedServiceProvider<GetTransactionsRequest, GetTransactionsResponse<Explorer::ShortTransactionInfo>>;
using BatchTransactionInfoService = Rpc::TypedServiceProvider<GetTransactionsRequest, GetTransactionsResponse<Explorer::TransactionInfo>>;
using BatchDetailedTransactionInfoService = Rpc::TypedServiceProvider<GetTransactionsRequest, GetTransactionsResponse<Explorer::DetailedTransactionInfo>>;

using PoolShortInfoService = Rpc::TypedServiceProvider<CryptoNote::Null, Explorer::ShortPoolInfo>;
using PoolDetailedInfoService = Rpc::TypedServiceProvider<CryptoNote::Null, Explorer::PoolInfo>;

using SearchService = Rpc::TypedServiceProvider<SearchRequest, std::optional<SearchResponse>>;

class BlockExplorer
    : public Rpc::ServiceProviderCollection,
      public Rpc::GenericServiceProviderCollection<
        InfoLimitsService,
        InfoCurrencyService,

        ShortBlockInfoService,
        BlockInfoService,
        DetailedBlockInfoService,
        BatchShortBlockInfoService,
        BatchBlockInfoService,
        BatchDetailedBlockInfoService,

        ShortTransactionInfoService,
        TransactionInfoService,
        DetailedTransactionInfoService,
        BatchShortTransactionInfoService,
        BatchTransactionInfoService,
        BatchDetailedTransactionInfoService,

        PoolShortInfoService,
        PoolDetailedInfoService,

        SearchService
    >,
        public std::enable_shared_from_this<BlockExplorer>
// clang-format on
{
 public:
  static std::shared_ptr<BlockExplorer> create(std::shared_ptr<Explorer::IExplorer> explorer, Logging::ILogger& logger);

  const Limits& limits();

 private:
  BlockExplorer(std::shared_ptr<Explorer::IExplorer> explorer, Logging::ILogger& logger);

 public:
  // clang-format off
  Rpc::ServiceError process(std::string_view command, const CryptoNote::Null&, Limits& response) override;
  Rpc::ServiceError process(std::string_view command, const CryptoNote::Null&, Explorer::CurrencyInfo& response) override;

  Rpc::ServiceError process(std::string_view command, const std::optional<GetBlockRequest>& request, std::optional<Explorer::ShortBlockInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const std::optional<GetBlockRequest>& request, std::optional<Explorer::BlockInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const std::optional<GetBlockRequest>& request, std::optional<Explorer::DetailedBlockInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const GetBlocksRequest& request, GetBlocksResponse<Explorer::ShortBlockInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const GetBlocksRequest& request, GetBlocksResponse<Explorer::BlockInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const GetBlocksRequest& request, GetBlocksResponse<Explorer::DetailedBlockInfo>& response) override;

  Rpc::ServiceError process(std::string_view command, const GetTransactionRequest& request, std::optional<Explorer::ShortTransactionInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const GetTransactionRequest& request, std::optional<Explorer::TransactionInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const GetTransactionRequest& request, std::optional<Explorer::DetailedTransactionInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const GetTransactionsRequest& request, GetTransactionsResponse<Explorer::ShortTransactionInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const GetTransactionsRequest& request, GetTransactionsResponse<Explorer::TransactionInfo>& response) override;
  Rpc::ServiceError process(std::string_view command, const GetTransactionsRequest& request, GetTransactionsResponse<Explorer::DetailedTransactionInfo>& response) override;

  Rpc::ServiceError process(std::string_view command, const CryptoNote::Null& request, Explorer::ShortPoolInfo& response) override;
  Rpc::ServiceError process(std::string_view command, const CryptoNote::Null& request, Explorer::PoolInfo& response) override;

  Rpc::ServiceError process(std::string_view command, const SearchRequest& request, std::optional<SearchResponse>& response) override;
  // clang-format on

  [[nodiscard]] bool check(const Block::Height& height) const;
  [[nodiscard]] bool check(const Block::Hash& hash) const;
  [[nodiscard]] bool check(Block::ConstHeightSpan heights, uint32_t limit) const;
  [[nodiscard]] bool check(Block::ConstHashSpan hashes, uint32_t limit) const;
  [[nodiscard]] bool check(BlockHeightRange range, uint32_t limit) const;
  [[nodiscard]] bool check(const Block::Height& start, const Block::Offset& count, uint32_t limit) const;

 private:
  std::shared_ptr<Explorer::IExplorer> m_explorer;
  Logging::LoggerRef m_logger;
  Limits m_limits{};
};

}  // namespace BlockExplorer
}  // namespace Services
}  // namespace Blockchain
}  // namespace Xi
