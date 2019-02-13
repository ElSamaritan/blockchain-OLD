/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include "CryptoNoteCore/Transactions/TransactionPool.h"

#include <algorithm>

#include <Common/int-util.h>
#include <Common/StringTools.h>
#include <crypto/CryptoTypes.h>
#include <crypto/hash.h>
#include <Serialization/SerializationTools.h>

#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"
#include "CryptoNoteCore/Transactions/TransactionUtils.h"
#include "CryptoNoteCore/Transactions/TransactionPoolErrors.h"
#include "CryptoNoteCore/Transactions/TransactionValidationErrors.h"
#include "CryptoNoteCore/Transactions/TransactionPriortiyComparator.h"
#include "CryptoNoteCore/Transactions/PoolTransactionValidator.h"
#include "CryptoNoteCore/Transactions/ITransactionPoolObserver.h"

using Addition = CryptoNote::ITransactionPoolObserver::AdditionReason;
using Deletion = CryptoNote::ITransactionPoolObserver::DeletionReason;
using Error = CryptoNote::error::TransactionPoolError;

namespace CryptoNote {

TransactionPool::TransactionPool(IBlockchain& blockchain, Logging::ILogger& logger)
    : m_blockchain{blockchain}, m_observers{}, m_access{}, m_logger(logger, "txpool"), m_keyImageReferences{} {
  blockchain.addObserver(this);
}

TransactionPool::~TransactionPool() {
  m_observers.clear();
  m_blockchain.removeObserver(this);
}

void TransactionPool::addObserver(ITransactionPoolObserver* observer) { m_observers.add(observer); }
void TransactionPool::removeObserver(ITransactionPoolObserver* observer) { m_observers.remove(observer); }

std::size_t TransactionPool::size() const {
  XI_CONCURRENT_RLOCK(m_access);
  return m_transactions.size();
}

Crypto::Hash TransactionPool::stateHash() const {
  XI_CONCURRENT_RLOCK(m_access);
  if (!m_stateHash.is_initialized()) {
    m_stateHash = computeStateHash();
  }
  return m_stateHash.get();
}

size_t TransactionPool::forceFlush() {
  XI_CONCURRENT_RLOCK(m_access);
  size_t count = 0;
  const auto hashes = getTransactionHashes();
  for (const auto& iTxHash : hashes) {
    if (removeTransaction(iTxHash, Deletion::Forced)) {
      count += 1;
    }
  }
  return count;
}

bool TransactionPool::forceErasure(const Crypto::Hash& hash) {
  XI_CONCURRENT_RLOCK(m_access);
  return removeTransaction(hash, Deletion::Forced);
}

Xi::Result<void> TransactionPool::pushTransaction(BinaryArray transactionBlob) {
  Transaction transaction;
  if (!fromBinaryArray<Transaction>(transaction, transactionBlob)) {
    return Xi::make_error(error::TransactionValidationError::INVALID_BINARY_REPRESNETATION);
  }
  return pushTransaction(std::move(transaction));
}

Xi::Result<void> TransactionPool::pushTransaction(Transaction transaction) {
  if (!m_blockchain.isInitialized()) {
    return Xi::make_error(Error::BLOCKCHAIN_UNINITIALIZED);
  }
  XI_CONCURRENT_RLOCK(m_access);
  return insertTransaction(std::move(transaction), Addition::Incoming);
}

bool TransactionPool::containsTransaction(const Crypto::Hash& hash) const {
  XI_CONCURRENT_RLOCK(m_access);
  return m_transactions.find(hash) != m_transactions.end();
}

bool TransactionPool::containsKeyImage(const Crypto::KeyImage& keyImage) const {
  XI_CONCURRENT_RLOCK(m_access);
  return m_keyImageReferences.find(keyImage) != m_keyImageReferences.end();
}

void TransactionPool::serialize(ISerializer& serializer) {
  std::vector<std::string> transactionBlobs;
  std::vector<PosixTimestamp> transactionTimestamps;
  if (serializer.type() == ISerializer::OUTPUT) {
    XI_CONCURRENT_RLOCK(m_access);
    transactionBlobs.reserve(m_transactions.size());
    for (const auto& iinfo : m_transactions) {
      if (iinfo.second.get() == nullptr) {
        m_logger(Logging::ERROR) << "nullptr transaction detected in pool";
      } else {
        transactionBlobs.push_back(Common::toHex(iinfo.second->transaction().getTransactionBinaryArray()));
        transactionTimestamps.push_back(iinfo.second->receiveTime());
      }
    }
  }

  serializeContainer(transactionBlobs, "transaction_blobs", serializer);
  serializeContainer(transactionTimestamps, "transaction_timestamps", serializer);

  if (serializer.type() == ISerializer::INPUT) {
    std::vector<BinaryArray> rawTransactions;
    rawTransactions.reserve(transactionBlobs.size());
    std::transform(transactionBlobs.begin(), transactionBlobs.end(), std::back_inserter(rawTransactions),
                   [](const auto& blob) { return Common::fromHex(blob); });
    {
      XI_CONCURRENT_RLOCK(m_access);
      for (size_t i = 0; i < rawTransactions.size(); ++i) {
        Transaction iTransaction;
        if (!fromBinaryArray(iTransaction, rawTransactions[i])) {
          m_logger(Logging::ERROR) << "failed to deserialize transaction";
        } else {
          auto insertionResult = insertTransaction(CachedTransaction{std::move(iTransaction)}, transactionTimestamps[i],
                                                   Addition::Deserialization);
          if (insertionResult.isError()) {
            m_logger(Logging::ERROR) << "failed to push deserialized transantion";
          }
        }
      }
    }
  }
}

ITransactionPool::TransactionQueryResult TransactionPool::queryTransaction(const Crypto::Hash& hash) const {
  XI_CONCURRENT_RLOCK(m_access);
  auto search = m_transactions.find(hash);
  if (search == m_transactions.end()) {
    return nullptr;
  } else if (search->second.get() == nullptr) {
    m_logger(Logging::ERROR) << "nullptr transaction detected in pool";
    return nullptr;
  } else {
    return search->second;
  }
}

std::vector<CachedTransaction> TransactionPool::eligiblePoolTransactions(
    TransactionValidationResult::EligibleIndex index) const {
  std::vector<PendingTransactionInfo> pending;
  {
    XI_CONCURRENT_RLOCK(m_access);
    pending.reserve(size());
    std::transform(m_transactions.begin(), m_transactions.end(), std::back_inserter(pending),
                   [](const auto& iTx) { return *iTx.second; });
  }
  std::sort(pending.begin(), pending.end(), TransactionPriorityComparator{});
  std::vector<CachedTransaction> result;
  result.reserve(pending.size());
  for (const auto& iPending : pending) {
    if (iPending.eligibleIndex().isSatisfiedByIndex(index)) {
      result.emplace_back(iPending.transaction());
    }
  }
  return result;
}

Xi::Concurrent::RecursiveLock::lock_t TransactionPool::acquireExclusiveAccess() const {
  return Xi::Concurrent::RecursiveLock::lock_t{m_access};
}

void TransactionPool::blockAdded(uint32_t index, const Crypto::Hash&) {
  XI_CONCURRENT_RLOCK(m_access);
  auto mainChain = m_blockchain.mainChain();
  if (mainChain == nullptr) {
    m_logger(Logging::ERROR) << "Unable to add block, main chain missing.";
    return;
  }
  pushBlock(mainChain->getBlockByIndex(index));
  evaluateBlockVersionUpgradeConstraints();
}

void TransactionPool::mainChainSwitched(const IBlockchainCache& previous, const IBlockchainCache& current,
                                        uint32_t commonRootIndex) {
  XI_CONCURRENT_RLOCK(m_access);
  m_logger(Logging::TRACE) << "Starting main chain switch";
  for (uint32_t i = previous.getTopBlockIndex(); i > commonRootIndex; --i) {
    popBlock(previous.getBlockByIndex(i));
  }
  for (uint32_t i = commonRootIndex + 1; i <= current.getTopBlockIndex(); ++i) {
    pushBlock(current.getBlockByIndex(i));
  }
  evaluateBlockVersionUpgradeConstraints();
  m_logger(Logging::TRACE) << "Main chain switch finished";
}

void TransactionPool::pushBlock(RawBlock block) {
  m_logger(Logging::TRACE) << "Processing incoming block";
  for (auto& transactionBlob : block.transactions) {
    pushBlockTransaction(std::move(transactionBlob));
  }
}

void TransactionPool::popBlock(RawBlock block) {
  m_logger(Logging::TRACE) << "Processing block reversal";
  for (auto& transactionBlob : block.transactions) {
    popBlockTransaction(std::move(transactionBlob));
  }
}

void TransactionPool::pushBlockTransaction(BinaryArray transactionBlob) {
  auto transaction = CachedTransaction::fromBinaryArray(transactionBlob);
  if (transaction.isError()) {
    m_logger(Logging::ERROR) << "Failed to deserialized block transaction: " << transaction.error().message();
  }
  const auto transactionHash = transaction.value().getTransactionHash();
  if (!removeTransaction(transactionHash, Deletion::AddedToMainChain)) {
    m_logger(Logging::DEBUGGING) << "Failed to remove pushed block transaction: "
                                 << Common::toHex(transactionHash.data, sizeof(decltype(transactionHash)));
  }
  for (const auto& keyImage : transaction.value().getKeyImages()) {
    auto keyImageSearch = m_keyImageReferences.find(keyImage);
    if (keyImageSearch != m_keyImageReferences.end()) {
      removeTransaction(keyImageSearch->second, Deletion::KeyImageUsedInMainChain);
    }
  }
}

void TransactionPool::popBlockTransaction(BinaryArray transactionBlob) {
  Transaction transaction;
  if (!fromBinaryArray(transaction, std::move(transactionBlob))) {
    m_logger(Logging::ERROR) << "Invalid encoded transaction blob poped.";
    return;
  }
  auto insertionResult = insertTransaction(std::move(transaction), Addition::MainChainSwitch);
  if (insertionResult.isError()) {
    m_logger(Logging::DEBUGGING)
        << "Transaction from alternative chain could not be recovered because it is invalid on the new main chain.\n\t"
        << insertionResult.error().message();
  }
}

bool TransactionPool::removeTransaction(const Crypto::Hash& hash, ITransactionPoolObserver::DeletionReason reason) {
  auto search = m_transactions.find(hash);
  if (search == m_transactions.end()) {
    return false;
  } else {
    if (search->second.get() != nullptr) {
      const auto& nfo = *search->second;
      for (const auto& keyImage : nfo.transaction().getKeyImages()) {
        auto keyImageSearch = m_keyImageReferences.find(keyImage);
        if (keyImageSearch != m_keyImageReferences.end()) {
          m_keyImageReferences.erase(keyImageSearch->first);
        }
      }
      auto paymentId = nfo.transaction().getPaymentId();
      if (paymentId.has_value()) {
        auto paymentIdSearch = m_paymentIds.find(*paymentId);
        if (paymentIdSearch != m_paymentIds.end()) {
          auto& pidRefs = paymentIdSearch->second;
          pidRefs.erase(std::remove(pidRefs.begin(), pidRefs.end(), *paymentId), pidRefs.end());
          if (paymentIdSearch->second.empty()) {
            m_paymentIds.erase(paymentIdSearch);
          }
        }
      }
    }
    m_transactions.erase(search);
    invalidateStateHash();
    m_observers.notify(&ITransactionPoolObserver::transactionDeletedFromPool, std::cref(hash), reason);
    return true;
  }
}

Xi::Result<TransactionValidationResult::EligibleIndex> TransactionPool::currentEligibleIndex() const {
  const auto mainChain = m_blockchain.mainChain();
  if (mainChain == nullptr) {
    return Xi::make_error(Error::MAIN_CHAIN_MISSING);
  }
  const auto timestamp = m_blockchain.timeProvider().posixNow();
  if (timestamp.isError()) {
    return timestamp.error();
  }
  return Xi::make_result<TransactionValidationResult::EligibleIndex>(mainChain->getTopBlockIndex() + 1,
                                                                     timestamp.value());
}

Xi::Result<void> TransactionPool::insertTransaction(Transaction transaction,
                                                    ITransactionPoolObserver::AdditionReason reason) {
  return insertTransaction(CachedTransaction{transaction}, reason);
}

Xi::Result<void> TransactionPool::insertTransaction(CachedTransaction transaction,
                                                    ITransactionPoolObserver::AdditionReason reason) {
  auto receiveTime = m_blockchain.timeProvider().posixNow();
  if (receiveTime.isError()) {
    return receiveTime.error();
  } else {
    return insertTransaction(std::move(transaction), receiveTime.value(), reason);
  }
}

Xi::Result<void> TransactionPool::insertTransaction(CachedTransaction transaction, PosixTimestamp receiveTime,
                                                    ITransactionPoolObserver::AdditionReason reason) {
  auto mainChain = m_blockchain.mainChain();
  if (mainChain == nullptr) return Xi::make_error(Error::MAIN_CHAIN_MISSING);
  if (mainChain->hasTransaction(transaction.getTransactionHash())) return Xi::make_error(Error::ALREADY_MINED);
  const uint8_t blockVersion = m_blockchain.upgradeManager().getBlockMajorVersion(mainChain->getTopBlockIndex() + 1);
  PoolTransactionValidator validator{*this, blockVersion, *mainChain, m_blockchain.currency()};
  auto validationResult = validator.validate(transaction);
  if (validationResult.isError()) {
    return validationResult.error();
  } else {
    auto validation = validationResult.take();
    auto eligibleIndex = currentEligibleIndex();
    if (eligibleIndex.isError()) {
      return eligibleIndex.error();
    } else if (!validation.eligibleIndex().isSatisfiedByIndex(eligibleIndex.value())) {
      return Xi::make_error(Error::INPUT_UNLOCKS_TOO_FAR_IN_FUTURE);
    }
    invalidateStateHash();
    auto transactionHash = transaction.getTransactionHash();
    if (transaction.getPaymentId()) {
      m_paymentIds[*transaction.getPaymentId()].push_back(transactionHash);
    }
    for (const auto& keyImage : transaction.getKeyImages()) {
      m_keyImageReferences.insert(std::make_pair(keyImage, transactionHash));
    }
    auto nfo =
        std::make_shared<PendingTransactionInfo>(std::move(transaction), validation.eligibleIndex(), receiveTime);
    m_transactions.insert(std::make_pair(transactionHash, nfo));
    m_logger(Logging::INFO) << "transaction added to pool";
    m_observers.notify(&ITransactionPoolObserver::transactionAddedToPool, std::cref(transactionHash), reason);
    return Xi::make_result<void>();
  }
}

Crypto::Hash TransactionPool::computeStateHash() const {
  std::vector<Crypto::Hash> transactionHashes{};
  transactionHashes.reserve(size());
  std::transform(m_transactions.begin(), m_transactions.end(), std::back_inserter(transactionHashes),
                 [](const auto& transaction) { return transaction.second->transaction().getTransactionHash(); });
  std::sort(transactionHashes.begin(), transactionHashes.end());
  Crypto::Hash hash{};
  std::memset(hash.data, 0, sizeof(Crypto::Hash));
  if (transactionHashes.empty()) {
    return hash;
  } else if (transactionHashes.size() < 2) {
    return transactionHashes[0];
  } else {
    Crypto::tree_hash(transactionHashes.data(), transactionHashes.size(), hash);
    return hash;
  }
}

void TransactionPool::invalidateStateHash() { m_stateHash = boost::none; }

void TransactionPool::evaluateBlockVersionUpgradeConstraints() {
  XI_CONCURRENT_RLOCK(m_access);

  const auto eligibleIndex = currentEligibleIndex();
  if (eligibleIndex.isError()) {
    m_logger(Logging::ERROR) << "Unable to retrieve eligible index: " << eligibleIndex.error().message();
  }
  uint8_t version = m_blockchain.upgradeManager().getBlockMajorVersion(eligibleIndex.value().Height);
  if (m_eligibleBlockVersion.has_value() && m_eligibleBlockVersion.value() == version) {
    return;
  }

  auto mainChain = m_blockchain.mainChain();
  if (mainChain == nullptr) {
    m_logger(Logging::ERROR) << "Unable to update constraints for a new block version due to missing main chain.";
    return;
  }

  std::vector<Crypto::Hash> transactionOutdated{};
  PoolTransactionValidator validator{*this, version, *mainChain, m_blockchain.currency()};
  for (auto it = m_transactions.begin(); it != m_transactions.end(); ++it) {
    if (it->second.get() == nullptr) {
      m_logger(Logging::DEBUGGING) << "nullptr transaction detected in pool";
      continue;
    }
    const auto& pendingTx = *it->second;
    auto validationResult = validator.updateValidation(pendingTx.transaction());
    if (validationResult.isError()) {
      transactionOutdated.emplace_back(pendingTx.transaction().getTransactionHash());
    }
  }
  for (const auto& hash : transactionOutdated) {
    removeTransaction(hash, Deletion::BlockMajorVersionUpgrade);
  }

  m_eligibleBlockVersion = version;
}

CachedTransaction TransactionPool::getTransaction(const Crypto::Hash& hash) const {
  return queryTransaction(hash)->transaction();
}

bool TransactionPool::removeTransaction(const Crypto::Hash& hash) {
  XI_CONCURRENT_RLOCK(m_access);
  auto query = queryTransaction(hash);
  if (!query) {
    return false;
  } else {
    removeTransaction(hash, Deletion::PoolCleanupProcedure);
    return true;
  }
}

std::vector<Crypto::Hash> TransactionPool::getTransactionHashes() const {
  XI_CONCURRENT_RLOCK(m_access);
  std::vector<Crypto::Hash> hashes;
  hashes.reserve(m_transactions.size());
  std::transform(m_transactions.begin(), m_transactions.end(), std::back_inserter(hashes),
                 [](const auto& iTx) { return iTx.second->transaction().getTransactionHash(); });
  return hashes;
}

bool TransactionPool::checkIfTransactionPresent(const Crypto::Hash& hash) const { return containsTransaction(hash); }

std::vector<CachedTransaction> TransactionPool::getPoolTransactions() const {
  XI_CONCURRENT_RLOCK(m_access);
  std::vector<CachedTransaction> result;
  result.reserve(size());
  std::transform(m_transactions.begin(), m_transactions.end(), std::back_inserter(result),
                 [](const auto& iTx) { return iTx.second->transaction(); });
  return result;
}

uint64_t TransactionPool::getTransactionReceiveTime(const Crypto::Hash& hash) const {
  XI_CONCURRENT_RLOCK(m_access);
  auto it = queryTransaction(hash);
  assert(it);
  return it->receiveTime();
}

std::vector<Crypto::Hash> TransactionPool::getTransactionHashesByPaymentId(const Crypto::Hash& paymentId) const {
  XI_CONCURRENT_RLOCK(m_access);
  auto search = m_paymentIds.find(paymentId);
  if (search == m_paymentIds.end()) {
    return std::vector<Crypto::Hash>{};
  } else {
    return search->second;
  }
}

}  // namespace CryptoNote
