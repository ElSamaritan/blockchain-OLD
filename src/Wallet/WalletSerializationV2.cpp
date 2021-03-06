﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#include "WalletSerializationV2.h"

#include "CryptoNoteCore/CryptoNoteSerialization.h"
#include "Serialization/BinaryInputStreamSerializer.h"
#include "Serialization/BinaryOutputStreamSerializer.h"

using namespace Common;
using namespace Crypto;

namespace {

// DO NOT CHANGE IT
struct UnlockHeightTransactionJobDtoV2 {
  CryptoNote::BlockHeight blockHeight;
  Hash transactionHash;
  Crypto::PublicKey walletSpendPublicKey;
};

// DO NOT CHANGE IT
struct UnlockTimestampTransactionJobDtoV2 {
  uint64_t timestamp;
  Hash transactionHash;
  Crypto::PublicKey walletSpendPublicKey;
};

// DO NOT CHANGE IT
struct WalletTransactionDtoV2 {
  CryptoNote::WalletTransactionState state;
  uint64_t timestamp;
  CryptoNote::BlockHeight blockHeight;
  Hash hash;
  int64_t totalAmount;
  uint64_t fee;
  uint64_t creationTime;
  uint64_t unlockTime;
  CryptoNote::TransactionExtra extra;
  bool isBase;

  WalletTransactionDtoV2() {
  }

  WalletTransactionDtoV2(const CryptoNote::WalletTransaction& wallet) {
    state = wallet.state;
    timestamp = wallet.timestamp;
    blockHeight = wallet.blockHeight;
    hash = wallet.hash;
    totalAmount = wallet.totalAmount;
    fee = wallet.fee;
    creationTime = wallet.creationTime;
    unlockTime = wallet.unlockTime;
    extra = wallet.extra;
    isBase = wallet.isBase;
  }
};

// DO NOT CHANGE IT
struct WalletTransferDtoV2 {
  WalletTransferDtoV2() {
  }

  WalletTransferDtoV2(const CryptoNote::WalletTransfer& tr) {
    address = tr.address;
    amount = tr.amount;
    type = static_cast<uint8_t>(tr.type);
  }

  std::string address;
  uint64_t amount;
  uint8_t type;
};

[[nodiscard]] bool serialize(UnlockHeightTransactionJobDtoV2& value, CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(value.blockHeight, "block_height"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.transactionHash, "transaction_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.walletSpendPublicKey, "wallet_spend_public_key"), false);
  return true;
}

[[nodiscard]] bool serialize(UnlockTimestampTransactionJobDtoV2& value, CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(value.timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.transactionHash, "transaction_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.walletSpendPublicKey, "wallet_spend_public_key"), false);
  return true;
}

[[nodiscard]] bool serialize(WalletTransactionDtoV2& value, CryptoNote::ISerializer& serializer) {
  typedef std::underlying_type<CryptoNote::WalletTransactionState>::type StateType;

  StateType state = static_cast<StateType>(value.state);
  XI_RETURN_EC_IF_NOT(serializer(state, "state"), false);
  value.state = static_cast<CryptoNote::WalletTransactionState>(state);

  XI_RETURN_EC_IF_NOT(serializer(value.timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.blockHeight, "block_height"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.hash, "hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.totalAmount, "total_amount"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.fee, "fee"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.creationTime, "creation_time"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.unlockTime, "unlock_time"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.extra, "extra"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.isBase, "is_base"), false);
  return true;
}

[[nodiscard]] bool serialize(WalletTransferDtoV2& value, CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(value.address, "address"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.amount, "amount"), false);
  XI_RETURN_EC_IF_NOT(serializer(value.type, "type"), false);
  return true;
}

}  // namespace

namespace CryptoNote {

WalletSerializerV2::WalletSerializerV2(ITransfersObserver& transfersObserver, uint64_t& actualBalance,
                                       uint64_t& pendingBalance, WalletsContainer& walletsContainer,
                                       TransfersSyncronizer& synchronizer,
                                       UnlockHeightTransactionJobs& unlockHeightTransactions,
                                       UnlockTimestampTransactionJobs& unlockTimestampTransactions,
                                       WalletTransactions& transactions, WalletTransfers& transfers,
                                       UncommitedTransactions& uncommitedTransactions, std::string& extra,
                                       uint32_t transactionSoftLockTime)
    : m_transfersObserver(transfersObserver),
      m_actualBalance(actualBalance),
      m_pendingBalance(pendingBalance),
      m_walletsContainer(walletsContainer),
      m_synchronizer(synchronizer),
      m_unlockHeightTransactions(unlockHeightTransactions),
      m_unlockTimestampTransactions(unlockTimestampTransactions),
      m_transactions(transactions),
      m_transfers(transfers),
      m_uncommitedTransactions(uncommitedTransactions),
      m_extra(extra),
      m_transactionSoftLockTime(transactionSoftLockTime) {
}

bool WalletSerializerV2::load(Common::IInputStream& source, uint8_t version) {
  if (version != WalletSerializerV2::SERIALIZATION_VERSION)
    throw std::runtime_error{"Unsupported wallet version."};

  CryptoNote::BinaryInputStreamSerializer s(source);

  uint8_t saveLevelValue;
  XI_RETURN_EC_IF_NOT(s(saveLevelValue, "saveLevel"), false);
  WalletSaveLevel saveLevel = static_cast<WalletSaveLevel>(saveLevelValue);

  XI_RETURN_EC_IF_NOT(loadKeyListAndBalances(s, saveLevel == WalletSaveLevel::SAVE_ALL), false);

  if (saveLevel == WalletSaveLevel::SAVE_KEYS_AND_TRANSACTIONS || saveLevel == WalletSaveLevel::SAVE_ALL) {
    XI_RETURN_EC_IF_NOT(loadTransactions(s), false);
    XI_RETURN_EC_IF_NOT(loadTransfers(s), false);
  }

  if (saveLevel == WalletSaveLevel::SAVE_ALL) {
    XI_RETURN_EC_IF_NOT(loadTransfersSynchronizer(s), false);
    XI_RETURN_EC_IF_NOT(loadUnlockTransactionsJobs(s), false);
    XI_RETURN_EC_IF_NOT(s(m_uncommitedTransactions, "uncommitedTransactions"), false);
  }

  XI_RETURN_EC_IF_NOT(s(m_extra, "extra"), false);
  return true;
}

bool WalletSerializerV2::save(Common::IOutputStream& destination, WalletSaveLevel saveLevel) {
  CryptoNote::BinaryOutputStreamSerializer s(destination);

  uint8_t saveLevelValue = static_cast<uint8_t>(saveLevel);
  XI_RETURN_EC_IF_NOT(s(saveLevelValue, "saveLevel"), false);

  XI_RETURN_EC_IF_NOT(saveKeyListAndBalances(s, saveLevel == WalletSaveLevel::SAVE_ALL), false);

  if (saveLevel == WalletSaveLevel::SAVE_KEYS_AND_TRANSACTIONS || saveLevel == WalletSaveLevel::SAVE_ALL) {
    XI_RETURN_EC_IF_NOT(saveTransactions(s), false);
    XI_RETURN_EC_IF_NOT(saveTransfers(s), false);
  }

  if (saveLevel == WalletSaveLevel::SAVE_ALL) {
    XI_RETURN_EC_IF_NOT(saveTransfersSynchronizer(s), false);
    XI_RETURN_EC_IF_NOT(saveUnlockTransactionsJobs(s), false);
    XI_RETURN_EC_IF_NOT(s(m_uncommitedTransactions, "uncommitedTransactions"), false);
  }

  XI_RETURN_EC_IF_NOT(s(m_extra, "extra"), false);
  return true;
}

std::unordered_set<Crypto::PublicKey>& WalletSerializerV2::addedKeys() {
  return m_addedKeys;
}

std::unordered_set<Crypto::PublicKey>& WalletSerializerV2::deletedKeys() {
  return m_deletedKeys;
}

bool WalletSerializerV2::loadKeyListAndBalances(CryptoNote::ISerializer& serializer, bool saveCache) {
  size_t walletCount;
  XI_RETURN_EC_IF_NOT(serializer(walletCount, "walletCount"), false);

  m_actualBalance = 0;
  m_pendingBalance = 0;
  m_deletedKeys.clear();

  std::unordered_set<Crypto::PublicKey> cachedKeySet;
  auto& index = m_walletsContainer.get<KeysIndex>();
  for (size_t i = 0; i < walletCount; ++i) {
    Crypto::PublicKey spendPublicKey;
    uint64_t actualBalance = 0;
    uint64_t pendingBalance = 0;
    XI_RETURN_EC_IF_NOT(serializer(spendPublicKey, "spendPublicKey"), false);

    if (saveCache) {
      XI_RETURN_EC_IF_NOT(serializer(actualBalance, "actualBalance"), false);
      XI_RETURN_EC_IF_NOT(serializer(pendingBalance, "pendingBalance"), false);
    }

    cachedKeySet.insert(spendPublicKey);

    auto it = index.find(spendPublicKey);
    if (it == index.end()) {
      m_deletedKeys.emplace(std::move(spendPublicKey));
    } else if (saveCache) {
      m_actualBalance += actualBalance;
      m_pendingBalance += pendingBalance;

      index.modify(it, [actualBalance, pendingBalance](WalletRecord& wallet) {
        wallet.actualBalance = actualBalance;
        wallet.pendingBalance = pendingBalance;
      });
    }
  }

  for (auto wallet : index) {
    if (cachedKeySet.count(wallet.spendPublicKey) == 0) {
      m_addedKeys.insert(wallet.spendPublicKey);
    }
  }
  return true;
}

bool WalletSerializerV2::saveKeyListAndBalances(CryptoNote::ISerializer& serializer, bool saveCache) {
  auto walletCount = m_walletsContainer.get<RandomAccessIndex>().size();
  XI_RETURN_EC_IF_NOT(serializer(walletCount, "walletCount"), false);
  for (auto wallet : m_walletsContainer.get<RandomAccessIndex>()) {
    XI_RETURN_EC_IF_NOT(serializer(wallet.spendPublicKey, "spendPublicKey"), false);

    if (saveCache) {
      XI_RETURN_EC_IF_NOT(serializer(wallet.actualBalance, "actualBalance"), false);
      XI_RETURN_EC_IF_NOT(serializer(wallet.pendingBalance, "pendingBalance"), false);
    }
  }
  return true;
}

bool WalletSerializerV2::loadTransactions(CryptoNote::ISerializer& serializer) {
  uint64_t count = 0;
  XI_RETURN_EC_IF_NOT(serializer(count, "transactionCount"), false);

  m_transactions.get<RandomAccessIndex>().reserve(count);

  for (uint64_t i = 0; i < count; ++i) {
    WalletTransactionDtoV2 dto;
    XI_RETURN_EC_IF_NOT(serializer(dto, "transaction"), false);

    WalletTransaction tx;
    tx.state = dto.state;
    tx.timestamp = dto.timestamp;
    tx.blockHeight = dto.blockHeight;
    tx.hash = dto.hash;
    tx.totalAmount = dto.totalAmount;
    tx.fee = dto.fee;
    tx.creationTime = dto.creationTime;
    tx.unlockTime = dto.unlockTime;
    tx.extra = dto.extra;
    tx.isBase = dto.isBase;

    m_transactions.get<RandomAccessIndex>().emplace_back(std::move(tx));
  }
  return true;
}

bool WalletSerializerV2::saveTransactions(CryptoNote::ISerializer& serializer) {
  uint64_t count = m_transactions.size();
  XI_RETURN_EC_IF_NOT(serializer(count, "transactionCount"), false);

  for (const auto& tx : m_transactions) {
    WalletTransactionDtoV2 dto(tx);
    XI_RETURN_EC_IF_NOT(serializer(dto, "transaction"), false);
  }
  return true;
}

bool WalletSerializerV2::loadTransfers(CryptoNote::ISerializer& serializer) {
  uint64_t count = 0;
  XI_RETURN_EC_IF_NOT(serializer(count, "transferCount"), false);

  m_transfers.reserve(count);

  for (uint64_t i = 0; i < count; ++i) {
    uint64_t txId = 0;
    XI_RETURN_EC_IF_NOT(serializer(txId, "transactionId"), false);

    WalletTransferDtoV2 dto;
    XI_RETURN_EC_IF_NOT(serializer(dto, "transfer"), false);

    WalletTransfer tr;
    tr.address = dto.address;
    tr.amount = dto.amount;
    tr.type = static_cast<WalletTransferType>(dto.type);

    m_transfers.emplace_back(std::piecewise_construct, std::forward_as_tuple(txId),
                             std::forward_as_tuple(std::move(tr)));
  }
  return true;
}

bool WalletSerializerV2::saveTransfers(CryptoNote::ISerializer& serializer) {
  uint64_t count = m_transfers.size();
  XI_RETURN_EC_IF_NOT(serializer(count, "transferCount"), false);

  for (const auto& kv : m_transfers) {
    uint64_t txId = kv.first;

    WalletTransferDtoV2 tr(kv.second);

    XI_RETURN_EC_IF_NOT(serializer(txId, "transactionId"), false);
    XI_RETURN_EC_IF_NOT(serializer(tr, "transfer"), false);
  }
  return true;
}

bool WalletSerializerV2::loadTransfersSynchronizer(CryptoNote::ISerializer& serializer) {
  std::string transfersSynchronizerData;
  XI_RETURN_EC_IF_NOT(serializer(transfersSynchronizerData, "transfersSynchronizer"), false);

  std::stringstream stream(transfersSynchronizerData);
  XI_RETURN_EC_IF_NOT(m_synchronizer.load(stream), false);
  return true;
}

bool WalletSerializerV2::saveTransfersSynchronizer(CryptoNote::ISerializer& serializer) {
  std::stringstream stream;
  XI_RETURN_EC_IF_NOT(m_synchronizer.save(stream), false);
  stream.flush();

  std::string transfersSynchronizerData = stream.str();
  XI_RETURN_EC_IF_NOT(serializer(transfersSynchronizerData, "transfersSynchronizer"), false);
  return true;
}

bool WalletSerializerV2::loadUnlockTransactionsJobs(CryptoNote::ISerializer& serializer) {
  {
    auto& index = m_unlockHeightTransactions.get<TransactionHashIndex>();
    auto& walletsIndex = m_walletsContainer.get<KeysIndex>();

    size_t jobsCount = 0;
    XI_RETURN_EC_IF_NOT(serializer.beginArray(jobsCount, "unlockHeightTransactionsJobsCount"), false);

    for (uint64_t i = 0; i < jobsCount; ++i) {
      UnlockHeightTransactionJobDtoV2 dto;
      XI_RETURN_EC_IF_NOT(serializer(dto, "unlockHeightTransactionsJob"), false);

      auto walletIt = walletsIndex.find(dto.walletSpendPublicKey);
      if (walletIt != walletsIndex.end()) {
        UnlockHeightTransactionJob job;
        job.blockHeight = dto.blockHeight;
        job.transactionHash = dto.transactionHash;
        job.container = walletIt->container;

        index.emplace(std::move(job));
      }
    }

    XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  }
  {
    auto& index = m_unlockTimestampTransactions.get<TransactionHashIndex>();
    auto& walletsIndex = m_walletsContainer.get<KeysIndex>();

    size_t jobsCount = 0;
    XI_RETURN_EC_IF_NOT(serializer.beginArray(jobsCount, "unlockTimestampTransactionsJobsCount"), false);

    for (uint64_t i = 0; i < jobsCount; ++i) {
      UnlockTimestampTransactionJobDtoV2 dto;
      XI_RETURN_EC_IF_NOT(serializer(dto, "unlockTimestampTransactionsJob"), false);

      auto walletIt = walletsIndex.find(dto.walletSpendPublicKey);
      if (walletIt != walletsIndex.end()) {
        UnlockTimestampTransactionJob job;
        job.timestamp = dto.timestamp;
        job.transactionHash = dto.transactionHash;
        job.container = walletIt->container;

        index.emplace(std::move(job));
      }
    }

    XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  }
  return true;
}

bool WalletSerializerV2::saveUnlockTransactionsJobs(CryptoNote::ISerializer& serializer) {
  {
    auto& index = m_unlockHeightTransactions.get<TransactionHashIndex>();
    auto& wallets = m_walletsContainer.get<TransfersContainerIndex>();

    size_t jobsCount = index.size();
    XI_RETURN_EC_IF_NOT(serializer.beginArray(jobsCount, "unlockHeightTransactionsJobsCount"), false);

    for (const auto& j : index) {
      auto containerIt = wallets.find(j.container);
      assert(containerIt != wallets.end());

      auto keyIt = m_walletsContainer.project<KeysIndex>(containerIt);
      assert(keyIt != m_walletsContainer.get<KeysIndex>().end());

      UnlockHeightTransactionJobDtoV2 dto;
      dto.blockHeight = j.blockHeight;
      dto.transactionHash = j.transactionHash;
      dto.walletSpendPublicKey = keyIt->spendPublicKey;

      XI_RETURN_EC_IF_NOT(serializer(dto, "unlockHeightTransactionsJob"), false);
    }

    XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  }
  {
    auto& index = m_unlockTimestampTransactions.get<TransactionHashIndex>();
    auto& wallets = m_walletsContainer.get<TransfersContainerIndex>();

    size_t jobsCount = index.size();
    XI_RETURN_EC_IF_NOT(serializer.beginArray(jobsCount, "unlockTimestampTransactionsJobsCount"), false);

    for (const auto& j : index) {
      auto containerIt = wallets.find(j.container);
      assert(containerIt != wallets.end());

      auto keyIt = m_walletsContainer.project<KeysIndex>(containerIt);
      assert(keyIt != m_walletsContainer.get<KeysIndex>().end());

      UnlockTimestampTransactionJobDtoV2 dto;
      dto.timestamp = j.timestamp;
      dto.transactionHash = j.transactionHash;
      dto.walletSpendPublicKey = keyIt->spendPublicKey;

      XI_RETURN_EC_IF_NOT(serializer(dto, "unlockTimestampTransactionsJob"), false);
    }

    XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  }
  return true;
}

}  // namespace CryptoNote
