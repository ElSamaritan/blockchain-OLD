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

#pragma once

#include <vector>

#include <crypto/CryptoTypes.h>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Result.h>

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
  const boost::optional<Crypto::Hash>& getPaymentId() const;
  uint64_t getInputAmount() const;
  uint64_t getOutputAmount() const;
  uint64_t getTransactionFee() const;

 private:
  Transaction transaction;
  mutable boost::optional<BinaryArray> transactionBinaryArray;
  mutable boost::optional<Crypto::Hash> transactionHash;
  mutable boost::optional<Crypto::Hash> transactionPrefixHash;
  mutable boost::optional<std::vector<Crypto::KeyImage>> keyImages;
  mutable boost::optional<Crypto::KeyImageSet> keyImagesSet;
  mutable boost::optional<std::vector<Crypto::PublicKey>> outputKeys;
  mutable bool paymentIdEvaluated;  ///< using optional<optional<<>> seems not to be a good idea
  mutable boost::optional<Crypto::Hash> paymentId;
  mutable boost::optional<uint64_t> inputAmount;
  mutable boost::optional<uint64_t> outputAmount;
  mutable boost::optional<uint64_t> transactionFee;
};

}  // namespace CryptoNote
