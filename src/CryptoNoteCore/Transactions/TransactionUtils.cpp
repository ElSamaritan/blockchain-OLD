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

#include "CryptoNoteCore/Transactions/TransactionUtils.h"

#include <unordered_set>
#include <numeric>
#include <iterator>
#include <exception>
#include <algorithm>

#include <crypto/crypto.h>

#include "CryptoNoteCore/Account.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"

using namespace Crypto;

namespace CryptoNote {

bool checkInputsKeyimagesDiff(const CryptoNote::TransactionPrefix& tx) {
  std::unordered_set<Crypto::KeyImage> ki;
  for (const auto& in : tx.inputs) {
    if (in.type() == typeid(KeyInput)) {
      if (!ki.insert(boost::get<KeyInput>(in).keyImage).second) return false;
    }
  }

  return true;
}

// TransactionInput helper functions

size_t getRequiredSignaturesCount(const TransactionInput& in) {
  if (in.type() == typeid(KeyInput)) {
    return boost::get<KeyInput>(in).outputIndexes.size();
  }

  return 0;
}

uint64_t getTransactionInputAmount(const TransactionInput& in) {
  if (in.type() == typeid(KeyInput)) {
    return boost::get<KeyInput>(in).amount;
  } else {
    return 0;
  }
}

uint64_t getTransactionInputAmount(const Transaction& transaction) {
  return std::accumulate(transaction.inputs.begin(), transaction.inputs.end(), 0ULL,
                         [](uint64_t acc, const auto& input) { return acc + getTransactionInputAmount(input); });
}

uint64_t getTransactionOutputAmount(const TransactionOutput& out) { return out.amount; }

uint64_t getTransactionOutputAmount(const Transaction& transaction) {
  return std::accumulate(transaction.outputs.begin(), transaction.outputs.end(), 0ULL,
                         [](uint64_t acc, const auto& out) { return acc + getTransactionOutputAmount(out); });
}

boost::optional<KeyImage> getTransactionInputKeyImage(const TransactionInput& input) {
  if (input.type() == typeid(KeyInput)) {
    return boost::optional<KeyImage>{boost::get<KeyInput>(input).keyImage};
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

PublicKey getTransactionOutputKey(const TransactionOutputTarget& target) {
  if (target.type() == typeid(KeyOutput)) {
    return boost::get<KeyOutput>(target).key;
  } else {
    throw std::runtime_error{"Unexpected transaction output type."};
  }
}

PublicKey getTransactionOutputKey(const TransactionOutput& output) { return getTransactionOutputKey(output.target); }

std::vector<PublicKey> getTransactionOutputKeys(const Transaction& transaction) {
  std::vector<PublicKey> keys;
  std::transform(transaction.outputs.begin(), transaction.outputs.end(), std::back_inserter(keys),
                 [](const auto& output) { return getTransactionOutputKey(output); });
  return keys;
}

TransactionTypes::InputType getTransactionInputType(const TransactionInput& in) {
  if (in.type() == typeid(KeyInput)) {
    return TransactionTypes::InputType::Key;
  }

  if (in.type() == typeid(BaseInput)) {
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

TransactionTypes::OutputType getTransactionOutputType(const TransactionOutputTarget& out) {
  if (out.type() == typeid(KeyOutput)) {
    return TransactionTypes::OutputType::Key;
  }

  return TransactionTypes::OutputType::Invalid;
}

const TransactionOutput& getOutputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index) {
  if (transaction.outputs.size() <= index) {
    throw std::runtime_error("Transaction output index out of range");
  }

  return transaction.outputs[index];
}

const TransactionOutput& getOutputChecked(const CryptoNote::TransactionPrefix& transaction, size_t index,
                                          TransactionTypes::OutputType type) {
  const auto& output = getOutputChecked(transaction, index);
  if (getTransactionOutputType(output.target) != type) {
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
  AccountKeys keys;
  keys.address = addr;
  // only view secret key is used, spend key is not needed
  keys.viewSecretKey = viewSecretKey;

  Crypto::PublicKey txPubKey = getTransactionPublicKeyFromExtra(transaction.extra);

  amount = 0;
  size_t keyIndex = 0;
  uint32_t outputIndex = 0;

  Crypto::KeyDerivation derivation;
  generate_key_derivation(txPubKey, keys.viewSecretKey, derivation);

  for (const TransactionOutput& o : transaction.outputs) {
    assert(o.target.type() == typeid(KeyOutput));
    if (o.target.type() == typeid(KeyOutput)) {
      if (is_out_to_acc(keys, boost::get<KeyOutput>(o.target), derivation, keyIndex)) {
        out.push_back(outputIndex);
        amount += o.amount;
      }

      ++keyIndex;
    }

    ++outputIndex;
  }

  return true;
}

std::vector<uint32_t> getTransactionInputIndices(const KeyInput& input) {
  std::vector<uint32_t> indices{};
  if (input.outputIndexes.empty()) return indices;
  indices.reserve(input.outputIndexes.size());
  indices[0] = input.outputIndexes[0];
  for (size_t i = 1; i < input.outputIndexes.size(); ++i) {
    indices[i] = indices[i - 1] + input.outputIndexes[i];
  }
  return indices;
}

}  // namespace CryptoNote
