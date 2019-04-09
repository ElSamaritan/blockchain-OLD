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

#include "CryptoNoteSerialization.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <Xi/ExternalIncludePush.h>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/endian/conversion.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.h>

#include "Serialization/ISerializer.h"
#include "Serialization/SerializationOverloads.h"
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

void getVariantValue(CryptoNote::ISerializer& serializer, uint8_t tag, CryptoNote::TransactionInput& in) {
  switch (tag) {
    case 0xff: {
      CryptoNote::BaseInput v;
      serializer(v, "value");
      in = v;
      break;
    }
    case 0x2: {
      CryptoNote::KeyInput v;
      serializer(v, "value");
      in = v;
      break;
    }
    default:
      throw std::runtime_error("Unknown variant tag");
  }
}

void getVariantValue(CryptoNote::ISerializer& serializer, uint8_t tag, CryptoNote::TransactionOutputTarget& out) {
  switch (tag) {
    case 0x2: {
      CryptoNote::KeyOutput v;
      serializer(v, "data");
      out = v;
      break;
    }
    default:
      throw std::runtime_error("Unknown variant tag");
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
    serializer(vector[i], "");
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

void serialize(TransactionPrefix& txP, ISerializer& serializer) {
  serializer(txP.version, "version");

  if ((Xi::Config::Transaction::maximumVersion() < txP.version ||
       Xi::Config::Transaction::minimumVersion() > txP.version) &&
      serializer.type() == ISerializer::INPUT) {
    throw std::runtime_error("Wrong transaction version");
  }

  serializer(txP.unlockTime, "unlock_time");
  serializer(txP.inputs, "vin");
  serializer(txP.outputs, "vout");
  serializeAsBinary(txP.extra, "extra", serializer);
}

void serialize(BaseTransaction& tx, ISerializer& serializer) {
  serializer(tx.version, "version");
  serializer(tx.unlockTime, "unlock_time");
  serializer(tx.inputs, "vin");
  serializer(tx.outputs, "vout");
  serializeAsBinary(tx.extra, "extra", serializer);
}

void serialize(Transaction& tx, ISerializer& serializer) {
  serialize(static_cast<TransactionPrefix&>(tx), serializer);

  size_t sigSize = tx.inputs.size();
  // TODO: make arrays without sizes
  //  serializer.beginArray(sigSize, "signatures");

  // ignore base transaction
  if (serializer.type() == ISerializer::INPUT && !(sigSize == 1 && tx.inputs[0].type() == typeid(BaseInput))) {
    tx.signatures.resize(sigSize);
  }

  bool signaturesNotExpected = tx.signatures.empty();
  if (!signaturesNotExpected && tx.inputs.size() != tx.signatures.size()) {
    throw std::runtime_error("Serialization error: unexpected signatures size");
  }

  for (size_t i = 0; i < tx.inputs.size(); ++i) {
    size_t signatureSize = getSignaturesCount(tx.inputs[i]);
    if (signaturesNotExpected) {
      if (signatureSize == 0) {
        continue;
      } else {
        throw std::runtime_error("Serialization error: signatures are not expected");
      }
    }

    if (serializer.type() == ISerializer::OUTPUT) {
      if (signatureSize != tx.signatures[i].size()) {
        throw std::runtime_error("Serialization error: unexpected signatures size");
      }

      for (Crypto::Signature& sig : tx.signatures[i]) {
        serializePod(sig, "", serializer);
      }

    } else {
      std::vector<Crypto::Signature> signatures(signatureSize);
      for (Crypto::Signature& sig : signatures) {
        serializePod(sig, "", serializer);
      }

      tx.signatures[i] = std::move(signatures);
    }
  }
  //  serializer.endArray();
}

void serialize(TransactionInput& in, ISerializer& serializer) {
  if (serializer.type() == ISerializer::OUTPUT) {
    BinaryVariantTagGetter tagGetter;
    uint8_t tag = boost::apply_visitor(tagGetter, in);
    serializer.binary(&tag, sizeof(tag), "type");

    VariantSerializer visitor(serializer, "value");
    boost::apply_visitor(visitor, in);
  } else {
    uint8_t tag;
    serializer.binary(&tag, sizeof(tag), "type");

    getVariantValue(serializer, tag, in);
  }
}

void serialize(BaseInput& gen, ISerializer& serializer) { serializer(gen.blockIndex, "height"); }

void serialize(KeyInput& key, ISerializer& serializer) {
  serializer(key.amount, "amount");
  serializeVarintVector(key.outputIndexes, serializer, "key_offsets");
  serializer(key.keyImage, "k_image");
}

void serialize(TransactionOutput& output, ISerializer& serializer) {
  serializer(output.amount, "amount");
  serializer(output.target, "target");
}

void serialize(TransactionOutputTarget& output, ISerializer& serializer) {
  if (serializer.type() == ISerializer::OUTPUT) {
    BinaryVariantTagGetter tagGetter;
    uint8_t tag = boost::apply_visitor(tagGetter, output);
    serializer.binary(&tag, sizeof(tag), "type");

    VariantSerializer visitor(serializer, "data");
    boost::apply_visitor(visitor, output);
  } else {
    uint8_t tag;
    serializer.binary(&tag, sizeof(tag), "type");

    getVariantValue(serializer, tag, output);
  }
}

void serialize(KeyOutput& key, ISerializer& serializer) { serializer(key.key, "key"); }

void serializeBlockHeader(BlockHeader& header, ISerializer& serializer) {
  serializer(header.majorVersion, "major_version");
  if (header.majorVersion > Xi::Config::BlockVersion::maximum()) {
    throw std::runtime_error("Wrong major version");
  }

  serializer(header.minorVersion, "minor_version");
  if (header.minorVersion != Xi::Config::BlockVersion::expectedMinorVersion()) {
    throw std::runtime_error{"Wrong minor version"};
  }

  auto nonce = header.nonce;
  boost::endian::native_to_little_inplace(nonce);
  serializer.binary(&header.nonce, sizeof(header.nonce), "nonce");

  serializer(header.timestamp, "timestamp");
  serializer(header.previousBlockHash, "prev_id");
}

void serialize(BlockHeader& header, ISerializer& serializer) { serializeBlockHeader(header, serializer); }

void serializeStaticReward(Transaction& tx, const uint8_t blockMajorVersion, ISerializer& serializer) {
  if (serializer.type() == ISerializer::INPUT) {
    if (Xi::Config::StaticReward::isEnabled(blockMajorVersion)) {
      serializer(tx, "static_reward");
    } else {
      tx = Transaction::Null;
    }
  } else {
    if (Xi::Config::StaticReward::isEnabled(blockMajorVersion)) {
      serializer(tx, "static_reward");
    } else {
      assert(tx.isNull());
    }
  }
}

void serialize(BlockTemplate& block, ISerializer& serializer) {
  serializeBlockHeader(block, serializer);
  serializer(block.baseTransaction, "miner_tx");
  serializeStaticReward(block.staticReward, block.majorVersion, serializer);
  serializer(block.transactionHashes, "tx_hashes");
}

void serialize(AccountPublicAddress& address, ISerializer& serializer) {
  serializer(address.spendPublicKey, "m_spend_public_key");
  serializer(address.viewPublicKey, "m_view_public_key");
}

void serialize(AccountKeys& keys, ISerializer& s) {
  s(keys.address, "m_account_address");
  s(keys.spendSecretKey, "m_spend_secret_key");
  s(keys.viewSecretKey, "m_view_secret_key");
}

void serialize(KeyPair& keyPair, ISerializer& serializer) {
  serializer(keyPair.secretKey, "secret_key");
  serializer(keyPair.publicKey, "public_key");
}

// unpack to strings to maintain protocol compatibility with older versions
void serialize(RawBlock& rawBlock, ISerializer& serializer) {
  if (serializer.type() == ISerializer::INPUT) {
    uint64_t blockSize;
    serializer(blockSize, "block_size");
    rawBlock.block.resize(static_cast<size_t>(blockSize));
  } else {
    uint64_t blockSize = rawBlock.block.size();
    serializer(blockSize, "block_size");
  }

  serializer.binary(rawBlock.block.data(), rawBlock.block.size(), "block");

  if (serializer.type() == ISerializer::INPUT) {
    uint64_t txCount;
    serializer(txCount, "tx_count");
    rawBlock.transactions.resize(static_cast<size_t>(txCount));

    for (auto& txBlob : rawBlock.transactions) {
      uint64_t txSize;
      serializer(txSize, "tx_size");
      txBlob.resize(txSize);
      serializer.binary(txBlob.data(), txBlob.size(), "transaction");
    }
  } else {
    auto txCount = rawBlock.transactions.size();
    serializer(txCount, "tx_count");

    for (auto& txBlob : rawBlock.transactions) {
      auto txSize = txBlob.size();
      serializer(txSize, "tx_size");
      serializer.binary(txBlob.data(), txBlob.size(), "transaction");
    }
  }
}

void serialize(LiteBlock& block, ISerializer& serializer) {
  if (serializer.type() == ISerializer::INPUT) {
    uint64_t size;
    serializer(size, "size");
    block.blockTemplate.resize(size);
  } else {
    uint64_t size = block.blockTemplate.size();
    serializer(size, "size");
  }
  serializer.binary(block.blockTemplate.data(), block.blockTemplate.size(), "blob");
}

}  // namespace CryptoNote
