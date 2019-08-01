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

#pragma once

#include <vector>
#include <set>
#include <map>

#include <Xi/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Result.h>
#include <Xi/Byte.hh>
#include <Xi/Crypto/KeyImage.hpp>
#include <Xi/Blockchain/Transaction/Transaction.hpp>

#include "CryptoNoteCore/CryptoNote.h"

namespace CryptoNote {

class CachedTransaction {
  CachedTransaction();

 public:
  static Xi::Result<CachedTransaction> fromBinaryArray(const BinaryArray& blob);

 public:
  explicit CachedTransaction(Transaction&& transaction);
  explicit CachedTransaction(const Transaction& transaction);

  /*!
   * \deprecated user CachedTransaction::fromBinaryArray
   * \throws std::runtime_error if blob is illformed
   */
  explicit CachedTransaction(const BinaryArray& transactionBinaryArray);
  const Transaction& getTransaction() const;
  const Crypto::Hash& getTransactionHash() const;
  const Crypto::Hash& getTransactionPrefixHash() const;
  const BinaryArray& getTransactionBinaryArray() const;
  size_t getBlobSize() const;
  const std::vector<Crypto::KeyImage>& getKeyImages() const;
  const Crypto::KeyImageSet& getKeyImagesSet() const;
  const std::vector<Crypto::PublicKey>& getOutputKeys() const;
  const Crypto::PublicKey& getPublicKey() const;
  const boost::optional<PaymentId>& getPaymentId() const;
  uint64_t getInputAmount() const;
  uint64_t getOutputAmount() const;
  uint64_t getTransactionFee() const;
  const std::set<uint64_t>& getAmountsUsed() const;
  const std::map<uint64_t, uint64_t>& getAmountsUsedCount() const;
  const std::map<uint64_t, uint64_t>& getAmountsGeneratedCount() const;
  bool isCoinbase() const;
  void prune();

 private:
  Transaction transaction;
  mutable boost::optional<BinaryArray> transactionBinaryArray;
  mutable boost::optional<Crypto::Hash> transactionHash;
  mutable boost::optional<Crypto::Hash> transactionPrefixHash;
  mutable boost::optional<std::vector<Crypto::KeyImage>> keyImages;
  mutable boost::optional<Crypto::KeyImageSet> keyImagesSet;
  mutable boost::optional<std::vector<Crypto::PublicKey>> outputKeys;
  mutable boost::optional<Crypto::PublicKey> publicKey;
  mutable bool paymentIdEvaluated;  ///< using optional<optional<<>> seems not to be a good idea
  mutable boost::optional<PaymentId> paymentId;
  mutable boost::optional<uint64_t> inputAmount;
  mutable boost::optional<std::set<uint64_t>> amountsUsed;
  mutable boost::optional<std::map<uint64_t, uint64_t>> amountsUsedCount;
  mutable boost::optional<std::map<uint64_t, uint64_t>> amountsGeneratedCount;
  mutable boost::optional<uint64_t> outputAmount;
  mutable boost::optional<uint64_t> transactionFee;
  mutable boost::optional<uint64_t> transactionBlobSize;
};

CachedTransaction cache(Transaction transaction);

using CachedTransactionVector = std::vector<CachedTransaction>;

}  // namespace CryptoNote
