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

#include "Xi/Blockchain/Explorer/CoreExplorer.hpp"

#include <numeric>
#include <iterator>
#include <cassert>
#include <type_traits>
#include <algorithm>

#include <Xi/Exceptions.hpp>
#include <CryptoNoteCore/CryptoNoteFormatUtils.h>
#include <CryptoNoteCore/Transactions/ITransactionPool.h>

namespace Xi {
namespace Blockchain {
namespace Explorer {

CoreExplorer::CoreExplorer(CryptoNote::ICore &core)
    : m_core{core}, m_mainChainHeight{0}, m_topBlock{nullptr}, m_poolInfo{nullptr} {
  m_core.addObserver(this);
  m_core.transactionPool().addObserver(this);
  m_mainChainHeight.store(m_core.getTopBlockIndex());
}

CoreExplorer::~CoreExplorer() {
  m_core.removeObserver(this);
  m_core.transactionPool().removeObserver(this);
}

Result<CurrencyInfo> CoreExplorer::queryCurrencyInfo() {
  XI_ERROR_TRY();
  const auto &currency = m_core.currency();
  CurrencyInfo reval{};
  reval.name = currency.coin().name();
  reval.ticker = currency.coin().ticker();
  reval.address_prefix = currency.coin().prefix().text();
  reval.total_supply = currency.coin().totalSupply();
  ;
  reval.premine = currency.coin().premine();
  reval.emission_speed = currency.emissionSpeedFactor();

  reval.homepage = currency.general().homepage();
  reval.description = currency.general().description();
  reval.copyright = currency.general().copyright();

  return success(std::move(reval));
  XI_ERROR_CATCH();
}

Result<Block::Height> CoreExplorer::mainChainHeight() {
  return success(Block::Height::fromIndex(m_mainChainHeight.load(std::memory_order_consume)));
}

IExplorer::SingleResult<ShortBlockInfo> CoreExplorer::queryTopShortBlockInfo() {
  if (auto top = topBlockInfo(); !top.isError()) {
    if (top->get() == nullptr) {
      return emplaceSuccess<std::optional<ShortBlockInfo>>(std::nullopt);
    } else {
      return success(std::optional<ShortBlockInfo>{std::in_place, top.value()->info});
    }
  } else {
    return top.error();
  }
}

namespace {

template <typename _InfoT>
IExplorer::VectorResult<_InfoT> toVectorResult(Result<std::vector<std::shared_ptr<_InfoT>>> &&result) {
  using vector_type = std::vector<std::optional<_InfoT>>;
  if (!result.isError()) {
    vector_type reval{};
    reval.reserve(result->size());
    std::transform(begin(*result), end(*result), std::back_inserter(reval),
                   [](auto &i) { return i ? std::make_optional<_InfoT>(std::move(*i)) : std::nullopt; });
    return success(std::move(reval));
  } else {
    return result.error();
  }
}

}  // namespace

IExplorer::VectorResult<ShortBlockInfo> CoreExplorer::queryShortBlockInfo(Block::ConstHashSpan hashes) {
  XI_ERROR_TRY();
  return toVectorResult(doQueryShortBlockInfo(hashes));
  XI_ERROR_CATCH();
}

IExplorer::VectorResult<ShortBlockInfo> CoreExplorer::queryShortBlockInfo(Block::ConstHeightSpan heights) {
  XI_ERROR_TRY();
  return toVectorResult(doQueryShortBlockInfo(heights));
  XI_ERROR_CATCH();
}

IExplorer::SingleResult<BlockInfo> CoreExplorer::queryTopBlockInfo() {
  if (auto top = topBlockInfo(); !top.isError()) {
    if (top->get() == nullptr) {
      return emplaceSuccess<std::optional<BlockInfo>>(std::nullopt);
    } else {
      return success(std::make_optional<BlockInfo>(top.value()->info));
    }
  } else {
    return top.error();
  }
}

IExplorer::VectorResult<BlockInfo> CoreExplorer::queryBlockInfo(Block::ConstHashSpan hashes) {
  XI_ERROR_TRY();
  return toVectorResult(doQueryBlockInfo(hashes));
  XI_ERROR_CATCH();
}

IExplorer::VectorResult<BlockInfo> CoreExplorer::queryBlockInfo(Block::ConstHeightSpan heights) {
  XI_ERROR_TRY();
  return toVectorResult(doQueryBlockInfo(heights));
  XI_ERROR_CATCH();
}

IExplorer::SingleResult<DetailedBlockInfo> CoreExplorer::queryTopDetailedBlockInfo() {
  if (auto top = topBlockInfo(); !top.isError()) {
    if (top->get() == nullptr) {
      return emplaceSuccess<std::optional<DetailedBlockInfo>>(std::nullopt);
    } else {
      return success(std::make_optional<DetailedBlockInfo>(*top.value()));
    }
  } else {
    return top.error();
  }
}

IExplorer::VectorResult<DetailedBlockInfo> CoreExplorer::queryDetailedBlockInfo(Block::ConstHashSpan hashes) {
  XI_ERROR_TRY();
  return toVectorResult(doQueryDetailedBlockInfo(hashes));
  XI_ERROR_CATCH();
}

IExplorer::VectorResult<DetailedBlockInfo> CoreExplorer::queryDetailedBlockInfo(Block::ConstHeightSpan heights) {
  XI_ERROR_TRY();
  return toVectorResult(doQueryDetailedBlockInfo(heights));
  XI_ERROR_CATCH();
}

IExplorer::VectorResult<ShortTransactionInfo> CoreExplorer::queryShortTransactionInfo(ConstTransactionHashSpan hashes) {
  XI_ERROR_TRY();
  return toVectorResult(doQueryShortTransactionInfo(hashes));
  XI_ERROR_CATCH();
}

IExplorer::VectorResult<TransactionInfo> CoreExplorer::queryTransactionInfo(ConstTransactionHashSpan hashes) {
  XI_ERROR_TRY();
  return toVectorResult(doQueryTransactionInfo(hashes, true));
  XI_ERROR_CATCH();
}

IExplorer::VectorResult<DetailedTransactionInfo> CoreExplorer::queryDetailedTransactionInfo(
    ConstTransactionHashSpan hashes) {
  XI_ERROR_TRY();
  return toVectorResult(doQueryDetailedTransactionInfo(hashes));
  XI_ERROR_CATCH();
}

Result<ShortPoolInfo> CoreExplorer::queryShortPoolInfo() {
  XI_ERROR_TRY();
  if (const auto result = poolInfo(); !result.isError()) {
    ShortPoolInfo reval{static_cast<ShortPoolInfo &>(**result)};
    return success(std::move(reval));
  } else {
    return result.error();
  }
  XI_ERROR_CATCH();
}

Result<PoolInfo> CoreExplorer::queryPoolInfo() {
  XI_ERROR_TRY();
  return success(*poolInfo().valueOrThrow());
  XI_ERROR_CATCH();
}

void CoreExplorer::blockAdded(uint32_t index, const Block::Hash &hash) {
  XI_UNUSED(hash);
  m_mainChainHeight.store(index, std::memory_order_release);
  XI_CONCURRENT_LOCK_WRITE(m_topBlockGuard);
  m_topBlock.reset();
}

void CoreExplorer::mainChainSwitched(const CryptoNote::IBlockchainCache &previous,
                                     const CryptoNote::IBlockchainCache &current, uint32_t splitIndex) {
  XI_UNUSED(previous, current, splitIndex);
  m_mainChainHeight.store(current.getTopBlockIndex(), std::memory_order_release);
  XI_CONCURRENT_LOCK_WRITE(m_topBlockGuard);
  m_topBlock.reset();
}

void CoreExplorer::transactionDeletedFromPool(const TransactionHash &hash,
                                              CryptoNote::ITransactionPoolObserver::DeletionReason reason) {
  XI_UNUSED(hash, reason);
  XI_CONCURRENT_LOCK_WRITE(m_poolInfoGuard);
  m_poolInfo.reset();
}

void CoreExplorer::transactionAddedToPool(const TransactionHash &hash,
                                          CryptoNote::ITransactionPoolObserver::AdditionReason reason) {
  XI_UNUSED(hash, reason);
  XI_CONCURRENT_LOCK_WRITE(m_poolInfoGuard);
  m_poolInfo.reset();
}

ShortTransactionInputInfo CoreExplorer::fromCore(const CryptoNote::TransactionInput &input) const {
  if (auto baseInput = std::get_if<CryptoNote::BaseInput>(&input)) {
    ShortTransactionBaseInputInfo reval{};
    reval.height = baseInput->height;
    return reval;
  } else if (auto keyInput = std::get_if<CryptoNote::KeyInput>(&input)) {
    ShortTransactionKeyInputInfo reval{};
    reval.amount = keyInput->amount;
    reval.ring_size = keyInput->outputIndices.size();
    return reval;
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

ShortTransactionOutputInfo CoreExplorer::fromCore(const CryptoNote::TransactionOutput &output) const {
  if (const auto amountOutput = std::get_if<Transaction::AmountOutput>(std::addressof(output))) {
    if (auto keyOutput = std::get_if<CryptoNote::KeyOutput>(&amountOutput->target)) {
      XI_UNUSED(keyOutput);

      ShortTransactionKeyOutputInfo reval{};
      reval.amount = amountOutput->amount;
      return reval;
    } else {
      exceptional<InvalidVariantTypeError>();
    }
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

TransactionInfo CoreExplorer::fromCore(const CryptoNote::CachedTransaction &transaction, bool isStaticReward,
                                       TransactionContainer container) const {
  TransactionInfo reval{};

  if (!transaction.isCoinbase()) {
    reval.fee = transaction.getTransactionFee();
    if (reval.fee == 0) {
      reval.type = TransactionType::Fusion;
    } else {
      reval.type = TransactionType::Transfer;
    }
  } else {
    reval.fee = 0;
    reval.type = isStaticReward ? TransactionType::StaticReweard : TransactionType::BlockReward;
  }
  reval.container = container;
  reval.hash = transaction.getTransactionHash();
  reval.sum_input = transaction.getInputAmount();
  reval.sum_output = transaction.getOutputAmount();
  reval.unlock_time = transaction.getTransaction().unlockTime;
  reval.blob_size = transaction.getBlobSize();

  reval.extra.public_key = transaction.getPublicKey();
  auto paymentId = transaction.getPaymentId();
  if (paymentId.has_value()) {
    reval.extra.payment_id = *paymentId;
  } else {
    reval.extra.payment_id = std::nullopt;
  }

  reval.inputs.reserve(transaction.getTransaction().inputs.size());
  for (const auto &input : transaction.getTransaction().inputs) {
    reval.inputs.emplace_back(fromCore(input));
  }
  reval.outputs.reserve(transaction.getTransaction().outputs.size());
  for (const auto &output : transaction.getTransaction().outputs) {
    reval.outputs.emplace_back(fromCore(output));
  }

  reval.block = std::nullopt;

  return reval;
}

// TODO: optimize batch queries of key output references
DetailedTransactionInfo CoreExplorer::fromCore(const CryptoNote::IBlockchainCache *segment,
                                               const CryptoNote::CachedTransactionInfo *info,
                                               const CryptoNote::CachedTransaction &transaction, bool isStaticReward,
                                               TransactionContainer container) const {
  DetailedTransactionInfo reval{};
  reval.info = fromCore(transaction, isStaticReward, container);

  const auto &inputs = transaction.getTransaction().inputs;
  reval.inputs.reserve(inputs.size());
  for (const auto &input : inputs) {
    if (auto baseInput = std::get_if<CryptoNote::BaseInput>(&input)) {
      reval.inputs.emplace_back(std::in_place_type<DetailedTransactionBaseInput>);
      auto &iBaseInput = std::get<DetailedTransactionBaseInput>(reval.inputs.back());
      iBaseInput.input = *baseInput;
    } else if (auto keyInput = std::get_if<CryptoNote::KeyInput>(&input)) {
      reval.inputs.emplace_back(std::in_place_type<DetailedTransactionKeyInput>);
      auto &iKeyInput = std::get<DetailedTransactionKeyInput>(reval.inputs.back());
      iKeyInput.input = *keyInput;
      iKeyInput.ring_size = keyInput->outputIndices.size();
      iKeyInput.references.reserve(iKeyInput.ring_size);
      Blockchain::Transaction::GlobalIndexVector globalIndices{};
      {
        [[maybe_unused]] const auto ec = Blockchain::Transaction::deltaDecodeGlobalIndices(
            keyInput->outputIndices, globalIndices, transaction.getTransaction().globalIndexOffset.value_or(0));
        assert(ec);
      }
      std::vector<std::pair<TransactionHash, size_t>> references{};
      references.reserve(globalIndices.size());
      segment->extractKeyOtputReferences(keyInput->amount,
                                         Common::ArrayView{globalIndices.data(), globalIndices.size()}, references);
      iKeyInput.references.reserve(references.size());
      for (auto &iRef : references) {
        DetailedTransactionKeyInputReference convertedRef{};
        convertedRef.transaction = std::move(iRef.first);
        convertedRef.index = iRef.second;
        iKeyInput.references.emplace_back(std::move(convertedRef));
      }
    } else {
      exceptional<InvalidVariantTypeError>();
    }
  }

  const auto &outputs = transaction.getTransaction().outputs;
  reval.outputs.reserve(outputs.size());
  size_t i = 0;
  for (const auto &output : outputs) {
    if (const auto amountOutput = std::get_if<Transaction::AmountOutput>(std::addressof(output))) {
      if (auto keyOutput = std::get_if<CryptoNote::KeyOutput>(std::addressof(amountOutput->target))) {
        reval.outputs.emplace_back(std::in_place_type<DetailedTransactionKeyOutput>);
        auto &iOutput = std::get<DetailedTransactionKeyOutput>(reval.outputs.back());
        iOutput.output = output;
        iOutput.global_index = std::nullopt;
        if (info != nullptr) {
          if (info->globalIndexes.size() > i) {
            iOutput.global_index = info->globalIndexes[i];
          }
        }
      } else {
        exceptional<InvalidVariantTypeError>();
      }
    } else {
      exceptional<InvalidVariantTypeError>();
    }
    i += 1;
  }

  return reval;
}

ShortBlockInfo CoreExplorer::fromCore(const CryptoNote::CachedBlockInfo &info, const Block::Height &height,
                                      bool isAlternative) const {
  ShortBlockInfo reval{};
  reval.hash = info.blockHash;
  reval.height = height;
  reval.timestamp = info.timestamp;
  reval.chain = isAlternative ? BlockSource::AlternativeChain : BlockSource::MainChain;
  reval.blob_size = info.blobSize;
  reval.version = info.version;
  reval.features = info.features;
  reval.cumulative_difficulty = info.cumulativeDifficulty;
  reval.cumulative_supply = info.alreadyGeneratedCoins;
  reval.cumulative_transactions_count = info.alreadyGeneratedTransactions;
  return reval;
}

BlockInfo CoreExplorer::fromCore(const CryptoNote::CachedRawBlock &raw, const CryptoNote::CachedBlockInfo &info,
                                 const Block::Height &height, bool isAlternative) const {
  BlockInfo reval{};
  static_cast<ShortBlockInfo &>(reval) = fromCore(info, height, isAlternative);

  const auto transactionContainer =
      isAlternative ? TransactionContainer::AlternativeChain : TransactionContainer::MainChain;

  reval.nonce = raw.block().getBlock().nonce;
  reval.previous_hash = raw.block().getBlock().previousBlockHash;
  reval.transactions_count = raw.transactionCount();
  reval.miner_reward = fromCore(raw.block().coinbase(), false, transactionContainer);

  const auto &currency = m_core.currency();
  if (currency.isStaticRewardEnabledForBlockVersion(reval.version)) {
    reval.static_reward = fromCore(CryptoNote::cache(*currency.constructStaticRewardTx(raw.block()).takeOrThrow()),
                                   true, transactionContainer);
  } else {
    reval.static_reward = std::nullopt;
  }

  reval.transfers.reserve(raw.transferCount());
  for (size_t i = 0; i < raw.transferCount(); ++i) {
    reval.transfers.emplace_back(fromCore(raw[i], false, transactionContainer));
  }
  reval.sum_fees = std::accumulate(begin(reval.transfers), end(reval.transfers), 0ULL,
                                   [](const auto acc, const auto &iTransfer) { return acc + iTransfer.fee; });

  return reval;
}

DetailedBlockInfo CoreExplorer::fromCore(const CryptoNote::IBlockchainCache *segment,
                                         const CryptoNote::CachedRawBlock &raw, const CryptoNote::CachedBlockInfo &info,
                                         const Block::Height &height, bool isAlternative) const {
  DetailedBlockInfo reval{};
  reval.info = fromCore(raw, info, height, isAlternative);

  CryptoNote::CachedTransactionVector transactions{};
  transactions.reserve(raw.transactionCount());

  transactions.emplace_back(raw.block().coinbase());
  if (raw.block().hasStaticReward()) {
    auto staticReward = m_core.currency().constructStaticRewardTx(raw.block()).takeOrThrow();
    exceptional_if_not<RuntimeError>(staticReward.has_value());
    transactions.emplace_back(CryptoNote::cache(std::move(*staticReward)));
  }
  for (size_t i = 0; i < raw.transferCount(); ++i) {
    transactions.emplace_back(CryptoNote::CachedTransaction::fromBinaryArray(raw.raw().transactions[i]).takeOrThrow());
  }

  TransactionHashVector transactionHashes{};
  transactionHashes.reserve(transactions.size());
  std::transform(begin(transactions), end(transactions), std::back_inserter(transactionHashes),
                 [](const auto &iTx) { return iTx.getTransactionHash(); });
  auto txInfos = segment->getTransactionInfos(transactionHashes);

  reval.transactions.reserve(txInfos.size());
  for (size_t i = 0; i < txInfos.size() && i < transactions.size(); ++i) {
    const bool isStaticReward = raw.block().hasStaticReward() && i == 1;
    const auto container = isAlternative ? TransactionContainer::AlternativeChain : TransactionContainer::MainChain;
    reval.transactions.emplace_back(fromCore(segment, &txInfos[i], transactions[i], isStaticReward, container));
  }

  return reval;
}

Result<std::shared_ptr<DetailedBlockInfo>> CoreExplorer::topBlockInfo() {
  XI_ERROR_TRY();
  XI_CONCURRENT_LOCK_PREPARE_WRITE(m_topBlockGuard);
  if (!m_topBlock) {
    XI_CONCURRENT_LOCK_ACQUIRE_WRITE(m_topBlockGuard);
    if (!m_topBlock) {
      auto topBlockHash = m_core.getTopBlockHash();
      auto info = doQueryDetailedBlockInfo(makeSpan(topBlockHash)).takeOrThrow();
      XI_RETURN_EC_IF(info.empty(), makeError(ExplorerError::NotFound));
      m_topBlock = info.front();
      return success(m_topBlock);
    }
  }
  return success(std::shared_ptr<DetailedBlockInfo>{m_topBlock});
  XI_ERROR_CATCH();
}

Result<std::shared_ptr<PoolInfo>> CoreExplorer::poolInfo() {
  XI_ERROR_TRY();
  XI_CONCURRENT_LOCK_PREPARE_WRITE(m_poolInfoGuard);
  if (!m_poolInfo) {
    XI_CONCURRENT_LOCK_ACQUIRE_WRITE(m_poolInfoGuard);
    if (!m_poolInfo) {
      m_poolInfo = std::make_shared<PoolInfo>();
      const auto &pool = m_core.transactionPool();
      m_poolInfo->state_hash = pool.stateHash();
      m_poolInfo->count = pool.size();
      m_poolInfo->cumulative_fees = pool.cumulativeFees();
      m_poolInfo->cumulative_size = pool.cumulativeSize();

      auto transactions = pool.getPoolTransactions();
      m_poolInfo->transactions.reserve(transactions.size());
      std::transform(begin(transactions), end(transactions), std::back_inserter(m_poolInfo->transactions),
                     [this](const auto &tx) { return fromCore(tx, false, TransactionContainer::Pool); });
    }
  }
  return success(std::shared_ptr<PoolInfo>{m_poolInfo});
  XI_ERROR_CATCH();
}

Result<std::vector<std::shared_ptr<ShortTransactionInfo>>> CoreExplorer::doQueryShortTransactionInfo(
    ConstTransactionHashSpan hashes) {
  XI_ERROR_TRY();
  if (auto inner = doQueryTransactionInfo(hashes, true); !inner.isError()) {
    std::vector<std::shared_ptr<ShortTransactionInfo>> reval{};
    reval.reserve(inner->size());
    std::transform(begin(*inner), end(*inner), std::back_inserter(reval), [](const auto &i) {
      if (!i) {
        return std::shared_ptr<ShortTransactionInfo>{nullptr};
      } else {
        return std::make_shared<ShortTransactionInfo>(std::move(static_cast<ShortTransactionInfo &>(*i)));
      }
    });
    return emplaceSuccess<std::vector<std::shared_ptr<ShortTransactionInfo>>>(std::move(reval));
  } else {
    return inner.error();
  }
  XI_ERROR_CATCH();
}

Result<std::vector<std::shared_ptr<TransactionInfo>>> CoreExplorer::doQueryTransactionInfo(
    ConstTransactionHashSpan hashes, bool skipBlockReferences) {
  XI_ERROR_TRY();
  XI_UNUSED_REVAL(m_core.lock());

  std::map<TransactionHash, std::shared_ptr<TransactionInfo>> result{};
  std::map<Block::Height, std::set<TransactionHash>> requiredBlockInfos{};

  auto segments = m_core.queryTransactionSegments(hashes);
  for (auto &segment : segments) {
    auto transactions = segment.segment->getTransactions(segment.content);
    auto infos = segment.segment->getTransactionInfos(segment.content);

    const auto container =
        (segment.source == CryptoNote::BlockSource::MainChain ? TransactionContainer::MainChain
                                                              : TransactionContainer::AlternativeChain);
    for (size_t i = 0; i < transactions.size() && i < infos.size() && i < segment.content.size(); ++i) {
      const bool isStaticReward = infos[i].isDeterministicallyGenerated && infos[i].transactionIndex == 1;
      auto info = fromCore(transactions[i], isStaticReward, container);
      if (!skipBlockReferences) {
        requiredBlockInfos[Block::Height::fromIndex(infos[i].blockIndex)].insert(info.hash);
      }
      result[segment.content[i]] = std::make_shared<TransactionInfo>(std::move(info));
    }
  }

  {
    const auto &pool = m_core.transactionPool();
    [[maybe_unused]] auto poolLock = pool.acquireExclusiveAccess();
    for (const auto &hash : hashes) {
      auto search = result.find(hash);
      if (search != result.end() && search->second->container == TransactionContainer::MainChain) {
        continue;
      }

      auto poolSearch = pool.queryTransaction(hash);
      if (poolSearch) {
        auto info = fromCore(poolSearch->transaction(), false, TransactionContainer::Pool);
        result[hash] = std::make_shared<TransactionInfo>(std::move(info));
      }
    }
  }

  if (!skipBlockReferences) {
    Block::HeightVector heights{};
    heights.reserve(requiredBlockInfos.size());
    std::transform(begin(requiredBlockInfos), end(requiredBlockInfos), std::back_inserter(heights),
                   [](const auto &i) { return i.first; });

    if (auto blockResult = doQueryShortBlockInfo(heights); !blockResult.isError()) {
      for (const auto &iBlock : *blockResult) {
        if (iBlock) {
          auto &transactionsToUpdate = requiredBlockInfos[iBlock->height];
          for (auto &toUpdate : transactionsToUpdate) {
            result[toUpdate]->block = *iBlock;
          }
        }
      }
    } else {
      return blockResult.error();
    }
  }

  std::vector<std::shared_ptr<TransactionInfo>> reval{};
  reval.reserve(result.size());
  for (const auto &hash : hashes) {
    reval.emplace_back(result[hash]);
  }
  return emplaceSuccess<std::vector<std::shared_ptr<TransactionInfo>>>(std::move(reval));

  XI_ERROR_CATCH();
}

Result<std::vector<std::shared_ptr<DetailedTransactionInfo>>> CoreExplorer::doQueryDetailedTransactionInfo(
    ConstTransactionHashSpan hashes) {
  XI_ERROR_TRY();
  XI_UNUSED_REVAL(m_core.lock());

  std::map<TransactionHash, std::shared_ptr<DetailedTransactionInfo>> result{};
  std::map<Block::Height, std::set<TransactionHash>> requiredBlockInfos{};

  auto segments = m_core.queryTransactionSegments(hashes);
  for (auto &segment : segments) {
    auto transactions = segment.segment->getTransactions(segment.content);
    auto infos = segment.segment->getTransactionInfos(segment.content);

    const auto container =
        (segment.source == CryptoNote::BlockSource::MainChain ? TransactionContainer::MainChain
                                                              : TransactionContainer::AlternativeChain);
    for (size_t i = 0; i < transactions.size() && i < infos.size() && i < segment.content.size(); ++i) {
      const bool isStaticReward = infos[i].isDeterministicallyGenerated && infos[i].transactionIndex == 1;
      auto info = fromCore(segment.segment.get(), &infos[i], transactions[i], isStaticReward, container);
      requiredBlockInfos[Block::Height::fromIndex(infos[i].blockIndex)].insert(info.info.hash);
      result[segment.content[i]] = std::make_shared<DetailedTransactionInfo>(std::move(info));
    }
  }

  {
    const auto &pool = m_core.transactionPool();
    [[maybe_unused]] auto poolLock = pool.acquireExclusiveAccess();
    for (const auto &hash : hashes) {
      auto search = result.find(hash);
      if (search != result.end() && search->second->info.container == TransactionContainer::MainChain) {
        continue;
      }

      auto poolSearch = pool.queryTransaction(hash);
      if (poolSearch) {
        DetailedTransactionInfo info =
            fromCore(m_core.mainChain(), nullptr, poolSearch->transaction(), false, TransactionContainer::Pool);
        result[hash] = std::make_shared<DetailedTransactionInfo>(std::move(info));
      }
    }
  }

  Block::HeightVector heights{};
  heights.reserve(requiredBlockInfos.size());
  std::transform(begin(requiredBlockInfos), end(requiredBlockInfos), std::back_inserter(heights),
                 [](const auto &i) { return i.first; });

  if (auto blockResult = doQueryShortBlockInfo(heights); !blockResult.isError()) {
    for (const auto &iBlock : *blockResult) {
      if (iBlock) {
        auto &transactionsToUpdate = requiredBlockInfos[iBlock->height];
        for (auto &toUpdate : transactionsToUpdate) {
          result[toUpdate]->info.block = *iBlock;
        }
      }
    }
  } else {
    return blockResult.error();
  }

  std::vector<std::shared_ptr<DetailedTransactionInfo>> reval{};
  reval.reserve(result.size());
  for (const auto &hash : hashes) {
    reval.emplace_back(result[hash]);
  }
  return emplaceSuccess<std::vector<std::shared_ptr<DetailedTransactionInfo>>>(std::move(reval));

  XI_ERROR_CATCH();
}

template <typename _InfoT>
Result<std::vector<std::shared_ptr<_InfoT>>> CoreExplorer::doQueryBlockInfoByHashes(Block::ConstHashSpan hashes) {
  XI_ERROR_TRY();
  XI_UNUSED_REVAL(m_core.lock());

  std::map<Block::Hash, std::shared_ptr<_InfoT>> reval{};

  auto hashSegments = m_core.queryBlockSegments(hashes);
  for (const auto &hashSegment : hashSegments) {
    auto heights = hashSegment.segment->getBlockHeights(hashSegment.content);
    std::vector<std::shared_ptr<_InfoT>> infos;
    if constexpr (std::is_same_v<_InfoT, ShortBlockInfo>) {
      infos = doQueryShortBlockInfo(heights).takeOrThrow();
    } else if constexpr (std::is_same_v<_InfoT, BlockInfo>) {
      infos = doQueryBlockInfo(heights).takeOrThrow();
    } else if constexpr (std::is_same_v<_InfoT, DetailedBlockInfo>) {
      infos = doQueryDetailedBlockInfo(heights).takeOrThrow();
    }

    for (size_t i = 0; i < hashSegment.content.size() && i < infos.size(); ++i) {
      reval[hashSegment.content[i]] = infos[i];
    }
  }

  std::vector<std::shared_ptr<_InfoT>> result{};
  result.resize(hashes.size(), nullptr);
  size_t i = 0;
  for (const auto &hash : hashes) {
    auto search = reval.find(hash);
    if (search != reval.end()) {
      result[i] = search->second;
    }
    i += 1;
  }

  return emplaceSuccess<std::vector<std::shared_ptr<_InfoT>>>(std::move(result));

  XI_ERROR_CATCH();
}

Result<std::vector<std::shared_ptr<ShortBlockInfo>>> CoreExplorer::doQueryShortBlockInfo(Block::ConstHashSpan hashes) {
  return doQueryBlockInfoByHashes<ShortBlockInfo>(hashes);
}

Result<std::vector<std::shared_ptr<BlockInfo>>> CoreExplorer::doQueryBlockInfo(Block::ConstHashSpan hashes) {
  return doQueryBlockInfoByHashes<BlockInfo>(hashes);
}

Result<std::vector<std::shared_ptr<DetailedBlockInfo>>> CoreExplorer::doQueryDetailedBlockInfo(
    Block::ConstHashSpan hashes) {
  return doQueryBlockInfoByHashes<DetailedBlockInfo>(hashes);
}

Result<std::vector<std::shared_ptr<ShortBlockInfo>>> CoreExplorer::doQueryShortBlockInfo(
    const Block::ConstHeightSpan &heights) {
  XI_ERROR_TRY();
  XI_UNUSED_REVAL(m_core.lock());

  std::map<Block::Height, std::shared_ptr<ShortBlockInfo>> reval{};

  auto heightSegments = m_core.queryBlockSegments(heights);
  for (const auto &heightSegment : heightSegments) {
    auto infoBlocks = heightSegment.segment->getBlockInfos(heightSegment.content);

    for (size_t i = 0; i < infoBlocks.size() && i < heightSegment.content.size(); ++i) {
      auto iInfo = std::make_shared<ShortBlockInfo>(fromCore(
          infoBlocks[i], heightSegment.content[i], heightSegment.source != CryptoNote::BlockSource::MainChain));
      reval.emplace(std::piecewise_construct, std::forward_as_tuple(heightSegment.content[i]),
                    std::forward_as_tuple(iInfo));
    }
  }

  std::vector<std::shared_ptr<ShortBlockInfo>> result{};
  result.resize(heights.size(), nullptr);
  size_t i = 0;
  for (const auto &height : heights) {
    auto search = reval.find(height);
    if (search != reval.end()) {
      result[i] = search->second;
    }
    i += 1;
  }

  return emplaceSuccess<std::vector<std::shared_ptr<ShortBlockInfo>>>(std::move(result));

  XI_ERROR_CATCH();
}

Result<std::vector<std::shared_ptr<BlockInfo>>> CoreExplorer::doQueryBlockInfo(Block::ConstHeightSpan heights) {
  XI_ERROR_TRY();
  XI_UNUSED_REVAL(m_core.lock());

  std::map<Block::Height, std::shared_ptr<BlockInfo>> reval{};

  auto heightSegments = m_core.queryBlockSegments(heights);
  for (const auto &heightSegment : heightSegments) {
    auto rawBlocks = cache(heightSegment.segment->getBlocks(heightSegment.content));
    auto infoBlocks = heightSegment.segment->getBlockInfos(heightSegment.content);

    for (size_t i = 0; i < rawBlocks.size() && i < infoBlocks.size() && i < heightSegment.content.size(); ++i) {
      auto &rawBlock = rawBlocks[i];
      auto iInfo = std::make_shared<BlockInfo>(fromCore(rawBlock, infoBlocks[i], heightSegment.content[i],
                                                        heightSegment.source != CryptoNote::BlockSource::MainChain));
      reval.emplace(std::piecewise_construct, std::forward_as_tuple(heightSegment.content[i]),
                    std::forward_as_tuple(iInfo));
    }
  }

  std::vector<std::shared_ptr<BlockInfo>> result{};
  result.resize(heights.size(), nullptr);
  size_t i = 0;
  for (const auto &height : heights) {
    auto search = reval.find(height);
    if (search != reval.end()) {
      result[i] = search->second;
    }
    i += 1;
  }

  return emplaceSuccess<std::vector<std::shared_ptr<BlockInfo>>>(std::move(result));

  XI_ERROR_CATCH();
}

Result<std::vector<std::shared_ptr<DetailedBlockInfo>>> CoreExplorer::doQueryDetailedBlockInfo(
    Block::ConstHeightSpan heights) {
  XI_ERROR_TRY();
  XI_UNUSED_REVAL(m_core.lock());

  std::map<Block::Height, std::shared_ptr<DetailedBlockInfo>> reval{};

  auto heightSegments = m_core.queryBlockSegments(heights);
  for (const auto &heightSegment : heightSegments) {
    auto rawBlocks = cache(heightSegment.segment->getBlocks(heightSegment.content));
    auto infoBlocks = heightSegment.segment->getBlockInfos(heightSegment.content);

    for (size_t i = 0; i < rawBlocks.size() && i < infoBlocks.size() && i < heightSegment.content.size(); ++i) {
      auto &rawBlock = rawBlocks[i];
      auto iInfo = std::make_shared<DetailedBlockInfo>(
          fromCore(heightSegment.segment.get(), rawBlock, infoBlocks[i], heightSegment.content[i],
                   heightSegment.source != CryptoNote::BlockSource::MainChain));
      reval.emplace(std::piecewise_construct, std::forward_as_tuple(heightSegment.content[i]),
                    std::forward_as_tuple(iInfo));
    }
  }

  std::vector<std::shared_ptr<DetailedBlockInfo>> result{};
  result.resize(heights.size(), nullptr);
  size_t i = 0;
  for (const auto &height : heights) {
    auto search = reval.find(height);
    if (search != reval.end()) {
      result[i] = search->second;
    }
    i += 1;
  }

  return emplaceSuccess<std::vector<std::shared_ptr<DetailedBlockInfo>>>(std::move(result));

  XI_ERROR_CATCH();
}

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi
