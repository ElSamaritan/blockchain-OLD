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

#include "CryptoNoteSerialization.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <Xi/ExternalIncludePush.h>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/endian/conversion.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>

#include "Serialization/ISerializer.h"
#include "Serialization/SerializationOverloads.h"
#include "Serialization/OptionalSerialization.hpp"
#include "Serialization/BinaryInputStreamSerializer.h"
#include "Serialization/BinaryOutputStreamSerializer.h"

#include "Common/StringOutputStream.h"
#include "crypto/crypto.h"

#include "Account.h"
#include <Xi/Config.h>
#include "CryptoNoteFormatUtils.h"
#include "CryptoNoteTools.h"
#include "Transactions/TransactionExtra.h"

using namespace Common;

namespace {

using namespace CryptoNote;
using namespace Common;

size_t getSignaturesCount(const TransactionInput& input) {
  struct txin_signature_size_visitor : public boost::static_visitor<size_t> {
    size_t operator()(const BaseInput& txin) const {
      XI_UNUSED(txin);
      return 0;
    }
    size_t operator()(const KeyInput& txin) const { return txin.outputIndexes.size(); }
  };

  return boost::apply_visitor(txin_signature_size_visitor(), input);
}

struct BinaryVariantTagGetter : boost::static_visitor<uint8_t> {
  uint8_t operator()(const CryptoNote::BaseInput) { return 0xff; }
  uint8_t operator()(const CryptoNote::KeyInput) { return 0x2; }
  uint8_t operator()(const CryptoNote::KeyOutput) { return 0x2; }
  uint8_t operator()(const CryptoNote::Transaction) { return 0xcc; }
  uint8_t operator()(const CryptoNote::BlockTemplate) { return 0xbb; }
};

struct VariantSerializer : boost::static_visitor<> {
  VariantSerializer(CryptoNote::ISerializer& serializer, const std::string& name) : s(serializer), name(name) {}

  template <typename T>
  void operator()(T& param) {
    s(param, name);
  }

  CryptoNote::ISerializer& s;
  std::string name;
};

[[nodiscard]] bool getVariantValue(CryptoNote::ISerializer& serializer, uint8_t tag, CryptoNote::TransactionInput& in) {
  switch (tag) {
    case 0xff: {
      CryptoNote::BaseInput v;
      XI_RETURN_EC_IF_NOT(serializer(v, "value"), false);
      in = v;
      return true;
    }
    case 0x2: {
      CryptoNote::KeyInput v;
      XI_RETURN_EC_IF_NOT(serializer(v, "value"), false);
      in = v;
      return true;
    }
    default:
      return false;
  }
}

[[nodiscard]] bool getVariantValue(CryptoNote::ISerializer& serializer, uint8_t tag,
                                   CryptoNote::TransactionOutputTarget& out) {
  switch (tag) {
    case 0x2: {
      CryptoNote::KeyOutput v;
      serializer(v, "data");
      out = v;
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

bool serializeVarintVector(std::vector<uint32_t>& vector, CryptoNote::ISerializer& serializer,
                           Common::StringView name) {
  size_t size = vector.size();

  if (!serializer.beginArray(size, name)) {
    vector.clear();
    return false;
  }

  vector.resize(size);

  for (size_t i = 0; i < size; ++i) {
    XI_RETURN_EC_IF_NOT(serializer(vector[i], ""), false);
  }

  serializer.endArray();
  return true;
}

}  // namespace

namespace Crypto {

bool serialize(chacha8_iv& chacha, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializePod(chacha, name, serializer);
}

bool serialize(EllipticCurveScalar& ecScalar, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializePod(ecScalar, name, serializer);
}

bool serialize(EllipticCurvePoint& ecPoint, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializePod(ecPoint, name, serializer);
}

}  // namespace Crypto

namespace CryptoNote {

bool serialize(TransactionPrefix& txP, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(txP.version, "version"), false);
  XI_RETURN_EC_IF_NOT(serializer(txP.unlockTime, "unlock_time"), false);
  XI_RETURN_EC_IF_NOT(serializer(txP.inputs, "vin"), false);
  XI_RETURN_EC_IF_NOT(serializer(txP.outputs, "vout"), false);
  XI_RETURN_EC_IF_NOT(serializeAsBinary(txP.extra, "extra", serializer), false);
  return true;
}

bool serialize(BaseTransaction& tx, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(tx.version, "version"), false);
  XI_RETURN_EC_IF_NOT(serializer(tx.unlockTime, "unlock_time"), false);
  XI_RETURN_EC_IF_NOT(serializer(tx.inputs, "vin"), false);
  XI_RETURN_EC_IF_NOT(serializer(tx.outputs, "vout"), false);
  XI_RETURN_EC_IF_NOT(serializeAsBinary(tx.extra, "extra", serializer), false);
  return true;
}

bool serialize(Transaction& tx, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serialize(static_cast<TransactionPrefix&>(tx), serializer), false);

  size_t sigSize = tx.inputs.size();
  XI_RETURN_EC_IF_NOT(serializer.beginStaticArray(sigSize, "signatures"), false);

  // ignore base transaction
  if (serializer.type() == ISerializer::INPUT && !(sigSize == 1 && tx.inputs[0].type() == typeid(BaseInput))) {
    tx.signatures.resize(sigSize);
  }

  bool signaturesNotExpected = tx.signatures.empty();
  if (!signaturesNotExpected && tx.inputs.size() != tx.signatures.size()) {
    return false;
  }

  for (size_t i = 0; i < tx.inputs.size(); ++i) {
    size_t signatureSize = getSignaturesCount(tx.inputs[i]);
    if (signaturesNotExpected) {
      if (signatureSize == 0) {
        continue;
      } else {
        return false;
      }
    }

    if (serializer.type() == ISerializer::OUTPUT) {
      if (signatureSize != tx.signatures[i].size()) {
        return false;
      }

      for (Crypto::Signature& sig : tx.signatures[i]) {
        XI_RETURN_EC_IF_NOT(serializePod(sig, "", serializer), false);
      }

    } else {
      std::vector<Crypto::Signature> signatures(signatureSize);
      for (Crypto::Signature& sig : signatures) {
        XI_RETURN_EC_IF_NOT(serializePod(sig, "", serializer), false);
      }

      tx.signatures[i] = std::move(signatures);
    }
  }
  serializer.endArray();
  return true;
}

bool serialize(TransactionInput& in, ISerializer& serializer) {
  if (serializer.type() == ISerializer::OUTPUT) {
    BinaryVariantTagGetter tagGetter;
    uint8_t tag = boost::apply_visitor(tagGetter, in);
    XI_RETURN_EC_IF_NOT(serializer.binary(&tag, sizeof(tag), "type"), false);

    VariantSerializer visitor(serializer, "value");
    boost::apply_visitor(visitor, in);
  } else {
    uint8_t tag;
    serializer.binary(&tag, sizeof(tag), "type");

    XI_RETURN_EC_IF_NOT(getVariantValue(serializer, tag, in), false);
  }
  return true;
}

bool serialize(BaseInput& gen, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(gen.blockIndex, "height"), false);
  return true;
}

bool serialize(KeyInput& key, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(key.amount, "amount"), false);
  XI_RETURN_EC_IF_NOT(serializeVarintVector(key.outputIndexes, serializer, "key_offsets"), false);
  XI_RETURN_EC_IF_NOT(serializer(key.keyImage, "k_image"), false);
  return true;
}

bool serialize(TransactionOutput& output, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(output.amount, "amount"), false);
  XI_RETURN_EC_IF_NOT(serializer(output.target, "target"), false);
  return true;
}

bool serialize(TransactionOutputTarget& output, ISerializer& serializer) {
  if (serializer.type() == ISerializer::OUTPUT) {
    BinaryVariantTagGetter tagGetter;
    uint8_t tag = boost::apply_visitor(tagGetter, output);
    XI_RETURN_EC_IF_NOT(serializer.binary(&tag, sizeof(tag), "type"), false);

    VariantSerializer visitor(serializer, "data");
    boost::apply_visitor(visitor, output);
    return true;
  } else {
    uint8_t tag;
    serializer.binary(&tag, sizeof(tag), "type");

    XI_RETURN_EC_IF_NOT(getVariantValue(serializer, tag, output), false);
    return true;
  }
}

bool serialize(KeyOutput& key, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(key.key, "key"), false);
  return true;
}

bool serializeBlockHeader(BlockHeader& header, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(header.majorVersion, "major_version"), false);
  XI_RETURN_EC_IF_NOT(serializer(header.minorVersion, "minor_version"), false);
  auto nonce = header.nonce;
  boost::endian::native_to_little_inplace(nonce);
  XI_RETURN_EC_IF_NOT(serializer.binary(&header.nonce, sizeof(header.nonce), "nonce"), false);

  XI_RETURN_EC_IF_NOT(serializer(header.timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializer(header.previousBlockHash, "prev_id"), false);
  return true;
}

bool serialize(BlockHeader& header, ISerializer& serializer) { return serializeBlockHeader(header, serializer); }

bool serialize(BlockTemplate& block, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializeBlockHeader(block, serializer), false);
  XI_RETURN_EC_IF_NOT(serializer(block.baseTransaction, "miner_tx"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.staticRewardHash, "static_reward_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(block.transactionHashes, "tx_hashes"), false);
  return true;
}

bool serialize(AccountPublicAddress& address, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(address.spendPublicKey, "m_spend_public_key"), false);
  XI_RETURN_EC_IF_NOT(serializer(address.viewPublicKey, "m_view_public_key"), false);
  return true;
}

bool serialize(AccountKeys& keys, ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(keys.address, "m_account_address"), false);
  XI_RETURN_EC_IF_NOT(s(keys.spendSecretKey, "m_spend_secret_key"), false);
  XI_RETURN_EC_IF_NOT(s(keys.viewSecretKey, "m_view_secret_key"), false);
  return true;
}

bool serialize(KeyPair& keyPair, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(keyPair.secretKey, "secret_key"), false);
  XI_RETURN_EC_IF_NOT(serializer(keyPair.publicKey, "public_key"), false);
  return true;
}

// unpack to strings to maintain protocol compatibility with older versions
bool serialize(RawBlock& rawBlock, ISerializer& serializer) {
  if (serializer.type() == ISerializer::INPUT) {
    uint64_t blockSize;
    XI_RETURN_EC_IF_NOT(serializer(blockSize, "block_size"), false);
    rawBlock.block.resize(static_cast<size_t>(blockSize));
  } else {
    uint64_t blockSize = rawBlock.block.size();
    XI_RETURN_EC_IF_NOT(serializer(blockSize, "block_size"), false);
  }

  XI_RETURN_EC_IF_NOT(serializer.binary(rawBlock.block.data(), rawBlock.block.size(), "block"), false);

  if (serializer.type() == ISerializer::INPUT) {
    uint64_t txCount;
    serializer(txCount, "tx_count");
    rawBlock.transactions.resize(static_cast<size_t>(txCount));

    for (auto& txBlob : rawBlock.transactions) {
      uint64_t txSize;
      serializer(txSize, "tx_size");
      txBlob.resize(txSize);
      XI_RETURN_EC_IF_NOT(serializer.binary(txBlob.data(), txBlob.size(), "transaction"), false);
    }
    return false;
  } else {
    auto txCount = rawBlock.transactions.size();
    XI_RETURN_EC_IF_NOT(serializer(txCount, "tx_count"), false);

    for (auto& txBlob : rawBlock.transactions) {
      auto txSize = txBlob.size();
      XI_RETURN_EC_IF_NOT(serializer(txSize, "tx_size"), false);
      XI_RETURN_EC_IF_NOT(serializer.binary(txBlob.data(), txBlob.size(), "transaction"), false);
    }
    return true;
  }
}

bool serialize(LiteBlock& block, ISerializer& serializer) {
  if (serializer.type() == ISerializer::INPUT) {
    uint64_t size;
    XI_RETURN_EC_IF_NOT(serializer(size, "size"), false);
    block.blockTemplate.resize(size);
  } else {
    uint64_t size = block.blockTemplate.size();
    XI_RETURN_EC_IF_NOT(serializer(size, "size"), false);
  }
  XI_RETURN_EC_IF_NOT(serializer.binary(block.blockTemplate.data(), block.blockTemplate.size(), "blob"), false);
  return true;
}

bool serialize(FeeAddress& addr, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(addr.address, "address"), false);
  XI_RETURN_EC_IF_NOT(serializer(addr.amount, "amount"), false);
  return true;
}

bool serialize(FullBlock& block, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(block.blockTemplate, "block_template"), false);
  XI_RETURN_EC_IF_NOT(serializer.beginStaticArray(block.blockTemplate.transactionHashes.size(), "transactions"), false);
  if (serializer.isInput()) {
    block.transactions.resize(block.blockTemplate.transactionHashes.size());
  }
  for (auto& iTransaction : block.transactions) {
    XI_RETURN_EC_IF_NOT(serializer(iTransaction, ""), false);
  }
  serializer.endArray();
  return true;
}

}  // namespace CryptoNote
