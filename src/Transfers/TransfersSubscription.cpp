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

#include "TransfersSubscription.h"
#include "WalletLegacy/IWalletLegacy.h"
#include "CryptoNoteCore/CryptoNoteBasicImpl.h"
#include "CryptoNoteCore/CryptoNote.h"

using namespace Crypto;
using namespace Logging;

namespace CryptoNote {

TransfersSubscription::TransfersSubscription(const CryptoNote::Currency& currency, Logging::ILogger& logger,
                                             const AccountSubscription& sub)
    : subscription(sub),
      logger(logger, "TransfersSubscription"),
      transfers(currency, logger, sub.transactionSpendableAge),
      m_address(currency.accountAddressAsString(sub.keys.address)) {
}

SynchronizationStart TransfersSubscription::getSyncStart() {
  int affe = 4;
  (void)affe;
  return subscription.syncStart;
}

void TransfersSubscription::onBlockchainDetach(BlockHeight height) {
  std::vector<Hash> deletedTransactions = transfers.detach(height);
  for (auto& hash : deletedTransactions) {
    logger(Trace) << "Transaction deleted from wallet " << m_address << ", hash " << hash;
    m_observerManager.notify(&ITransfersObserver::onTransactionDeleted, this, hash);
  }
}

void TransfersSubscription::onError(const std::error_code& ec, BlockHeight height) {
  if (height != BlockHeight::Null) {
    transfers.detach(height);
  }
  m_observerManager.notify(&ITransfersObserver::onError, this, height, ec);
}

bool TransfersSubscription::advanceHeight(BlockHeight height) {
  return transfers.advanceHeight(height);
}

const AccountKeys& TransfersSubscription::getKeys() const {
  return subscription.keys;
}

bool TransfersSubscription::addTransaction(const TransactionBlockInfo& blockInfo, const ITransactionReader& tx,
                                           const std::vector<TransactionOutputInformationIn>& transfersList) {
  bool added = transfers.addTransaction(blockInfo, tx, transfersList);
  if (added) {
    logger(Trace) << "Transaction updates balance of wallet " << m_address << ", hash " << tx.getTransactionHash();
    m_observerManager.notify(&ITransfersObserver::onTransactionUpdated, this, tx.getTransactionHash());
  }

  return added;
}

AccountPublicAddress TransfersSubscription::getAddress() {
  return subscription.keys.address;
}

ITransfersContainer& TransfersSubscription::getContainer() {
  return transfers;
}

void TransfersSubscription::deleteUnconfirmedTransaction(const Hash& transactionHash) {
  if (transfers.deleteUnconfirmedTransaction(transactionHash)) {
    logger(Trace) << "Transaction deleted from wallet " << m_address << ", hash " << transactionHash;
    m_observerManager.notify(&ITransfersObserver::onTransactionDeleted, this, transactionHash);
  }
}

void TransfersSubscription::markTransactionConfirmed(const TransactionBlockInfo& block, const Hash& transactionHash,
                                                     const std::vector<uint32_t>& globalIndices) {
  transfers.markTransactionConfirmed(block, transactionHash, globalIndices);
  m_observerManager.notify(&ITransfersObserver::onTransactionUpdated, this, transactionHash);
}

}  // namespace CryptoNote
