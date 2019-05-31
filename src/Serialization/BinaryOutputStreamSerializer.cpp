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

#include "BinaryOutputStreamSerializer.h"

#include <cassert>
#include <stdexcept>

#include <Xi/Global.hh>

#include "Common/StreamTools.h"

using namespace Common;

namespace CryptoNote {

ISerializer::SerializerType BinaryOutputStreamSerializer::type() const { return ISerializer::OUTPUT; }

bool BinaryOutputStreamSerializer::beginObject(Common::StringView name) {
  XI_UNUSED(name);
  return true;
}

void BinaryOutputStreamSerializer::endObject() {}

bool BinaryOutputStreamSerializer::beginArray(size_t& size, Common::StringView name) {
  XI_UNUSED(name);
  writeVarint(stream, size);
  return true;
}

bool BinaryOutputStreamSerializer::beginStaticArray(const size_t size, StringView name) {
  XI_UNUSED(size, name);
  return true;
}

void BinaryOutputStreamSerializer::endArray() {}

bool BinaryOutputStreamSerializer::operator()(uint8_t& value, Common::StringView name) {
  XI_UNUSED(name);
  write(stream, value);
  return true;
}

bool BinaryOutputStreamSerializer::operator()(uint16_t& value, Common::StringView name) {
  XI_UNUSED(name);
  writeVarint(stream, value);
  return true;
}

bool BinaryOutputStreamSerializer::operator()(int16_t& value, Common::StringView name) {
  XI_UNUSED(name);
  writeVarint(stream, static_cast<uint16_t>(value));
  return true;
}

bool BinaryOutputStreamSerializer::operator()(uint32_t& value, Common::StringView name) {
  XI_UNUSED(name);
  writeVarint(stream, value);
  return true;
}

bool BinaryOutputStreamSerializer::operator()(int32_t& value, Common::StringView name) {
  XI_UNUSED(name);
  writeVarint(stream, static_cast<uint32_t>(value));
  return true;
}

bool BinaryOutputStreamSerializer::operator()(int64_t& value, Common::StringView name) {
  XI_UNUSED(name);
  writeVarint(stream, static_cast<uint64_t>(value));
  return true;
}

bool BinaryOutputStreamSerializer::operator()(uint64_t& value, Common::StringView name) {
  XI_UNUSED(name);
  writeVarint(stream, value);
  return true;
}

bool BinaryOutputStreamSerializer::operator()(bool& value, Common::StringView name) {
  uint8_t boolVal = value ? 0b01010101 : 0b00101010;
  XI_RETURN_EC_IF_NOT(this->operator()(boolVal, name), false);
  return true;
}

bool BinaryOutputStreamSerializer::operator()(std::string& value, Common::StringView name) {
  XI_UNUSED(name);
  writeVarint(stream, value.size());
  checkedWrite(value.data(), value.size());
  return true;
}

bool BinaryOutputStreamSerializer::binary(void* value, size_t size, Common::StringView name) {
  XI_UNUSED(name);
  checkedWrite(static_cast<const char*>(value), size);
  return true;
}

bool BinaryOutputStreamSerializer::binary(std::string& value, Common::StringView name) {
  // write as string (with size prefix)
  return (*this)(value, name);
}

bool BinaryOutputStreamSerializer::maybe(bool& value, Common::StringView name) { return this->operator()(value, name); }

bool BinaryOutputStreamSerializer::typeTag(TypeTag& tag, Common::StringView name) {
  XI_RETURN_EC_IF(tag.binary() == TypeTag::NoBinaryTag, false);
  auto bTag = tag.binary();
  return this->operator()(bTag, name);
}

bool BinaryOutputStreamSerializer::flag(std::vector<TypeTag>& flag, Common::StringView name) {
  uint16_t nativeFlag = 0;
  for (const auto& iFlag : flag) {
    XI_RETURN_EC_IF(iFlag.binary() == TypeTag::NoBinaryTag, false);
    XI_RETURN_EC_IF(iFlag.binary() > 15, false);
    assert(iFlag.binary() > 0);
    nativeFlag |= (1 << (iFlag.binary() - 1));
  }
  return this->operator()(nativeFlag, name);
}

bool BinaryOutputStreamSerializer::operator()(double& value, Common::StringView name) {
  XI_UNUSED(value, name);
  throw std::runtime_error("double serialization is not supported in BinaryOutputStreamSerializer");
}

void BinaryOutputStreamSerializer::checkedWrite(const char* buf, size_t size) { write(stream, buf, size); }

}  // namespace CryptoNote
