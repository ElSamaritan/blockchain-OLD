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

#include <atomic>
#include <thread>
#include <memory>

#include <Common/ObserverManager.h>
#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>
#include <P2p/NetNode.h>
#include <CryptoNoteProtocol/CryptoNoteProtocolHandler.h>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/INode.h>
#include <CryptoNoteCore/ICore.h>
#include <CryptoNoteCore/Transactions/ITransactionPoolObserver.h>

namespace CryptoNote {

class NodeInProcess final : public INode, ICryptoNoteProtocolObserver, IBlockchainObserver, ITransactionPoolObserver {
 public:
  enum struct State {
    NotInitialized,
    Initializing,
    Initialized,
  };

 public:
  explicit NodeInProcess(NodeServer& p2p, CryptoNoteProtocolHandler& protocol, ICore& core, Logging::ILogger& logger);
  ~NodeInProcess() override;

  State state() const;

  // IBlockchainObserver
  void blockAdded(uint32_t index, const Crypto::Hash& hash) override;
  void mainChainSwitched(const IBlockchainCache& previous, const IBlockchainCache& current,
                         uint32_t splitIndex) override;

  // ITransactionPoolObserver
  void transactionDeletedFromPool(const Crypto::Hash& hash, DeletionReason reason) override;
  void transactionAddedToPool(const Crypto::Hash& hash, AdditionReason reason) override;

  // ICryptoNoteProtocolObserver
  void peerCountUpdated(size_t count) override;
  void lastKnownBlockHeightUpdated(BlockHeight height) override;
  void blockchainSynchronized(BlockHeight topHeight) override;

  // INode
  virtual bool addObserver(INodeObserver* observer) override;
  virtual bool removeObserver(INodeObserver* observer) override;

  virtual void init(const Callback& callback) override;
  virtual bool shutdown() override;
  virtual size_t getPeerCount() const override;
  virtual BlockHeight getLastLocalBlockHeight() const override;
  virtual BlockHeight getLastKnownBlockHeight() const override;
  virtual BlockVersion getLastKnownBlockVersion() const override;
  virtual uint32_t getLocalBlockCount() const override;
  virtual uint32_t getKnownBlockCount() const override;
  virtual uint64_t getLastLocalBlockTimestamp() const override;
  virtual BlockHeight getNodeHeight() const override;
  virtual void getFeeInfo() override;
  virtual const Currency& currency() const override;
  virtual std::error_code ping() override;

  virtual void getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount,
                                          std::vector<Crypto::Hash>& blockHashes, const Callback& callback) override;
  virtual void getTransactionHashesByPaymentId(const PaymentId& paymentId, std::vector<Crypto::Hash>& transactionHashes,
                                               const Callback& callback) override;

  virtual BlockHeaderInfo getLastLocalBlockHeaderInfo() const override;
  virtual void getLastBlockHeaderInfo(BlockHeaderInfo& info, const Callback& callback) override;

  virtual void relayTransaction(const Transaction& transaction, const Callback& callback) override;
  virtual void getRandomOutsByAmounts(
      std::map<uint64_t, uint64_t>&& amounts,
      std::vector<CryptoNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result,
      const Callback& callback) override;
  virtual void getRequiredMixinByAmounts(std::set<uint64_t>&& amounts, std::map<uint64_t, uint64_t>& out,
                                         const Callback& callback) override;
  virtual void getNewBlocks(std::vector<Crypto::Hash>&& knownBlockIds, std::vector<RawBlock>& newBlocks,
                            BlockHeight& startHeight, const Callback& callback) override;
  virtual void getTransactionOutsGlobalIndices(const Crypto::Hash& transactionHash,
                                               std::vector<uint32_t>& outsGlobalIndices,
                                               const Callback& callback) override;
  virtual void queryBlocks(std::vector<Crypto::Hash>&& knownBlockIds, uint64_t timestamp,
                           std::vector<BlockShortEntry>& newBlocks, BlockHeight& startHeight,
                           const Callback& callback) override;
  virtual void getPoolSymmetricDifference(std::vector<Crypto::Hash>&& knownPoolTxIds, Crypto::Hash knownBlockId,
                                          bool& isBcActual, std::vector<std::unique_ptr<ITransactionReader>>& newTxs,
                                          std::vector<Crypto::Hash>& deletedTxIds, const Callback& callback) override;

  virtual void getBlocks(const std::vector<BlockHeight>& blockHeights, std::vector<std::vector<BlockDetails>>& blocks,
                         const Callback& callback) override;
  virtual void getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<BlockDetails>& blocks,
                         const Callback& callback) override;
  virtual void getRawBlocksByRange(BlockHeight height, uint32_t count, std::vector<RawBlock>& blocks,
                                   const Callback& callback) override;
  virtual void getBlock(const BlockHeight blockHeight, BlockDetails& block, const Callback& callback) override;
  virtual void getTransactions(const std::vector<Crypto::Hash>& transactionHashes,
                               std::vector<TransactionDetails>& transactions, const Callback& callback) override;
  virtual void isSynchronized(bool& syncStatus, const Callback& callback) override;
  virtual std::optional<FeeAddress> feeAddress() const override;

 private:
  NodeServer& m_p2p;
  CryptoNoteProtocolHandler& m_protocol;
  ICore& m_core;
  Logging::LoggerRef m_logger;
  Tools::ObserverManager<INodeObserver> m_observers{};
  std::atomic<State> m_state{State::NotInitialized};
  std::thread m_initThread{};
  BlockHeaderInfo m_cachedTopInfo{};
};

}  // namespace CryptoNote
