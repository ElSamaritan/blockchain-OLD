/* ============================================================================================== *
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

#include "CryptoNoteCore/Transactions/CachedTransaction.h"

#include <exception>

#include <Xi/Config.h>
#include <Common/Varint.h>

#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Transactions/TransactionUtils.h"
#include "CryptoNoteCore/Transactions/TransactionApiExtra.h"
#include "CryptoNoteCore/Transactions/TransactionValidationErrors.h"

using namespace Crypto;
using namespace CryptoNote;

CachedTransaction::CachedTransaction()
    : transactionBinaryArray{boost::none},
      transactionHash{boost::none},
      transactionPrefixHash{boost::none},
      keyImages{boost::none},
      keyImagesSet{boost::none},
      outputKeys{boost::none},
      paymentIdEvaluated{false},
      paymentId{boost::none},
      inputAmount{boost::none},
      outputAmount{boost::none},
      transactionFee{boost::none} {
}

Xi::Result<CachedTransaction> CachedTransaction::fromBinaryArray(const BinaryArray& blob) {
  Transaction transaction{};
  if (!::fromBinaryArray<Transaction>(transaction, blob)) {
    return Xi::make_error(error::TransactionValidationError::INVALID_BINARY_REPRESNETATION);
  } else {
    CachedTransaction reval{transaction};
    reval.transactionBinaryArray = blob;
    return Xi::success<CachedTransaction>(std::move(reval));
  }
}

CachedTransaction::CachedTransaction(Transaction&& transaction) : CachedTransaction() {
  this->transaction = std::move(transaction);
}

CachedTransaction::CachedTransaction(const Transaction& transaction) : CachedTransaction() {
  this->transaction = transaction;
}

CachedTransaction::CachedTransaction(const BinaryArray& transactionBinaryArray) : CachedTransaction() {
  this->transactionBinaryArray = transactionBinaryArray;
  if (!::fromBinaryArray<Transaction>(transaction, this->transactionBinaryArray.get())) {
    throw std::runtime_error("CachedTransaction::CachedTransaction(BinaryArray&&), deserealization error.");
  }
}

const Transaction& CachedTransaction::getTransaction() const {
  return transaction;
}

const Crypto::Hash& CachedTransaction::getTransactionHash() const {
  if (!transactionHash.is_initialized()) {
    transactionHash = getTransaction().hash();
  }

  return transactionHash.get();
}

const Crypto::Hash& CachedTransaction::getTransactionPrefixHash() const {
  if (!transactionPrefixHash.is_initialized()) {
    transactionPrefixHash = getTransaction().prefixHash();
  }

  return transactionPrefixHash.get();
}

const BinaryArray& CachedTransaction::getTransactionBinaryArray() const {
  if (!transactionBinaryArray.is_initialized()) {
    transactionBinaryArray = toBinaryArray(transaction);
  }

  return transactionBinaryArray.get();
}

size_t CachedTransaction::getBlobSize() const {
  if (!transactionBlobSize.is_initialized()) {
    transactionBlobSize = getTransaction().binarySize();
  }
  return transactionBlobSize.get();
}

const std::vector<KeyImage>& CachedTransaction::getKeyImages() const {
  if (!keyImages.is_initialized()) {
    keyImages = getTransactionKeyImages(getTransaction());
  }
  return keyImages.get();
}

const Crypto::KeyImageSet& CachedTransaction::getKeyImagesSet() const {
  if (!keyImagesSet.is_initialized()) {
    Crypto::KeyImageSet set{};
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

const PublicKey& CachedTransaction::getPublicKey() const {
  if (!publicKey.is_initialized()) {
    publicKey = getTransactionPublicKeyFromExtra(transaction.extra);
  }
  return *publicKey;
}

const boost::optional<PaymentId>& CachedTransaction::getPaymentId() const {
  if (!paymentIdEvaluated) {
    PaymentId pid{};
    if (getPaymentIdFromTxExtra(getTransaction().extra, pid)) {
      paymentId = pid;
    }
    paymentIdEvaluated = true;
  }
  return paymentId;
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

const std::set<uint64_t>& CachedTransaction::getAmountsUsed() const {
  if (!amountsUsed.is_initialized()) {
    amountsUsed = boost::value_initialized<std::set<uint64_t>>();
    for (const auto& input : transaction.inputs) {
      if (const auto keyInput = std::get_if<KeyInput>(std::addressof(input))) {
        amountsUsed->insert(keyInput->amount);
      }
    }
  }
  return amountsUsed.get();
}

const std::map<uint64_t, uint64_t>& CachedTransaction::getAmountsUsedCount() const {
  if (!amountsUsedCount.is_initialized()) {
    amountsUsedCount = boost::value_initialized<std::map<uint64_t, uint64_t>>();
    for (const auto& input : transaction.inputs) {
      if (const auto keyInput = std::get_if<KeyInput>(std::addressof(input))) {
        (*amountsUsedCount)[keyInput->amount] += 1;
      }
    }
  }
  return amountsUsedCount.get();
}

bool CachedTransaction::isCoinbase() const {
  return getInputAmount() == 0;
}

void CachedTransaction::prune() {
  getTransaction();
  transaction.prune();
  transactionBinaryArray = boost::none;
}

CachedTransaction CryptoNote::cache(Transaction transaction) {
  return CachedTransaction{std::move(transaction)};
}
