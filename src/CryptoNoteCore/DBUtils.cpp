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

#include "DBUtils.h"

#include <Xi/Global.hh>

namespace CryptoNote {
namespace DB {
std::string serialize(const RawBlock& value, const std::string& name) {
  std::stringstream ss;
  Common::StdOutputStream stream(ss);
  CryptoNote::BinaryOutputStreamSerializer serializer(stream);

  Xi::exceptional_if_not<SerializationError>(serializer.beginObject(name));
  Xi::exceptional_if_not<SerializationError>(serializer(const_cast<RawBlock&>(value).block, "raw_block"));
  Xi::exceptional_if_not<SerializationError>(serializer(const_cast<RawBlock&>(value).transactions, "raw_transactions"));
  serializer.endObject();

  return ss.str();
}

void deserialize(const std::string& serialized, RawBlock& value, const std::string& name) {
  std::stringstream ss(serialized);
  Common::StdInputStream stream(ss);
  CryptoNote::BinaryInputStreamSerializer serializer(stream);
  Xi::exceptional_if_not<DeserializationError>(serializer.beginObject(name));
  Xi::exceptional_if_not<DeserializationError>(serializer(value.block, "raw_block"));
  Xi::exceptional_if_not<DeserializationError>(serializer(value.transactions, "raw_transactions"));
  serializer.endObject();
}
}  // namespace DB
}  // namespace CryptoNote
