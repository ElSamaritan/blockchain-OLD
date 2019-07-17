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

#include <string>
#include <sstream>
#include <iterator>

#include <Xi/Exceptional.hpp>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/BinaryInputStreamSerializer.h>
#include <Serialization/BinaryOutputStreamSerializer.h>

#include <Common/ByteSpanInputStream.hpp>
#include "Common/StdOutputStream.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteCore/CryptoNoteSerialization.h"
#include "Common/StdInputStream.h"

namespace CryptoNote {
namespace DB {

XI_DECLARE_EXCEPTIONAL_CATEGORY(Database)
XI_DECLARE_EXCEPTIONAL_INSTANCE(Serialization, "object serialization failed", Database)
XI_DECLARE_EXCEPTIONAL_INSTANCE(Deserialization, "object deserialization failed", Database)

struct KeyPrefix {
  Xi::ByteArray<2> Prefix;

  explicit KeyPrefix(const Xi::Byte prefix) {
    this->Prefix[0] = prefix;
    this->Prefix[1] = 0xFF;
  }

  bool operator==(const KeyPrefix& rhs) const {
    return Prefix == rhs.Prefix;
  }
  bool operator!=(const KeyPrefix& rhs) const {
    return Prefix != rhs.Prefix;
  }
};
static inline KeyPrefix makePrefix(const Xi::Byte prefix) {
  return KeyPrefix{prefix};
}

struct NoKey {
  KV_BEGIN_SERIALIZATION
  XI_UNUSED(s)
  KV_END_SERIALIZATION

  bool operator==(const NoKey&) const {
    return true;
  }
  bool operator!=(const NoKey&) const {
    return false;
  }
};
static inline NoKey noKey() {
  return NoKey{};
}

const KeyPrefix BLOCK_INDEX_TO_KEY_IMAGE_PREFIX = makePrefix(0x01);
const KeyPrefix BLOCK_INDEX_TO_TX_HASHES_PREFIX = makePrefix(0x02);
const KeyPrefix BLOCK_INDEX_TO_TRANSACTION_INFO_PREFIX = makePrefix(0x03);
const KeyPrefix BLOCK_INDEX_TO_RAW_BLOCK_PREFIX = makePrefix(0x04);
const KeyPrefix BLOCK_HASH_TO_BLOCK_INDEX_PREFIX = makePrefix(0x05);
const KeyPrefix BLOCK_INDEX_TO_BLOCK_INFO_PREFIX = makePrefix(0x06);
const KeyPrefix KEY_IMAGE_TO_BLOCK_INDEX_PREFIX = makePrefix(0x07);
const KeyPrefix BLOCK_INDEX_TO_BLOCK_HASH_PREFIX = makePrefix(0x08);
const KeyPrefix TRANSACTION_HASH_TO_TRANSACTION_INFO_PREFIX = makePrefix(0x09);
const KeyPrefix KEY_OUTPUT_AMOUNT_PREFIX = makePrefix(0x0A);
const KeyPrefix KEY_OUTPUT_AMOUNT_COUNT_PREFIX = makePrefix(0xAA);
const KeyPrefix CLOSEST_TIMESTAMP_BLOCK_INDEX_PREFIX = makePrefix(0x0B);
const KeyPrefix PAYMENT_ID_TO_TX_HASH_PREFIX = makePrefix(0x0C);
const KeyPrefix TIMESTAMP_TO_BLOCKHASHES_PREFIX = makePrefix(0x0D);
const KeyPrefix KEY_OUTPUT_AMOUNTS_COUNT_PREFIX = makePrefix(0x0E);
const KeyPrefix LAST_BLOCK_INDEX_KEY = makePrefix(0x0F);
const KeyPrefix KEY_OUTPUT_AMOUNTS_COUNT_KEY = makePrefix(0x10);
const KeyPrefix TRANSACTIONS_COUNT_KEY = makePrefix(0x11);
const KeyPrefix KEY_OUTPUT_KEY_PREFIX = makePrefix(0x12);

template <typename _KeyT>
std::string serializeKey(const KeyPrefix& cprefix, const _KeyT& ckey) {
  using namespace Xi;

  auto& prefix = const_cast<KeyPrefix&>(cprefix);
  auto& key = const_cast<_KeyT&>(ckey);

  std::stringstream builder{};
  Common::StdOutputStream stream{builder};
  BinaryOutputStreamSerializer serializer{stream};
  serializer.setUseVarInt(false);

  exceptional_if_not<SerializationError>(serializer.binary(prefix.Prefix.data(), prefix.Prefix.size(), "prefix"),
                                         "database prefix serialization failed");
  exceptional_if_not<SerializationError>(serializer(key, "key"), "database key serialization failed");

  return builder.str();
}

template <typename _KeyT, typename _ValueT>
std::pair<std::string, std::string> serialize(const KeyPrefix& cprefix, const _KeyT& ckey, const _ValueT& cvalue) {
  using namespace Xi;

  auto& value = const_cast<_ValueT&>(cvalue);

  std::stringstream builder{};
  Common::StdOutputStream stream{builder};
  BinaryOutputStreamSerializer serializer{stream};

  auto key = serializeKey(cprefix, ckey);
  exceptional_if_not<SerializationError>(serializer(value, "value"), "database value serialization failed");

  return std::make_pair(std::move(key), builder.str());
}

template <typename _KeyT, typename _ValueT>
void serializeKeys(std::vector<std::string>& rawKeys, const KeyPrefix& prefix,
                   const std::unordered_map<_KeyT, _ValueT>& values) {
  for (const auto& valuePair : values) {
    rawKeys.emplace_back(serializeKey(prefix, valuePair.first));
  }
}

template <typename _KeyT, typename _ValueT>
void deserialize(const std::string& serialized, _ValueT& value, const _KeyT& expectedKey,
                 const KeyPrefix& expectedPrefix) {
  using namespace Xi;

  XI_UNUSED(expectedKey, expectedPrefix);

  Common::ByteSpanInputStream stream{Xi::asConstByteSpan(serialized)};
  CryptoNote::BinaryInputStreamSerializer serializer(stream);

  exceptional_if_not<DeserializationError>(serializer(value, "value"), "database value deserialization failed");
  exceptional_if_not<DeserializationError>(stream.isEndOfStream(), "database deserialization has left overs");
}

template <typename _ValueT, typename _IteratorT>
void deserializeValue(std::pair<_ValueT, bool>& container, _IteratorT& combinedIter, const KeyPrefix& prefix) {
  const bool wasRequested = container.second;
  if (wasRequested) {
    const bool found = boost::get<1>(*combinedIter);
    if (found) {
      const auto& blob = boost::get<0>(*combinedIter);
      DB::deserialize(blob, container.first, DB::noKey(), prefix);
    } else {
      container = {_ValueT{}, false};
    }
    std::advance(combinedIter, 1);
  }
}

template <typename _KeyT, typename _ValueT, typename _IteratorT>
void deserializeValues(std::unordered_map<_KeyT, _ValueT>& container, _IteratorT& combinedIter,
                       const KeyPrefix& prefix) {
  for (auto i = container.begin(); i != container.end(); std::advance(combinedIter, 1)) {
    const bool found = boost::get<1>(*combinedIter);
    if (found) {
      const auto& blob = boost::get<0>(*combinedIter);
      DB::deserialize(blob, i->second, i->first, prefix);
      std::advance(i, 1);
    } else {
      i = container.erase(i);
    }
  }
}
}  // namespace DB
}  // namespace CryptoNote
