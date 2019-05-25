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

#include <CryptoNoteCore/Transactions/TransactionPoolMessages.h>

#include <utility>

namespace CryptoNote {

TransactionPoolMessage::TransactionPoolMessage(AddTransaction at)
    : type(TransactionMessageType::AddTransactionType), data(std::move(at)) {}

TransactionPoolMessage::TransactionPoolMessage(DeleteTransaction dt)
    : type(TransactionMessageType::DeleteTransactionType), data(std::move(dt)) {}

// pattern match
void TransactionPoolMessage::match(std::function<void(const AddTransaction&)>&& addTxVisitor,
                                   std::function<void(const DeleteTransaction&)>&& delTxVisitor) {
  switch (getType()) {
    case TransactionMessageType::AddTransactionType:
      addTxVisitor(std::get<AddTransaction>(data));
      break;
    case TransactionMessageType::DeleteTransactionType:
      delTxVisitor(std::get<DeleteTransaction>(data));
      break;
  }
}

// API with explicit type handling
TransactionMessageType TransactionPoolMessage::getType() const { return type; }

const AddTransaction& TransactionPoolMessage::getAddTransaction() const {
  assert(getType() == TransactionMessageType::AddTransactionType);
  return std::get<AddTransaction>(data);
}

const DeleteTransaction& TransactionPoolMessage::getDeleteTransaction() const {
  assert(getType() == TransactionMessageType::DeleteTransactionType);
  return std::get<DeleteTransaction>(data);
}

TransactionPoolMessage makeAddTransaction(const Crypto::Hash& hash) {
  return TransactionPoolMessage{AddTransaction{hash}};
}

TransactionPoolMessage makeDelTransaction(const Crypto::Hash& hash) {
  return TransactionPoolMessage{DeleteTransaction{hash}};
}

AddTransaction::AddTransaction(Crypto::Hash _hash) : hash{std::move(_hash)} {}

DeleteTransaction::DeleteTransaction(Crypto::Hash _hash) : hash{std::move(_hash)} {}

}  // namespace CryptoNote
