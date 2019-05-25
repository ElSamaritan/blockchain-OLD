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

#include <functional>
#include <variant>

#include "CryptoNoteCore/CryptoNote.h"

namespace CryptoNote {

enum class TransactionMessageType { AddTransactionType, DeleteTransactionType };

// immutable messages
struct AddTransaction {
  Crypto::Hash hash;

  explicit AddTransaction(Crypto::Hash hash);
};

struct DeleteTransaction {
  Crypto::Hash hash;

  explicit DeleteTransaction(Crypto::Hash hash);
};

class TransactionPoolMessage {
 public:
  TransactionPoolMessage(AddTransaction at);
  TransactionPoolMessage(DeleteTransaction at);

  // pattern matchin API
  void match(std::function<void(const AddTransaction&)>&&, std::function<void(const DeleteTransaction&)>&&);

  // API with explicit type handling
  TransactionMessageType getType() const;
  const AddTransaction& getAddTransaction() const;
  const DeleteTransaction& getDeleteTransaction() const;

 private:
  const TransactionMessageType type;
  std::variant<AddTransaction, DeleteTransaction> data;
};

TransactionPoolMessage makeAddTransaction(const Crypto::Hash& hash);
TransactionPoolMessage makeDelTransaction(const Crypto::Hash& hash);
}  // namespace CryptoNote
