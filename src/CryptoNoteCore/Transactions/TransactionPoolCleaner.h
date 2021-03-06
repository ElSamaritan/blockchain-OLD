﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include "CryptoNoteCore/Transactions/ITransactionPoolCleaner.h"

#include <chrono>
#include <unordered_map>
#include <vector>

#include "crypto/crypto.h"

#include "CryptoNoteCore/Time/LocalTimeProvider.h"
#include "CryptoNoteCore/Transactions/ITransactionPool.h"
#include "Logging/ILogger.h"
#include "Logging/LoggerRef.h"

#include <Xi/Global.hh>
#include <System/ContextGroup.h>

namespace CryptoNote {

class TransactionPoolCleanWrapper : public ITransactionPool {
  XI_DELETE_COPY(TransactionPoolCleanWrapper);
  XI_DEFAULT_MOVE(TransactionPoolCleanWrapper);

 public:
  TransactionPoolCleanWrapper(std::unique_ptr<ITransactionPool>&& transactionPool,
                              std::unique_ptr<ITimeProvider>&& timeProvider, Logging::ILogger& logger,
                              uint64_t m_timeout);

  ~TransactionPoolCleanWrapper() override;

  void addObserver(ITransactionPoolObserver* observer) override;
  void removeObserver(ITransactionPoolObserver* observer) override;

  std::size_t size() const override;
  std::size_t cumulativeSize() const override;
  std::size_t cumulativeFees() const override;
  Crypto::Hash stateHash() const override;
  size_t forceFlush() override;
  bool forceErasure(const Crypto::Hash& hash) override;
  Xi::Result<void> pushTransaction(BinaryArray transactionBlob) override;
  Xi::Result<void> pushTransaction(Transaction transaction) override;
  bool containsTransaction(const Crypto::Hash& hash) const override;
  bool containsKeyImage(const Crypto::KeyImage& keyImage) const override;
  std::vector<Crypto::Hash> sanityCheck(const uint64_t timeout) override;
  [[nodiscard]] bool serialize(ISerializer& serializer) override;
  [[nodiscard]] bool load(const std::string& dataDir) override;
  [[nodiscard]] bool save(const std::string& dataDir) override;
  TransactionQueryResult queryTransaction(const Crypto::Hash& hash) const override;
  std::vector<CachedTransaction> eligiblePoolTransactions(EligibleIndex index) const override;
  Xi::Concurrent::RecursiveLock::lock_t acquireExclusiveAccess() const override;

  CachedTransaction getTransaction(const Crypto::Hash& hash) const override;
  bool removeTransaction(const Crypto::Hash& hash) override;

  std::vector<Crypto::Hash> getTransactionHashes() const override;
  bool checkIfTransactionPresent(const Crypto::Hash& hash) const override;

  std::vector<CachedTransaction> getPoolTransactions() const override;

  uint64_t getTransactionReceiveTime(const Crypto::Hash& hash) const override;
  std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const PaymentId& paymentId) const override;

  std::vector<Crypto::Hash> clean();

 private:
  std::unique_ptr<ITransactionPool> transactionPool;
  std::unique_ptr<ITimeProvider> timeProvider;
  Logging::LoggerRef logger;
  std::unordered_map<Crypto::Hash, uint64_t> recentlyDeletedTransactions;
  uint64_t m_timeout;

  bool isTransactionRecentlyDeleted(const Crypto::Hash& hash) const;
  void cleanRecentlyDeletedTransactions(uint64_t currentTime);
};

}  // namespace CryptoNote
