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

#include <vector>

#include <crypto/CryptoTypes.h>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include "CryptoNoteCore/CryptoNote.h"

namespace CryptoNote {

class CachedTransaction {
 public:
  explicit CachedTransaction(Transaction&& transaction);
  explicit CachedTransaction(const Transaction& transaction);
  explicit CachedTransaction(const BinaryArray& transactionBinaryArray);
  const Transaction& getTransaction() const;
  const Crypto::Hash& getTransactionHash() const;
  const Crypto::Hash& getTransactionPrefixHash() const;
  const BinaryArray& getTransactionBinaryArray() const;
  size_t getBlobSize() const;
  const std::vector<Crypto::KeyImage>& getKeyImages() const;
  const Crypto::KeyImagesSet& getKeyImagesSet() const;
  const std::vector<Crypto::PublicKey>& getOutputKeys() const;
  uint64_t getInputAmount() const;
  uint64_t getOutputAmount() const;
  uint64_t getTransactionFee() const;

 private:
  Transaction transaction;
  mutable boost::optional<BinaryArray> transactionBinaryArray;
  mutable boost::optional<Crypto::Hash> transactionHash;
  mutable boost::optional<Crypto::Hash> transactionPrefixHash;
  mutable boost::optional<std::vector<Crypto::KeyImage>> keyImages;
  mutable boost::optional<Crypto::KeyImagesSet> keyImagesSet;
  mutable boost::optional<std::vector<Crypto::PublicKey>> outputKeys;
  mutable boost::optional<uint64_t> inputAmount;
  mutable boost::optional<uint64_t> outputAmount;
  mutable boost::optional<uint64_t> transactionFee;
};

}  // namespace CryptoNote
