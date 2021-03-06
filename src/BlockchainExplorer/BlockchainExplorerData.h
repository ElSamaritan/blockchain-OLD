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

#include <array>
#include <string>
#include <vector>

#include <Xi/Blockchain/Explorer/Data/Data.hpp>
#include <Xi/Crypto/FastHash.hpp>
#include <Xi/Crypto/PublicKey.hpp>
#include <Serialization/VariantSerialization.hpp>
#include <CryptoNoteCore/CryptoNote.h>

namespace CryptoNote {

enum class TransactionRemoveReason : uint8_t { INCLUDED_IN_BLOCK = 0, TIMEOUT = 1 };

struct TransactionOutputDetails {
  TransactionOutput output;
  uint64_t globalIndex;
};

struct TransactionOutputReferenceDetails {
  Crypto::Hash transactionHash;
  size_t number;
};

struct BaseInputDetails {
  BaseInput input;
  uint64_t amount;
};

struct KeyInputDetails {
  KeyInput input;
  uint64_t mixin;
  TransactionOutputReferenceDetails output;
};

typedef std::variant<BaseInputDetails, KeyInputDetails> TransactionInputDetails;

struct TransactionExtraDetails {
  Crypto::PublicKey publicKey;
};

struct TransactionDetails {
  Crypto::Hash hash;
  uint64_t size = 0;
  uint64_t fee = 0;
  uint64_t totalInputsAmount = 0;
  uint64_t totalOutputsAmount = 0;
  uint64_t mixin = 0;
  uint64_t unlockTime = 0;
  uint64_t timestamp = 0;
  PaymentId paymentId;
  bool hasPaymentId = false;
  bool inBlockchain = false;
  Crypto::Hash blockHash;
  BlockHeight blockHeight = BlockHeight::Null;
  TransactionExtraDetails extra;
  std::optional<TransactionSignatures> signatures;
  std::vector<TransactionInputDetails> inputs;
  std::vector<TransactionOutputDetails> outputs;
};

struct BlockDetails {
  BlockVersion version{0};
  BlockFeature features{BlockFeature::None};
  uint64_t timestamp = 0;
  Crypto::Hash prevBlockHash;
  BlockNonce nonce = BlockNonce::Null;
  bool isAlternative = false;
  BlockHeight height = BlockHeight::Null;
  Crypto::Hash hash;
  uint64_t difficulty = 0;
  uint64_t reward = 0;
  uint64_t baseReward = 0;
  uint64_t staticReward = 0;
  uint64_t blockSize = 0;
  uint64_t transactionsCumulativeSize = 0;
  uint64_t alreadyGeneratedCoins = 0;
  uint64_t alreadyGeneratedTransactions = 0;
  uint64_t sizeMedian = 0;
  double penalty = 0.0;
  uint64_t totalFeeAmount = 0;
  std::vector<TransactionDetails> transactions;
};

}  // namespace CryptoNote

XI_SERIALIZATION_VARIANT_TAG(CryptoNote::TransactionInputDetails, 0, 0x01, "base_input");
XI_SERIALIZATION_VARIANT_TAG(CryptoNote::TransactionInputDetails, 1, 0x02, "key_input");
