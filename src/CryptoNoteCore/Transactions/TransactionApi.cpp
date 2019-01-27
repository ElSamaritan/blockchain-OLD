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

#include <numeric>
#include <unordered_set>

#include <boost/optional.hpp>

#include <Xi/Config.h>

#include "CryptoNoteCore/Account.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Transactions/ITransactionBuilder.h"
#include "CryptoNoteCore/Transactions/TransactionUtils.h"
#include "CryptoNoteCore/Transactions/TransactionApiExtra.h"

#include "CryptoNoteCore/Transactions/__impl/TransactionBuilder.h"

using namespace Crypto;

namespace CryptoNote {

using namespace Crypto;

////////////////////////////////////////////////////////////////////////
// class Transaction implementation
////////////////////////////////////////////////////////////////////////

std::unique_ptr<ITransactionBuilder> createTransaction() {
  return std::unique_ptr<ITransactionBuilder>(new TransactionBuilder());
}

std::unique_ptr<ITransactionBuilder> createTransaction(const BinaryArray& transactionBlob) {
  return std::unique_ptr<ITransactionBuilder>(new TransactionBuilder(transactionBlob));
}

std::unique_ptr<ITransactionBuilder> createTransaction(const CryptoNote::Transaction& tx) {
  return std::unique_ptr<ITransactionBuilder>(new TransactionBuilder(tx));
}
}  // namespace CryptoNote
