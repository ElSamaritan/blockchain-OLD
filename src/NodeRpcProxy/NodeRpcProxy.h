﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include <Xi/Global.hh>
#include <Xi/Http/Client.h>
#include <Xi/Http/SSLConfiguration.h>

#include "Common/ObserverManager.h"
#include "Logging/LoggerRef.h"
#include "CryptoNoteCore/INode.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"

namespace System {
class ContextGroup;
class Dispatcher;
class Event;
}  // namespace System

namespace CryptoNote {

using HttpClient = Xi::Http::Client;

class INodeRpcProxyObserver {
 public:
  virtual ~INodeRpcProxyObserver() {
  }
  virtual void connectionStatusUpdated(bool connected) {
    XI_UNUSED(connected);
  }
};

class NodeRpcProxy : public CryptoNote::INode {
 public:
  NodeRpcProxy(const std::string& nodeHost, unsigned short nodePort, ::Xi::Http::SSLConfiguration sslConfig,
               const Currency& currency, Logging::ILogger& logger);
  virtual ~NodeRpcProxy() override;

  virtual bool addObserver(CryptoNote::INodeObserver* observer) override;
  virtual bool removeObserver(CryptoNote::INodeObserver* observer) override;

  virtual bool addObserver(CryptoNote::INodeRpcProxyObserver* observer);
  virtual bool removeObserver(CryptoNote::INodeRpcProxyObserver* observer);

  virtual void init(const Callback& callback) override;
  virtual void getLastBlockHeaderInfo(BlockHeaderInfo& info, const Callback& callback) override;
  virtual bool shutdown() override;

  virtual size_t getPeerCount() const override;
  virtual BlockVersion getLastKnownBlockVersion() const override;
  virtual BlockHeight getLastLocalBlockHeight() const override;
  virtual BlockHeight getLastKnownBlockHeight() const override;
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

  virtual void relayTransaction(const CryptoNote::Transaction& transaction, const Callback& callback) override;
  virtual void getRandomOutsByAmounts(std::map<uint64_t, uint64_t>&& amounts,
                                      std::vector<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result,
                                      const Callback& callback) override;
  virtual void getRequiredMixinByAmounts(std::set<uint64_t>&& amounts, std::map<uint64_t, uint64_t>& out,
                                         const Callback& callback) override;

  virtual void getNewBlocks(std::vector<Crypto::Hash>&& knownBlockIds, std::vector<CryptoNote::RawBlock>& newBlocks,
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

  unsigned int rpcTimeout() const {
    return m_rpcTimeout;
  }
  void rpcTimeout(unsigned int val) {
    m_rpcTimeout = val;
  }

  void setPollUpdatesEnabled(bool enabled);
  bool pollUpdatesEnabled() const;

 private:
  void resetInternalState();
  void workerThread(const Callback& initialized_callback);

  std::vector<Crypto::Hash> getKnownTxsVector() const;
  void pullNodeStatusAndScheduleTheNext();
  void updateNodeStatus();
  std::error_code updateBlockchainStatus();
  bool updatePoolStatus();
  void updatePeerCount(size_t peerCount);
  void updatePoolState(const std::vector<std::unique_ptr<ITransactionReader>>& addedTxs,
                       const std::vector<Crypto::Hash>& deletedTxsIds);

  std::error_code doGetLastBlockHeaderInfo(BlockHeaderInfo& info);
  std::error_code doGetBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount,
                                               std::vector<Crypto::Hash>& blockHashes);
  std::error_code doRelayTransaction(const CryptoNote::Transaction& transaction);
  std::error_code doGetRandomOutsByAmounts(
      std::map<uint64_t, uint64_t>& amounts,
      std::vector<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result);
  std::error_code doGetRequiredMixinByAmounts(std::set<uint64_t>& amounts, std::map<uint64_t, uint64_t>& out);
  std::error_code doGetNewBlocks(std::vector<Crypto::Hash>& knownBlockIds, std::vector<CryptoNote::RawBlock>& newBlocks,
                                 BlockHeight& startHeight);
  std::error_code doGetTransactionOutsGlobalIndices(const Crypto::Hash& transactionHash,
                                                    std::vector<uint32_t>& outsGlobalIndices);
  std::error_code doQueryBlocksLite(const std::vector<Crypto::Hash>& knownBlockIds, uint64_t timestamp,
                                    std::vector<CryptoNote::BlockShortEntry>& newBlocks, BlockHeight& startHeight);
  std::error_code doGetPoolSymmetricDifference(std::vector<Crypto::Hash>&& knownPoolTxIds, Crypto::Hash knownBlockId,
                                               bool& isBcActual,
                                               std::vector<std::unique_ptr<ITransactionReader>>& newTxs,
                                               std::vector<Crypto::Hash>& deletedTxIds);
  std::error_code doGetBlocksByHeight(const std::vector<BlockHeight>& blockHeights,
                                      std::vector<std::vector<BlockDetails>>& blocks);
  std::error_code doGetBlocksByHash(const std::vector<Crypto::Hash>& blockHashes, std::vector<BlockDetails>& blocks);
  std::error_code doGetRawBlocksByRange(BlockHeight height, uint32_t count, std::vector<RawBlock>& blocks);
  std::error_code doGetBlock(const BlockHeight blockHeight, BlockDetails& block);
  std::error_code doGetTransactionHashesByPaymentId(const PaymentId& paymentId,
                                                    std::vector<Crypto::Hash>& transactionHashes);
  std::error_code doGetTransactions(const std::vector<Crypto::Hash>& transactionHashes,
                                    std::vector<TransactionDetails>& transactions);

  void scheduleRequest(std::function<std::error_code()>&& procedure, const Callback& callback);
  template <typename Request, typename Response>
  std::error_code binaryCommand(const std::string& url, const Request& req, Response& res);
  template <typename Request, typename Response>
  std::error_code jsonCommand(const std::string& url, const Request& req, Response& res);
  template <typename Request, typename Response>
  std::error_code jsonRpcCommand(const std::string& method, const Request& req, Response& res);

  enum State { STATE_NOT_INITIALIZED, STATE_INITIALIZING, STATE_INITIALIZED };

 private:
  Logging::LoggerRef m_logger;
  const Currency& m_currency;
  State m_state = STATE_NOT_INITIALIZED;
  mutable std::mutex m_mutex;
  std::condition_variable m_cv_initialized;
  std::thread m_workerThread;
  System::Dispatcher* m_dispatcher = nullptr;
  System::ContextGroup* m_context_group = nullptr;
  Tools::ObserverManager<CryptoNote::INodeObserver> m_observerManager;
  Tools::ObserverManager<CryptoNote::INodeRpcProxyObserver> m_rpcProxyObserverManager;

  const std::string m_nodeHost;
  const unsigned short m_nodePort;
  unsigned int m_rpcTimeout;
  std::unique_ptr<Xi::Http::Client> m_httpClient;
  System::Event* m_httpEvent = nullptr;

  uint64_t m_pullInterval;

  // Internal state
  bool m_stop = false;
  std::atomic_bool m_pollUpdates{true};
  std::atomic<size_t> m_peerCount;
  std::atomic<BlockHeight> m_networkHeight;
  std::atomic<BlockVersion::value_type> m_networkVersion;
  std::atomic<BlockHeight> m_nodeHeight;

  BlockHeaderInfo lastLocalBlockHeaderInfo;
  // protect it with mutex if decided to add worker threads
  std::unordered_set<Crypto::Hash> m_knownTxs;

  bool m_connected;
  std::optional<FeeAddress> m_fee = std::nullopt;
};
}  // namespace CryptoNote
