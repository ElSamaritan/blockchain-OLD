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

#include "BlockchainExplorerDataSerialization.h"

#include <stdexcept>

#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "CryptoNoteCore/CryptoNoteSerialization.h"

#include "Serialization/SerializationOverloads.h"

namespace CryptoNote {

using CryptoNote::SerializationTag;

namespace {

struct BinaryVariantTagGetter : boost::static_visitor<uint8_t> {
  uint8_t operator()(const CryptoNote::BaseInputDetails) { return static_cast<uint8_t>(SerializationTag::Base); }
  uint8_t operator()(const CryptoNote::KeyInputDetails) { return static_cast<uint8_t>(SerializationTag::Key); }
};

struct VariantSerializer : boost::static_visitor<> {
  VariantSerializer(CryptoNote::ISerializer& serializer, const std::string& name) : s(serializer), name(name) {}

  template <typename T>
  bool operator()(T& param) {
    return s(param, name);
  }

  CryptoNote::ISerializer& s;
  const std::string name;
};

bool getVariantValue(CryptoNote::ISerializer& serializer, uint8_t tag,
                     boost::variant<CryptoNote::BaseInputDetails, CryptoNote::KeyInputDetails>& in) {
  switch (static_cast<SerializationTag>(tag)) {
    case SerializationTag::Base: {
      CryptoNote::BaseInputDetails v;
      XI_RETURN_EC_IF_NOT(serializer(v, "data"), false);
      in = v;
      return true;
    }
    case SerializationTag::Key: {
      CryptoNote::KeyInputDetails v;
      XI_RETURN_EC_IF_NOT(serializer(v, "data"), false);
      in = v;
      return true;
    }
    default:
      return false;
  }
}

template <typename T>
bool serializePod(T& v, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializer.binary(&v, sizeof(v), name);
}

}  // namespace

// namespace CryptoNote {

bool serialize(TransactionOutputDetails& output, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(output.output, "output"), false);
  XI_RETURN_EC_IF_NOT(serializer(output.globalIndex, "globalIndex"), false);
  return true;
}

bool serialize(TransactionOutputReferenceDetails& outputReference, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializePod(outputReference.transactionHash, "transactionHash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(outputReference.number, "number"), false);
  return true;
}

bool serialize(BaseInputDetails& inputBase, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(inputBase.input, "input"), false);
  XI_RETURN_EC_IF_NOT(serializer(inputBase.amount, "amount"), false);
  return true;
}

bool serialize(KeyInputDetails& inputToKey, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(inputToKey.input, "input"), false);
  XI_RETURN_EC_IF_NOT(serializer(inputToKey.mixin, "mixin"), false);
  XI_RETURN_EC_IF_NOT(serializer(inputToKey.output, "output"), false);
  return true;
}

bool serialize(TransactionInputDetails& input, ISerializer& serializer) {
  if (serializer.type() == ISerializer::OUTPUT) {
    BinaryVariantTagGetter tagGetter;
    uint8_t tag = boost::apply_visitor(tagGetter, input);
    XI_RETURN_EC_IF_NOT(serializer.binary(&tag, sizeof(tag), "type"), false);
    if (input.type() == typeid(BaseInputDetails)) {
      XI_RETURN_EC_IF_NOT(serializer(boost::get<BaseInputDetails>(input), "data"), false);
      return true;
    } else if (input.type() == typeid(KeyInputDetails)) {
      XI_RETURN_EC_IF_NOT(serializer(boost::get<KeyInputDetails>(input), "data"), false);
      return true;
    } else {
      return false;
    }
  } else {
    uint8_t tag;
    XI_RETURN_EC_IF_NOT(serializer.binary(&tag, sizeof(tag), "type"), false);
    XI_RETURN_EC_IF_NOT(getVariantValue(serializer, tag, input), false);
    return true;
  }
}

bool serialize(TransactionExtraDetails& extra, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializePod(extra.publicKey, "publicKey", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(extra.nonce, "nonce"), false);
  XI_RETURN_EC_IF_NOT(serializeAsBinary(extra.raw, "raw", serializer), false);
  return true;
}

bool serialize(TransactionDetails& transaction, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializePod(transaction.hash, "hash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.size, "size"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.fee, "fee"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.totalInputsAmount, "totalInputsAmount"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.totalOutputsAmount, "totalOutputsAmount"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.mixin, "mixin"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.unlockTime, "unlockTime"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializePod(transaction.paymentId, "paymentId", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.inBlockchain, "inBlockchain"), false);
  XI_RETURN_EC_IF_NOT(serializePod(transaction.blockHash, "blockHash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.blockIndex, "blockIndex"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.extra, "extra"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.inputs, "inputs"), false);
  XI_RETURN_EC_IF_NOT(serializer(transaction.outputs, "outputs"), false);

  // serializer(transaction.signatures, "signatures");
  if (serializer.type() == ISerializer::OUTPUT) {
    std::vector<std::pair<size_t, Crypto::Signature>> signaturesForSerialization;
    signaturesForSerialization.reserve(transaction.signatures.size());
    size_t ctr = 0;
    for (const auto& signaturesV : transaction.signatures) {
      for (auto signature : signaturesV) {
        signaturesForSerialization.emplace_back(ctr, std::move(signature));
      }
      ++ctr;
    }
    size_t size = transaction.signatures.size();
    XI_RETURN_EC_IF_NOT(serializer(size, "signaturesSize"), false);
    XI_RETURN_EC_IF_NOT(serializer(signaturesForSerialization, "signatures"), false);
    return true;
  } else {
    size_t size = 0;
    XI_RETURN_EC_IF_NOT(serializer(size, "signaturesSize"), false);
    transaction.signatures.resize(size);

    std::vector<std::pair<size_t, Crypto::Signature>> signaturesForSerialization;
    XI_RETURN_EC_IF_NOT(serializer(signaturesForSerialization, "signatures"), false);

    for (const auto& signatureWithIndex : signaturesForSerialization) {
      transaction.signatures[signatureWithIndex.first].push_back(signatureWithIndex.second);
    }
    return true;
  }
}

bool serialize(BlockDetails& block, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(block.majorVersion, "majorVersion"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.minorVersion, "minorVersion"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializePod(block.prevBlockHash, "prevBlockHash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(block.nonce, "nonce"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.index, "index"), false);
  XI_RETURN_EC_IF_NOT(serializePod(block.hash, "hash", serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(block.difficulty, "difficulty"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.reward, "reward"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.baseReward, "baseReward"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.staticReward, "staticReward"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.blockSize, "blockSize"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.transactionsCumulativeSize, "transactionsCumulativeSize"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.alreadyGeneratedCoins, "alreadyGeneratedCoins"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.alreadyGeneratedTransactions, "alreadyGeneratedTransactions"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.sizeMedian, "sizeMedian"), false);
  /* Some serializers don't support doubles, which causes this to fail and
     not serialize the whole object
  serializer(block.penalty, "penalty");
  */
  XI_RETURN_EC_IF_NOT(serializer(block.totalFeeAmount, "totalFeeAmount"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.transactions, "transactions"), false);
  return true;
}

}  // namespace CryptoNote
