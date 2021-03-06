﻿/* ============================================================================================== *
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

#include <vector>
#include <cinttypes>
#include <atomic>
#include <unordered_map>
#include <unordered_set>

#include <Xi/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>
#include <Xi/Concurrent/RecursiveLock.h>

#include <crypto/crypto.h>
#include <Common/ObserverManager.h>
#include <Logging/LoggerMessage.h>
#include <Logging/LoggerRef.h>

#include "CryptoNoteCore/Blockchain/IBlockchain.h"
#include "CryptoNoteCore/Blockchain/IBlockchainObserver.h"
#include "CryptoNoteCore/Transactions/ITransactionPool.h"
#include "CryptoNoteCore/Transactions/ITransactionPoolObserver.h"
#include "CryptoNoteCore/Transactions/ITransactionValidator.h"
#include "CryptoNoteCore/Transactions/TransactionPriortiyComparator.h"
#include "CryptoNoteCore/Transactions/PendingTransactionInfo.h"
#include "CryptoNoteCore/Transactions/TransactionValidatiorState.h"

namespace CryptoNote {
class TransactionPool : public ITransactionPool, private IBlockchainObserver {
  XI_DELETE_COPY(TransactionPool);
  XI_DELETE_MOVE(TransactionPool);

 public:
  TransactionPool(IBlockchain& blockchain, Logging::ILogger& m_logger);
  ~TransactionPool() override;

  // ------------------------------------- ITransactionPool Begin ------------------------------------------------------
 public:
  void addObserver(ITransactionPoolObserver* observer) override;
  void removeObserver(ITransactionPoolObserver* observer) override;

  std::size_t size() const override;
  std::size_t cumulativeSize() const override;
  std::size_t cumulativeFees() const override;
  Crypto::Hash stateHash() const override;

  size_t forceFlush() override;
  bool forceErasure(const Crypto::Hash& hash) override;

  Xi::Result<void> pushTransaction(BinaryArray transaction) override;
  Xi::Result<void> pushTransaction(Transaction transaction) override;
  bool containsTransaction(const Crypto::Hash& hash) const override;
  bool containsKeyImage(const Crypto::KeyImage& keyImage) const override;

  std::vector<Crypto::Hash> sanityCheck(const uint64_t timeout) override;

  /*!
   * \copydoc ITransactionPool::serialize(ISerializer)
   *
   * Transactions and their receive time will be serialized. Eligible indices will be skipped as can and should
   * be recalcuated once the cache gets restored, othwerise they may wont be up to date.
   */
  [[nodiscard]] bool serialize(ISerializer& serializer) override;

  [[nodiscard]] bool load(const std::string& dataDir) override;
  [[nodiscard]] bool save(const std::string& dataDir) override;

  TransactionQueryResult queryTransaction(const Crypto::Hash& hash) const override;
  std::vector<CachedTransaction> eligiblePoolTransactions(EligibleIndex index) const override;
  Xi::Concurrent::RecursiveLock::lock_t acquireExclusiveAccess() const override;
  // -------------------------------------- ITransactionPool End -------------------------------------------------------

  // ----------------------------------- IBlockchainObserver Begin -----------------------------------------------------
 private:
  void blockAdded(uint32_t index, const Crypto::Hash& hash) override;

  void mainChainSwitched(const IBlockchainCache& previous, const IBlockchainCache& current,
                         uint32_t splitIndex) override;
  // ------------------------------------ IBlockchainObserver End ------------------------------------------------------

 private:
  /*!
   * \brief pushBlock processes a new block added to the main chain
   * \param block The raw representation of the new block
   */
  void pushBlock(RawBlock block);

  /*!
   * \brief popBlock tries to invert changes to the transaction pool by and added block
   * \param block The raw representation of a block that is no longer part of the main chain.
   *
   * Once the main chain switches the previous main chain may contains transactions that now need to be reconsidered.
   * Therefore the transaction pool adds the processed transactions by the given block back to the transaction pool.
   */
  void popBlock(RawBlock block);

  /*!
   * \brief pushBlockTransaction updates the internal state based on an mined transaction, considered to be valid
   * \param transactionBlob The binary representation of the transaction contained in the block
   * \param newAmounts Gathers new amounts available to batch update partially mixed transactions
   *
   * Once a transaction is mined on the main chain and accepted by the blockchain the transactions are considered as
   * valid. Thus we need to update the internal state of transactions. There are generally two cases that can happen.
   *
   * 1. The transaction pool contains the referenced transaction. We remove the transaction and notify observers that a
   * pool transaction has been mined.
   *
   * 2. The transaction was unknown to the pool. The incoming transaction may invalidate other transactions in the pool
   * as they could reference the same inputs. We need to check whether transaction may conflict with the newly mined one
   * and delete them.
   */
  void pushBlockTransaction(BinaryArray transactionBlob, std::map<uint64_t, uint64_t> newAmounts);

  /*!
   * \brief popBlockTransaction tries to recover a transaction that was processed in the previous main chain that was
   * replaced by an alternative chain.
   * \param transactionBlob The raw transaction blob contained by a block that got reverted during the main chain
   * switch.
   */
  void popBlockTransaction(BinaryArray transactionBlob);

  /*!
   * \brief removeTransaction removes the transaction and updates the state accordingly, observers will be notified
   * \param hash The hash of the transaction to be removed
   * \param reason The reasoning about the removal
   * \return true if the transaction was found and removed, otherwise false
   */
  bool removeTransaction(const Crypto::Hash& hash, ITransactionPoolObserver::DeletionReason reason);

  /*!
   * \brief currentEligibleIndex computes the minimum mainChain index in order to accept incoming transactions
   * \return A blockchain index that must be satisfied in order to accept incoming transactions
   */
  Xi::Result<EligibleIndex> currentEligibleIndex() const;

  /*!
   * \brief insertTransaction tries to enqueue a new transaction
   * \param transaction the transaction to enqueue
   * \param reason why the transaction is added
   * \return Nothing if the transaction was successfully added otherwise an error
   */
  Xi::Result<void> insertTransaction(Transaction transaction, ITransactionPoolObserver::AdditionReason reason);

  /*!
   * \brief insertTransaction tries to enqueue a new transaction
   * \param transaction the transaction to enqueue
   * \param reason why the transaction is added
   * \return Nothing if the transaction was successfully added otherwise an error
   */
  Xi::Result<void> insertTransaction(CachedTransaction transaction, ITransactionPoolObserver::AdditionReason reason);

  /*!
   * \brief insertTransaction tries to enqueue a new transaction
   * \param transaction the transaction to enqueue
   * \param receiveTime the posxi timestamp the transaction was received
   * \param reason why the transaction is added
   * \return Nothing if the transaction was successfully added otherwise an error
   */
  Xi::Result<void> insertTransaction(CachedTransaction transaction, PosixTimestamp receiveTime,
                                     ITransactionPoolObserver::AdditionReason reason);

  /*!
   * \brief computeStateHash calculates a hash of all contained transactions
   * \return A unique hash encoding the state of the pool
   */
  Crypto::Hash computeStateHash() const;

  /*!
   * \brief invalidateStateHash marks the state hash as invalid to enforce recomputation next time queired
   */
  void invalidateStateHash();

  /*!
   * \brief evaluateBlockVersionUpgradeConstraints reevaluates all transactions that may got invalid due to higher
   * constraints after a version upgrade
   */
  void evaluateBlockVersionUpgradeConstraints();

 private:
  IBlockchain& m_blockchain;                                     ///< The blockchain this pool operates on
  Tools::ObserverManager<ITransactionPoolObserver> m_observers;  ///< observers to notify about pool changes
  Xi::Concurrent::RecursiveLock m_access;                        ///< Mutex fron concurrent read single write access
  mutable boost::optional<Crypto::Hash> m_stateHash;     ///< Lazy initialized and cached tree hash of all transactions
  boost::optional<BlockVersion> m_eligibleBlockVersion;  ///< Stores the block version all transactions are elgible for.
  std::atomic<std::size_t> m_cumulativeSize{0};          /// Sum of all transaction blob sizes.
  std::atomic<std::size_t> m_cumulativeFees{0};          /// Sum of all transaction blob sizes.
  Logging::LoggerRef m_logger;

  template <typename _KeyT, typename _ValueT>
  using _hash_map = std::unordered_map<_KeyT, _ValueT>;
  _hash_map<Crypto::KeyImage, Crypto::Hash> m_keyImageReferences;  ///< referenced inputs of all transactions contained
                                                                   ///< in the pool (KeyImage -> TransactionHash)
  _hash_map<Crypto::Hash, std::shared_ptr<PendingTransactionInfo>>
      m_transactions;  ///< general info about transactions in the pool (TransactionHash -> Info)
  _hash_map<PaymentId, std::vector<Crypto::Hash>>
      m_paymentIds;  ///< transactions using a payment id (PaymentId -> TransactionHash[])
  _hash_map<Amount, Crypto::HashSet>
      m_partiallyMixed;  ///< references not fully mixed transactions (Amount -> TransactionHash)

  // Deprecated BEGIN
 public:
  CachedTransaction getTransaction(const Crypto::Hash& hash) const override;
  bool removeTransaction(const Crypto::Hash& hash) override;

  std::vector<Crypto::Hash> getTransactionHashes() const override;
  bool checkIfTransactionPresent(const Crypto::Hash& hash) const override;

  std::vector<CachedTransaction> getPoolTransactions() const override;

  uint64_t getTransactionReceiveTime(const Crypto::Hash& hash) const override;
  std::vector<Crypto::Hash> getTransactionHashesByPaymentId(const PaymentId& paymentId) const override;
};

}  // namespace CryptoNote
