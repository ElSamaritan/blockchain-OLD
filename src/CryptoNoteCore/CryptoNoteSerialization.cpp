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

template <typename T>
[[nodiscard]] bool serializePod(T& v, Common::StringView name, CryptoNote::ISerializer& serializer) {
  static_assert(std::is_standard_layout_v<T>, "pod must have a standard layout");
  return serializer.binary(&v, sizeof(v), name);
}

}  // namespace

namespace Crypto {

[[nodiscard]] bool serialize(chacha8_iv& chacha, Common::StringView name, CryptoNote::ISerializer& serializer) {
  return serializePod(chacha, name, serializer);
}

[[nodiscard]] bool serialize(EllipticCurveScalar& ecScalar, Common::StringView name,
                             CryptoNote::ISerializer& serializer) {
  return serializePod(ecScalar, name, serializer);
}

[[nodiscard]] bool serialize(EllipticCurvePoint& ecPoint, Common::StringView name,
                             CryptoNote::ISerializer& serializer) {
  return serializePod(ecPoint, name, serializer);
}

}  // namespace Crypto

namespace CryptoNote {

[[nodiscard]] bool serialize(AccountPublicAddress& address, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(address.spendPublicKey, "spend_public_key"), false);
  XI_RETURN_EC_IF_NOT(serializer(address.viewPublicKey, "view_public_key"), false);
  return true;
}

[[nodiscard]] bool serialize(AccountKeys& keys, ISerializer& s) {
  XI_RETURN_EC_IF_NOT(s(keys.address, "account_address"), false);
  XI_RETURN_EC_IF_NOT(s(keys.spendSecretKey, "spend_secret_key"), false);
  XI_RETURN_EC_IF_NOT(s(keys.viewSecretKey, "view_secret_key"), false);
  return true;
}

[[nodiscard]] bool serialize(KeyPair& keyPair, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(keyPair.secretKey, "secret_key"), false);
  XI_RETURN_EC_IF_NOT(serializer(keyPair.publicKey, "public_key"), false);
  return true;
}

[[nodiscard]] bool serialize(RawBlock& rawBlock, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer.binary(rawBlock.blockTemplate, "binary_block_template"), false);
  size_t count = rawBlock.transactions.size();
  XI_RETURN_EC_IF_NOT(serializer.beginArray(count, "binary_block_transactions"), false);
  rawBlock.transactions.resize(count);
  for (auto& itx : rawBlock.transactions) {
    XI_RETURN_EC_IF_NOT(serializer.binary(itx, ""), false);
  }
  XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  return true;
}

[[nodiscard]] bool serialize(LiteBlock& block, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(block.blockTemplate, "binary_block_template"), false);
  return true;
}

[[nodiscard]] bool serialize(FeeAddress& addr, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(addr.address, "address"), false);
  XI_RETURN_EC_IF_NOT(serializer(addr.amount, "amount"), false);
  return true;
}

[[nodiscard]] bool serialize(FullBlock& block, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(block.blockTemplate, "block_template"), false);
  XI_RETURN_EC_IF_NOT(serializer.beginStaticArray(block.blockTemplate.transactionHashes.size(), "transactions"), false);
  if (serializer.isInput()) {
    block.transactions.resize(block.blockTemplate.transactionHashes.size());
  }
  for (auto& iTransaction : block.transactions) {
    XI_RETURN_EC_IF_NOT(serializer(iTransaction, ""), false);
  }
  XI_RETURN_EC_IF_NOT(serializer.endArray(), false);
  return true;
}

}  // namespace CryptoNote
