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

#pragma once

#include <Xi/Global.hh>

#include "IWalletLegacy.h"
#include "Common/ObserverManager.h"

namespace CryptoNote {

class WalletLegacyEvent {
 public:
  virtual ~WalletLegacyEvent() = default;

  virtual void notify(Tools::ObserverManager<CryptoNote::IWalletLegacyObserver>& observer) = 0;
};

class WalletTransactionUpdatedEvent : public WalletLegacyEvent {
 public:
  WalletTransactionUpdatedEvent(TransactionId transactionId) : m_id(transactionId) {
  }
  virtual ~WalletTransactionUpdatedEvent() override = default;

  virtual void notify(Tools::ObserverManager<CryptoNote::IWalletLegacyObserver>& observer) override {
    observer.notify(&IWalletLegacyObserver::transactionUpdated, m_id);
  }

 private:
  TransactionId m_id;
};

class WalletSendTransactionCompletedEvent : public WalletLegacyEvent {
 public:
  WalletSendTransactionCompletedEvent(TransactionId transactionId, std::error_code result)
      : m_id(transactionId), m_error(result) {
  }
  virtual ~WalletSendTransactionCompletedEvent() override = default;

  virtual void notify(Tools::ObserverManager<CryptoNote::IWalletLegacyObserver>& observer) override {
    observer.notify(&IWalletLegacyObserver::sendTransactionCompleted, m_id, m_error);
  }

 private:
  TransactionId m_id;
  std::error_code m_error;
};

class WalletExternalTransactionCreatedEvent : public WalletLegacyEvent {
 public:
  WalletExternalTransactionCreatedEvent(TransactionId transactionId) : m_id(transactionId) {
  }
  virtual ~WalletExternalTransactionCreatedEvent() override = default;

  virtual void notify(Tools::ObserverManager<CryptoNote::IWalletLegacyObserver>& observer) override {
    observer.notify(&IWalletLegacyObserver::externalTransactionCreated, m_id);
  }

 private:
  TransactionId m_id;
};

class WalletSynchronizationProgressUpdatedEvent : public WalletLegacyEvent {
 public:
  WalletSynchronizationProgressUpdatedEvent(uint32_t current, uint32_t total) : m_current(current), m_total(total) {
  }
  virtual ~WalletSynchronizationProgressUpdatedEvent() override = default;

  virtual void notify(Tools::ObserverManager<CryptoNote::IWalletLegacyObserver>& observer) override {
    observer.notify(&IWalletLegacyObserver::synchronizationProgressUpdated, m_current, m_total);
  }

 private:
  uint32_t m_current;
  uint32_t m_total;
};

class WalletSynchronizationCompletedEvent : public WalletLegacyEvent {
 public:
  WalletSynchronizationCompletedEvent(uint32_t current, uint32_t total, std::error_code result) : m_ec(result) {
    XI_UNUSED(current, total);
  }
  virtual ~WalletSynchronizationCompletedEvent() override = default;

  virtual void notify(Tools::ObserverManager<CryptoNote::IWalletLegacyObserver>& observer) override {
    observer.notify(&IWalletLegacyObserver::synchronizationCompleted, m_ec);
  }

 private:
  std::error_code m_ec;
};

class WalletActualBalanceUpdatedEvent : public WalletLegacyEvent {
 public:
  WalletActualBalanceUpdatedEvent(uint64_t balance) : m_balance(balance) {
  }
  virtual ~WalletActualBalanceUpdatedEvent() override = default;

  virtual void notify(Tools::ObserverManager<CryptoNote::IWalletLegacyObserver>& observer) override {
    observer.notify(&IWalletLegacyObserver::actualBalanceUpdated, m_balance);
  }

 private:
  uint64_t m_balance;
};

class WalletPendingBalanceUpdatedEvent : public WalletLegacyEvent {
 public:
  WalletPendingBalanceUpdatedEvent(uint64_t balance) : m_balance(balance) {
  }
  virtual ~WalletPendingBalanceUpdatedEvent() override = default;

  virtual void notify(Tools::ObserverManager<CryptoNote::IWalletLegacyObserver>& observer) override {
    observer.notify(&IWalletLegacyObserver::pendingBalanceUpdated, m_balance);
  }

 private:
  uint64_t m_balance;
};

} /* namespace CryptoNote */
