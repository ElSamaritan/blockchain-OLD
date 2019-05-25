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
#include <unordered_set>
#include <functional>

#include <Xi/Result.h>
#include <Xi/Concurrent/RecursiveLock.h>

#include <crypto/CryptoTypes.h>
#include <Serialization/ISerializer.h>

#include "CryptoNoteCore/Transactions/CachedTransaction.h"
#include "CryptoNoteCore/Transactions/PendingTransactionInfo.h"

namespace CryptoNote {

struct TransactionValidatorState;
class ITransactionPoolObserver;

class ITransactionPool {
 public:
  using transaction_hashes_container_t = std::vector<Crypto::Hash>;
  using TransactionQueryResult = std::shared_ptr<PendingTransactionInfo>;

 public:
  virtual ~ITransactionPool() = default;

  virtual void addObserver(ITransactionPoolObserver* observer) = 0;
  virtual void removeObserver(ITransactionPoolObserver* observer) = 0;

  /*!
   * \brief size Number of transactions in the pool.
   */
  virtual std::size_t size() const = 0;

  /*!
   * \brief stateHash Returns a hash of the state of the pool. If the pool returns the same hash multiple times the
   * state is unchanged.
   */
  virtual Crypto::Hash stateHash() const = 0;

  /*!
   * \brief forceFlush Deletes all transactions in the pool.
   * \return Number of transactions removed.
   */
  virtual size_t forceFlush() = 0;

  /*!
   * \brief forceErasure Deletes a transaction from the pool
   * \param hash The transactions hash to delete
   * \return True if the transaction was deleted successfully
   */
  virtual bool forceErasure(const Crypto::Hash& hash) = 0;

  /*!
   * \brief pushTransaction parses the binary representation of a transaction and pushs it
   * \param transaction the binary representation of a transaction
   * \return nothing if successfull, otherwise an error
   */
  virtual Xi::Result<void> pushTransaction(BinaryArray transaction) = 0;

  virtual Xi::Result<void> pushTransaction(Transaction transaction) = 0;
  virtual TransactionQueryResult queryTransaction(const Crypto::Hash& hash) const = 0;
  virtual bool containsTransaction(const Crypto::Hash& hash) const = 0;
  virtual bool containsKeyImage(const Crypto::KeyImage& keyImage) const = 0;

  /*!
   * \brief sanityCheck reevalutes a contained transactions, if they are still valid.
   * \param timeout Maximum age in seconds, of a tranasction, older ones get deleted.
   * \return All hashes of deleted transactions. (Observers get still notified)
   */
  virtual std::vector<Crypto::Hash> sanityCheck(const uint64_t timeout) = 0;

  /*!
   * \brief serialize loads/stores the state of the transaction pool from/into a serializer
   * \param serializer the interface for decoding/encoding data pushed
   */
  virtual void serialize(ISerializer& serializer) = 0;

  /*!
   * \brief eligiblePoolTransactions queries pool transactions ready to be mined and sorts them to prioratize more
   * profitable transactions.
   * \param index The eligble index of the blockchain the returned transactions must be eligible
   * \return all transactions contained in the pool satisfying the index requirement
   */
  virtual std::vector<CachedTransaction> eligiblePoolTransactions(EligibleIndex index) const = 0;

  /*!
   * \brief acquireExclusiveAccess locks the transaction pool for exclusive access
   * \return a RAII object holding the log, once destroyed your exclusive access is gone
   */
  virtual Xi::Concurrent::RecursiveLock::lock_t acquireExclusiveAccess() const = 0;

  // DEPRECATED BEGIN | These interfaces may change or get adopted to the new inteface declaration
  virtual CachedTransaction getTransaction(const Crypto::Hash& hash) const = 0;
  virtual bool removeTransaction(const Crypto::Hash& hash) = 0;

  virtual std::vector<Crypto::Hash> getTransactionHashes() const = 0;
  virtual bool checkIfTransactionPresent(const Crypto::Hash& hash) const = 0;

  virtual std::vector<CachedTransaction> getPoolTransactions() const = 0;

  virtual uint64_t getTransactionReceiveTime(const Crypto::Hash& hash) const = 0;
  virtual std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const Crypto::Hash& paymentId) const = 0;
};

}  // namespace CryptoNote
