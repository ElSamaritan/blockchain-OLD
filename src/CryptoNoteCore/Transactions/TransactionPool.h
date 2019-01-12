// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <vector>
#include <cinttypes>
#include <unordered_map>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <crypto/crypto.h>
#include <Logging/LoggerMessage.h>
#include <Logging/LoggerRef.h>

#include "CryptoNoteCore/Transactions/ITransactionPool.h"
#include "CryptoNoteCore/Transactions/TransactionPriortiyComparator.h"
#include "CryptoNoteCore/Transactions/PendingTransactionInfo.h"
#include "CryptoNoteCore/Transactions/TransactionValidatiorState.h"

namespace CryptoNote {

class TransactionPool : public ITransactionPool {
 public:
  TransactionPool(Logging::ILogger& logger);

  bool pushTransaction(CachedTransaction&& transaction, TransactionValidatorState&& transactionState) override;
  const CachedTransaction& getTransaction(const Crypto::Hash& hash) const override;
  bool removeTransaction(const Crypto::Hash& hash) override;

  size_t getTransactionCount() const override;
  std::vector<Crypto::Hash> getTransactionHashes() const override;
  bool checkIfTransactionPresent(const Crypto::Hash& hash) const override;

  const TransactionValidatorState& getPoolTransactionValidationState() const override;
  std::vector<CachedTransaction> getPoolTransactions() const override;

  uint64_t getTransactionReceiveTime(const Crypto::Hash& hash) const override;
  std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const Crypto::Hash& paymentId) const override;

 private:
  TransactionValidatorState poolState;

  struct TransactionHashTag {};
  struct TransactionCostTag {};
  struct PaymentIdTag {};

  typedef boost::multi_index::ordered_non_unique<boost::multi_index::tag<TransactionCostTag>,
                                                 boost::multi_index::identity<PendingTransactionInfo>,
                                                 TransactionPriorityComparator>
      TransactionCostIndex;

  typedef boost::multi_index::hashed_unique<
      boost::multi_index::tag<TransactionHashTag>,
      boost::multi_index::const_mem_fun<PendingTransactionInfo, const Crypto::Hash&,
                                        &PendingTransactionInfo::getTransactionHash> >
      TransactionHashIndex;

  struct PaymentIdHasher {
    size_t operator()(const boost::optional<Crypto::Hash>& paymentId) const;
  };

  typedef boost::multi_index::hashed_non_unique<
      boost::multi_index::tag<PaymentIdTag>,
      BOOST_MULTI_INDEX_MEMBER(PendingTransactionInfo, boost::optional<Crypto::Hash>, paymentId), PaymentIdHasher>
      PaymentIdIndex;

  typedef boost::multi_index_container<
      PendingTransactionInfo,
      boost::multi_index::indexed_by<TransactionHashIndex, TransactionCostIndex, PaymentIdIndex> >
      TransactionsContainer;

  TransactionsContainer transactions;
  TransactionsContainer::index<TransactionHashTag>::type& transactionHashIndex;
  TransactionsContainer::index<TransactionCostTag>::type& transactionCostIndex;
  TransactionsContainer::index<PaymentIdTag>::type& paymentIdIndex;

  Logging::LoggerRef logger;

  using mutex_t = boost::shared_mutex;
  using read_lock_t = boost::shared_lock<mutex_t>;
  using write_lock_t = boost::unique_lock<mutex_t>;

  /*!
   * \brief m_mutationMutex should always be locked using the according lock_guard
   *
   * The mutex protects from concurrent mutation of the state. The locks implement a multiple read single write
   * protection. Thus if you want to read data use read_lock_t and if you are mutating the data use wirte_lock_t.
   */
  mutex_t m_mutationMutex;
};

}  // namespace CryptoNote
