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

#include "CryptoNoteCore/Transactions/TransactionUtils.h"

#include <unordered_set>
#include <numeric>
#include <iterator>
#include <exception>
#include <algorithm>

#include <Xi/Exceptions.hpp>
#include <crypto/crypto.h>

#include "CryptoNoteCore/Account.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"

using namespace Crypto;

namespace CryptoNote {

bool checkInputsKeyimagesDiff(const CryptoNote::TransactionPrefix& tx) {
  std::unordered_set<Crypto::KeyImage> ki;
  for (const auto& in : tx.inputs) {
    if (auto keyInput = std::get_if<KeyInput>(&in)) {
      if (!ki.insert(keyInput->keyImage).second)
        return false;
    }
  }

  return true;
}

// TransactionInput helper functions

size_t getRequiredSignaturesCount(const TransactionInput& in) {
  if (auto keyInput = std::get_if<KeyInput>(&in)) {
    return keyInput->outputIndices.size();
  }

  return 0;
}

uint64_t getTransactionInputAmount(const TransactionInput& in) {
  if (auto keyInput = std::get_if<KeyInput>(&in)) {
    return keyInput->amount;
  } else {
    return 0;
  }
}

uint64_t getTransactionInputAmount(const Transaction& transaction) {
  return std::accumulate(transaction.inputs.begin(), transaction.inputs.end(), 0ULL,
                         [](uint64_t acc, const auto& input) { return acc + getTransactionInputAmount(input); });
}

uint64_t getTransactionOutputAmount(const TransactionOutput& out) {
  if (const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(out))) {
    return amountOutput->amount;
  } else {
    return 0ULL;
  }
}

uint64_t getTransactionOutputAmount(const Transaction& transaction) {
  return std::accumulate(transaction.outputs.begin(), transaction.outputs.end(), 0ULL,
                         [](uint64_t acc, const auto& out) { return acc + getTransactionOutputAmount(out); });
}

boost::optional<KeyImage> getTransactionInputKeyImage(const TransactionInput& input) {
  if (auto keyInput = std::get_if<KeyInput>(&input)) {
    return boost::optional<KeyImage>{keyInput->keyImage};
  } else {
    return boost::optional<KeyImage>{};
  }
}

std::vector<KeyImage> getTransactionKeyImages(const Transaction& transaction) {
  std::vector<KeyImage> reval{};
  reval.reserve(transaction.inputs.size());
  for (const auto& input : transaction.inputs) {
    auto keyImage = getTransactionInputKeyImage(input);
    if (keyImage) {
      reval.push_back(*keyImage);
    }
  }
  return reval;
}

std::vector<PublicKey> getTransactionOutputKeys(const Transaction& transaction) {
  std::vector<PublicKey> keys;
  keys.reserve(transaction.outputs.size());
  for (const auto& output : transaction.outputs) {
    const auto amountOutput = std::get_if<TransactionAmountOutput>(std::addressof(output));
    if (amountOutput == nullptr) {
      continue;
    }
    const auto keyTarget = std::get_if<KeyOutput>(std::addressof(amountOutput->target));
    if (keyTarget == nullptr) {
      continue;
    }
    keys.emplace_back(keyTarget->key);
  }
  return keys;
}

TransactionTypes::InputType getTransactionInputType(const TransactionInput& in) {
  if (std::holds_alternative<KeyInput>(in)) {
    return TransactionTypes::InputType::Key;
  }

  if (std::holds_alternative<BaseInput>(in)) {
    return TransactionTypes::InputType::Generating;
  }

  return TransactionTypes::InputType::Invalid;
}

const TransactionInput& getInputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index) {
  if (transaction.inputs.size() <= index) {
    throw std::runtime_error("Transaction input index out of range");
  }

  return transaction.inputs[index];
}

const TransactionInput& getInputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index,
                                        TransactionTypes::InputType type) {
  const auto& input = getInputChecked(transaction, index);
  if (getTransactionInputType(input) != type) {
    throw std::runtime_error("Unexpected transaction input type");
  }

  return input;
}

// TransactionOutput helper functions
TransactionTypes::OutputType getTransactionOutputType(const TransactionOutput& out) {
  if (std::holds_alternative<TransactionAmountOutput>(out)) {
    return TransactionTypes::OutputType::Amount;
  }

  return TransactionTypes::OutputType::Invalid;
}

TransactionTypes::OutputTargetType getTransactionOutputTargetType(const TransactionOutputTarget& out) {
  if (std::holds_alternative<KeyOutput>(out)) {
    return TransactionTypes::OutputTargetType::Key;
  }

  return TransactionTypes::OutputTargetType::Invalid;
}

const TransactionOutput& getOutputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index) {
  if (transaction.outputs.size() <= index) {
    throw std::runtime_error("Transaction output index out of range");
  }

  return transaction.outputs[index];
}

const TransactionOutputTarget& getOutputTargetChecked(const TransactionOutput& out) {
  if (!std::holds_alternative<TransactionAmountOutput>(out)) {
    throw std::runtime_error{"Unexpected transaction output type"};
  }
  return std::get<TransactionAmountOutput>(out).target;
}

const TransactionOutput& getOutputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index,
                                          TransactionTypes::OutputType outType,
                                          TransactionTypes::OutputTargetType type) {
  const auto& output = getOutputChecked(transaction, index);
  const auto outputType = getTransactionOutputType(output);
  if (outputType != outType) {
    throw std::runtime_error{"Unexpected transaction output type"};
  }

  const auto& target = getOutputTargetChecked(output);
  if (getTransactionOutputTargetType(target) != type) {
    throw std::runtime_error("Unexpected transaction output target type");
  }

  return output;
}

bool isOutToKey(const Crypto::PublicKey& spendPublicKey, const Crypto::PublicKey& outKey,
                const Crypto::KeyDerivation& derivation, size_t keyIndex) {
  Crypto::PublicKey pk;
  derive_public_key(derivation, keyIndex, spendPublicKey, pk);
  return pk == outKey;
}

bool findOutputsToAccount(const CryptoNote::TransactionPrefix& transaction, const AccountPublicAddress& addr,
                          const SecretKey& viewSecretKey, std::vector<uint32_t>& out, uint64_t& amount) {
  using namespace Xi;

  AccountKeys keys;
  keys.address = addr;
  // only view secret key is used, spend key is not needed
  keys.viewSecretKey = viewSecretKey;

  ::Crypto::PublicKey txPubKey = getTransactionPublicKeyFromExtra(transaction.extra);

  amount = 0;
  size_t keyIndex = 0;
  uint32_t outputIndex = 0;

  ::Crypto::KeyDerivation derivation;
  generate_key_derivation(txPubKey, keys.viewSecretKey, derivation);

  for (const TransactionOutput& o : transaction.outputs) {
    exceptional_if_not<InvalidVariantTypeError>(std::holds_alternative<TransactionAmountOutput>(o));
    const auto& amountOutput = std::get<TransactionAmountOutput>(o);
    exceptional_if_not<InvalidVariantTypeError>(std::holds_alternative<KeyOutput>(amountOutput.target));
    const auto& keyOutput = std::get<KeyOutput>(amountOutput.target);

    if (is_out_to_acc(keys, keyOutput, derivation, keyIndex)) {
      out.push_back(outputIndex);
      amount += amountOutput.amount;
    }

    ++keyIndex;
    ++outputIndex;
  }

  return true;
}

std::vector<uint32_t> getTransactionInputIndices(const KeyInput& input) {
  std::vector<uint32_t> indices{};
  if (input.outputIndices.empty())
    return indices;
  indices.resize(input.outputIndices.size());
  indices[0] = input.outputIndices[0];
  for (size_t i = 1; i < input.outputIndices.size(); ++i) {
    indices[i] = indices[i - 1] + input.outputIndices[i];
  }
  return indices;
}

}  // namespace CryptoNote
