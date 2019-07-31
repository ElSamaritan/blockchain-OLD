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

#include "BinaryInputStreamSerializer.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

#include <Common/StreamTools.h>
#include <Xi/Config.h>
#include <Xi/Global.hh>

#include "SerializationOverloads.h"

using namespace Common;

namespace CryptoNote {

namespace {

template <typename T>
void readInteger(IInputStream& s, T& i, bool useVarint) {
  if (useVarint) {
    Common::readVarint(s, i);
  } else {
    Common::read(s, i);
  }
}

}  // namespace

ISerializer::SerializerType BinaryInputStreamSerializer::type() const {
  return ISerializer::INPUT;
}

bool BinaryInputStreamSerializer::useVarInt() {
  return m_varintUse;
}

void BinaryInputStreamSerializer::setUseVarInt(bool use) {
  m_varintUse = use;
}

bool BinaryInputStreamSerializer::beginObject(Common::StringView name) {
  XI_UNUSED(name);
  return true;
}

bool BinaryInputStreamSerializer::endObject() {
  return true;
}

bool BinaryInputStreamSerializer::beginArray(size_t& size, Common::StringView name) {
  XI_UNUSED(name);
  uint64_t _ = size;
  readInteger(stream, _, useVarInt());
  size = _;
  return true;
}

bool BinaryInputStreamSerializer::beginStaticArray(const size_t size, StringView name) {
  XI_UNUSED(size, name);
  return true;
}

bool BinaryInputStreamSerializer::endArray() {
  return true;
}

bool BinaryInputStreamSerializer::operator()(uint8_t& value, Common::StringView name) {
  XI_UNUSED(name);
  readInteger(stream, value, false);
  return true;
}

bool BinaryInputStreamSerializer::operator()(uint16_t& value, Common::StringView name) {
  XI_UNUSED(name);
  readInteger(stream, value, useVarInt());
  return true;
}

bool BinaryInputStreamSerializer::operator()(int16_t& value, Common::StringView name) {
  XI_UNUSED(name);
  readInteger(stream, value, useVarInt());
  return true;
}

bool BinaryInputStreamSerializer::operator()(uint32_t& value, Common::StringView name) {
  XI_UNUSED(name);
  readInteger(stream, value, useVarInt());
  return true;
}

bool BinaryInputStreamSerializer::operator()(int32_t& value, Common::StringView name) {
  XI_UNUSED(name);
  readInteger(stream, value, useVarInt());
  return true;
}

bool BinaryInputStreamSerializer::operator()(int64_t& value, Common::StringView name) {
  XI_UNUSED(name);
  readInteger(stream, value, useVarInt());
  return true;
}

bool BinaryInputStreamSerializer::operator()(uint64_t& value, Common::StringView name) {
  XI_UNUSED(name);
  readInteger(stream, value, useVarInt());
  return true;
}

bool BinaryInputStreamSerializer::operator()(bool& value, Common::StringView name) {
  XI_UNUSED(name);
  uint8_t byte;
  readInteger(stream, byte, false);
  if (byte == 0b01010101) {
    value = true;
    return true;
  } else if (byte == 0b00101010) {
    value = false;
    return true;
  } else {
    return false;
  }
}

bool BinaryInputStreamSerializer::operator()(std::string& value, Common::StringView name) {
  XI_UNUSED(name);
  uint64_t size;
  readInteger(stream, size, useVarInt());

  if (size > 0) {
    std::vector<char> temp;
    temp.resize(size);
    checkedRead(&temp[0], size);
    value.reserve(size);
    value.assign(&temp[0], size);
  } else {
    value.clear();
  }

  return true;
}

bool BinaryInputStreamSerializer::binary(void* value, size_t size, Common::StringView name) {
  XI_UNUSED(name);
  checkedRead(static_cast<char*>(value), size);
  return true;
}

bool BinaryInputStreamSerializer::binary(std::string& value, Common::StringView name) {
  return (*this)(value, name);
}

bool BinaryInputStreamSerializer::binary(Xi::ByteVector& value, StringView name) {
  uint64_t size = 0;
  readInteger(stream, size, useVarInt());
  if (size > 0) {
    value.resize(size);
    return this->binary(value.data(), value.size(), name);
  } else {
    value.clear();
    return true;
  }
}

bool BinaryInputStreamSerializer::maybe(bool& value, Common::StringView name) {
  return this->operator()(value, name);
}

bool BinaryInputStreamSerializer::typeTag(TypeTag& tag, Common::StringView name) {
  TypeTag::binary_type bTag = TypeTag::NoBinaryTag;
  XI_RETURN_EC_IF_NOT(this->operator()(bTag, name), false);
  XI_RETURN_EC_IF(bTag == TypeTag::NoBinaryTag, false);
  tag = TypeTag{bTag, TypeTag::NoTextTag};
  return true;
}

bool BinaryInputStreamSerializer::flag(std::vector<TypeTag>& flag, Common::StringView name) {
  XI_UNUSED(name);
  uint16_t nativeFlag = 0;
  readInteger(stream, nativeFlag, false);
  XI_RETURN_EC_IF(nativeFlag > (1 << 14), false);
  flag.clear();
  for (size_t i = 0; (1 << i) <= nativeFlag; ++i) {
    if ((nativeFlag & (1 << i))) {
      flag.emplace_back(i + 1, TypeTag::NoTextTag);
    }
  }
  return true;
}

bool BinaryInputStreamSerializer::operator()(double& value, Common::StringView name) {
  XI_UNUSED(value, name);
  throw std::runtime_error("double serialization is not supported in BinaryInputStreamSerializer");
}

void BinaryInputStreamSerializer::checkedRead(char* buf, size_t size) {
  read(stream, buf, size);
}

}  // namespace CryptoNote
