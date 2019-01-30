// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "CryptoNoteCore/Transactions/TransactionPoolCleaner.h"

#include <Common/StringTools.h>
#include <System/InterruptedException.h>
#include <System/Timer.h>

#include "CryptoNoteCore/Core.h"
#include "CryptoNoteCore/Transactions/Mixins.h"

namespace CryptoNote {

TransactionPoolCleanWrapper::TransactionPoolCleanWrapper(std::unique_ptr<ITransactionPool>&& transactionPool,
                                                         std::unique_ptr<ITimeProvider>&& timeProvider,
                                                         Logging::ILogger& logger, uint64_t timeout)
    : transactionPool(std::move(transactionPool)),
      timeProvider(std::move(timeProvider)),
      logger(logger, "TransactionPoolCleanWrapper"),
      timeout(timeout) {
  assert(this->timeProvider);
}

TransactionPoolCleanWrapper::~TransactionPoolCleanWrapper() {}

void TransactionPoolCleanWrapper::addObserver(ITransactionPoolObserver* observer) {
  transactionPool->addObserver(observer);
}

void TransactionPoolCleanWrapper::removeObserver(ITransactionPoolObserver* observer) {
  transactionPool->removeObserver(observer);
}

std::size_t TransactionPoolCleanWrapper::size() const { return transactionPool->size(); }

Crypto::Hash TransactionPoolCleanWrapper::stateHash() const { return transactionPool->stateHash(); }

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

void TransactionPoolCleanWrapper::serialize(ISerializer& serializer) { return transactionPool->serialize(serializer); }

ITransactionPool::TransactionQueryResult TransactionPoolCleanWrapper::queryTransaction(const Crypto::Hash& hash) const {
  return transactionPool->queryTransaction(hash);
}

std::vector<CachedTransaction> TransactionPoolCleanWrapper::eligiblePoolTransactions(
    TransactionValidationResult::EligibleIndex index) const {
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
    const Crypto::Hash& paymentId) const {
  return transactionPool->getTransactionHashesByPaymentId(paymentId);
}

std::vector<Crypto::Hash> TransactionPoolCleanWrapper::clean(const uint32_t height) {
  try {
    uint64_t currentTime = timeProvider->posixNow().value();
    auto transactionHashes = transactionPool->getTransactionHashes();

    std::vector<Crypto::Hash> deletedTransactions;
    for (const auto& hash : transactionHashes) {
      uint64_t transactionAge = currentTime - transactionPool->getTransactionReceiveTime(hash);
      if (transactionAge >= timeout) {
        logger(Logging::DEBUGGING) << "Deleting transaction " << Common::podToHex(hash) << " from pool";
        recentlyDeletedTransactions.emplace(hash, currentTime);
        transactionPool->removeTransaction(hash);
        deletedTransactions.emplace_back(std::move(hash));
      }

      CachedTransaction transaction = transactionPool->getTransaction(hash);
      std::vector<CachedTransaction> transactions;
      transactions.emplace_back(transaction);

      bool success;
      std::string error;

      std::tie(success, error) = Mixins::validate(transactions, height);
      if (!success) {
        logger(Logging::DEBUGGING) << "Deleting invalid transaction " << Common::podToHex(hash) << " from pool."
                                   << error;
        recentlyDeletedTransactions.emplace(hash, currentTime);
        transactionPool->removeTransaction(hash);
        deletedTransactions.emplace_back(std::move(hash));
      }
    }

    cleanRecentlyDeletedTransactions(currentTime);
    return deletedTransactions;
  } catch (System::InterruptedException&) {
    throw;
  } catch (std::exception& e) {
    logger(Logging::WARNING) << "Caught an exception: " << e.what() << ", stopping cleaning procedure cycle";
    throw;
  }
}

bool TransactionPoolCleanWrapper::isTransactionRecentlyDeleted(const Crypto::Hash& hash) const {
  auto it = recentlyDeletedTransactions.find(hash);
  return it != recentlyDeletedTransactions.end() && it->second >= timeout;
}

void TransactionPoolCleanWrapper::cleanRecentlyDeletedTransactions(uint64_t currentTime) {
  for (auto it = recentlyDeletedTransactions.begin(); it != recentlyDeletedTransactions.end();) {
    if (currentTime - it->second >= timeout) {
      it = recentlyDeletedTransactions.erase(it);
    } else {
      ++it;
    }
  }
}

}  // namespace CryptoNote
