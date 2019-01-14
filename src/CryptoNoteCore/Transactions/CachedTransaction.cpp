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

#include "CryptoNoteCore/Transactions/CachedTransaction.h"

#include <exception>

#include <Xi/Config.h>
#include <Common/Varint.h>

#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Transactions/TransactionUtils.h"

using namespace Crypto;
using namespace CryptoNote;

CachedTransaction::CachedTransaction(Transaction&& transaction) : transaction(std::move(transaction)) {}

CachedTransaction::CachedTransaction(const Transaction& transaction) : transaction(transaction) {}

CachedTransaction::CachedTransaction(const BinaryArray& transactionBinaryArray)
    : transactionBinaryArray(transactionBinaryArray) {
  if (!fromBinaryArray<Transaction>(transaction, this->transactionBinaryArray.get())) {
    throw std::runtime_error("CachedTransaction::CachedTransaction(BinaryArray&&), deserealization error.");
  }
}

const Transaction& CachedTransaction::getTransaction() const { return transaction; }

const Crypto::Hash& CachedTransaction::getTransactionHash() const {
  if (!transactionHash.is_initialized()) {
    transactionHash = getBinaryArrayHash(getTransactionBinaryArray());
  }

  return transactionHash.get();
}

const Crypto::Hash& CachedTransaction::getTransactionPrefixHash() const {
  if (!transactionPrefixHash.is_initialized()) {
    transactionPrefixHash = getObjectHash(static_cast<const TransactionPrefix&>(transaction));
  }

  return transactionPrefixHash.get();
}

const BinaryArray& CachedTransaction::getTransactionBinaryArray() const {
  if (!transactionBinaryArray.is_initialized()) {
    transactionBinaryArray = toBinaryArray(transaction);
  }

  return transactionBinaryArray.get();
}

size_t CachedTransaction::getBlobSize() const { return getObjectBinarySize(getTransaction()); }

const std::vector<KeyImage>& CachedTransaction::getKeyImages() const {
  if (!keyImages.is_initialized()) {
    keyImages = getTransactionKeyImages(getTransaction());
  }
  return keyImages.get();
}

const Crypto::KeyImagesSet& CachedTransaction::getKeyImagesSet() const {
  if (!keyImagesSet.is_initialized()) {
    Crypto::KeyImagesSet set{};
    for (const auto& keyImage : getKeyImages()) {
      if (!set.insert(keyImage).second)
        throw std::runtime_error{
            "CachedTransaction contains duplicate key images, make sure to check getKeyImages for duplicates first."};
    }
    keyImagesSet = set;
  }
  return keyImagesSet.get();
}

const std::vector<PublicKey>& CachedTransaction::getOutputKeys() const {
  if (!outputKeys.is_initialized()) {
    outputKeys = getTransactionOutputKeys(getTransaction());
  }
  return outputKeys.get();
}

uint64_t CachedTransaction::getInputAmount() const {
  if (!inputAmount.is_initialized()) {
    inputAmount = getTransactionInputAmount(getTransaction());
  }
  return inputAmount.get();
}

uint64_t CachedTransaction::getOutputAmount() const {
  if (!outputAmount.is_initialized()) {
    outputAmount = getTransactionOutputAmount(getTransaction());
  }
  return outputAmount.get();
}

uint64_t CachedTransaction::getTransactionFee() const {
  if (!transactionFee.is_initialized()) {
    transactionFee = getInputAmount() - getOutputAmount();
  }
  return transactionFee.get();
}
