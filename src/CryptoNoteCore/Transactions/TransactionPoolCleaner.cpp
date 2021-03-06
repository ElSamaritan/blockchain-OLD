﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "CryptoNoteCore/Transactions/TransactionPoolCleaner.h"

#include <Xi/Exceptions.hpp>

#include <Common/StringTools.h>
#include <System/InterruptedException.h>
#include <System/Timer.h>

#include "CryptoNoteCore/Core.h"

namespace CryptoNote {

TransactionPoolCleanWrapper::TransactionPoolCleanWrapper(std::unique_ptr<ITransactionPool>&& transactionPool,
                                                         std::unique_ptr<ITimeProvider>&& timeProvider,
                                                         Logging::ILogger& logger, uint64_t timeout)
    : transactionPool(std::move(transactionPool)),
      timeProvider(std::move(timeProvider)),
      logger(logger, "TransactionPoolCleanWrapper"),
      m_timeout(timeout) {
  assert(this->timeProvider);
}

TransactionPoolCleanWrapper::~TransactionPoolCleanWrapper() {
}

void TransactionPoolCleanWrapper::addObserver(ITransactionPoolObserver* observer) {
  transactionPool->addObserver(observer);
}

void TransactionPoolCleanWrapper::removeObserver(ITransactionPoolObserver* observer) {
  transactionPool->removeObserver(observer);
}

std::size_t TransactionPoolCleanWrapper::size() const {
  return transactionPool->size();
}

std::size_t TransactionPoolCleanWrapper::cumulativeSize() const {
  return transactionPool->cumulativeSize();
}

std::size_t TransactionPoolCleanWrapper::cumulativeFees() const {
  return transactionPool->cumulativeFees();
}

Crypto::Hash TransactionPoolCleanWrapper::stateHash() const {
  return transactionPool->stateHash();
}

size_t TransactionPoolCleanWrapper::forceFlush() {
  return transactionPool->forceFlush();
}

bool TransactionPoolCleanWrapper::forceErasure(const Crypto::Hash& hash) {
  return transactionPool->forceErasure(hash);
}

Xi::Result<void> TransactionPoolCleanWrapper::pushTransaction(BinaryArray transactionBlob) {
  return transactionPool->pushTransaction(std::move(transactionBlob));
}

Xi::Result<void> TransactionPoolCleanWrapper::pushTransaction(Transaction transaction) {
  return transactionPool->pushTransaction(std::move(transaction));
}

bool TransactionPoolCleanWrapper::containsTransaction(const Crypto::Hash& hash) const {
  return transactionPool->containsTransaction(hash);
}

bool TransactionPoolCleanWrapper::containsKeyImage(const Crypto::KeyImage& keyImage) const {
  return transactionPool->containsKeyImage(keyImage);
}

std::vector<Crypto::Hash> TransactionPoolCleanWrapper::sanityCheck(const uint64_t timeout) {
  return transactionPool->sanityCheck(timeout);
}

bool TransactionPoolCleanWrapper::serialize(ISerializer& serializer) {
  return transactionPool->serialize(serializer);
}

bool TransactionPoolCleanWrapper::load(const std::string& dataDir) {
  return transactionPool->load(dataDir);
}

bool TransactionPoolCleanWrapper::save(const std::string& dataDir) {
  return transactionPool->save(dataDir);
}

ITransactionPool::TransactionQueryResult TransactionPoolCleanWrapper::queryTransaction(const Crypto::Hash& hash) const {
  return transactionPool->queryTransaction(hash);
}

std::vector<CachedTransaction> TransactionPoolCleanWrapper::eligiblePoolTransactions(EligibleIndex index) const {
  return transactionPool->eligiblePoolTransactions(index);
}

Xi::Concurrent::RecursiveLock::lock_t TransactionPoolCleanWrapper::acquireExclusiveAccess() const {
  return transactionPool->acquireExclusiveAccess();
}

CachedTransaction TransactionPoolCleanWrapper::getTransaction(const Crypto::Hash& hash) const {
  return transactionPool->getTransaction(hash);
}

bool TransactionPoolCleanWrapper::removeTransaction(const Crypto::Hash& hash) {
  return transactionPool->removeTransaction(hash);
}

std::vector<Crypto::Hash> TransactionPoolCleanWrapper::getTransactionHashes() const {
  return transactionPool->getTransactionHashes();
}

bool TransactionPoolCleanWrapper::checkIfTransactionPresent(const Crypto::Hash& hash) const {
  return transactionPool->checkIfTransactionPresent(hash);
}

std::vector<CachedTransaction> TransactionPoolCleanWrapper::getPoolTransactions() const {
  return transactionPool->getPoolTransactions();
}

uint64_t TransactionPoolCleanWrapper::getTransactionReceiveTime(const Crypto::Hash& hash) const {
  return transactionPool->getTransactionReceiveTime(hash);
}

std::vector<Crypto::Hash> TransactionPoolCleanWrapper::getTransactionHashesByPaymentId(
    const PaymentId& paymentId) const {
  return transactionPool->getTransactionHashesByPaymentId(paymentId);
}

std::vector<Crypto::Hash> TransactionPoolCleanWrapper::clean() {
  try {
    uint64_t currentTime = timeProvider->posixNow().value();
    std::vector<Crypto::Hash> deletedTransactions = sanityCheck(m_timeout);
    cleanRecentlyDeletedTransactions(currentTime);
    return deletedTransactions;
  } catch (System::InterruptedException&) {
    throw;
  } catch (std::exception& e) {
    logger(Logging::Warning) << "Caught an exception: " << e.what() << ", stopping cleaning procedure cycle";
    throw;
  }
}

bool TransactionPoolCleanWrapper::isTransactionRecentlyDeleted(const Crypto::Hash& hash) const {
  auto it = recentlyDeletedTransactions.find(hash);
  return it != recentlyDeletedTransactions.end() && it->second >= m_timeout;
}

void TransactionPoolCleanWrapper::cleanRecentlyDeletedTransactions(uint64_t currentTime) {
  for (auto it = recentlyDeletedTransactions.begin(); it != recentlyDeletedTransactions.end();) {
    if (currentTime - it->second >= m_timeout) {
      it = recentlyDeletedTransactions.erase(it);
    } else {
      ++it;
    }
  }
}

}  // namespace CryptoNote
