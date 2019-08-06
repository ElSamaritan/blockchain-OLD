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

#include "JsonOutputStreamSerializer.h"

#include <cassert>
#include <stdexcept>

#include <Xi/Global.hh>
#include <Xi/Algorithm/String.h>

#include "Common/StringTools.h"

using Common::JsonValue;
using namespace CryptoNote;

namespace CryptoNote {
std::ostream& operator<<(std::ostream& out, const JsonOutputStreamSerializer& enumerator) {
  out << enumerator.root;
  return out;
}
}  // namespace CryptoNote

namespace {

template <typename T>
void insertOrPush(JsonValue& js, Common::StringView name, const T& value) {
  if (js.isArray()) {
    js.pushBack(JsonValue(value));
  } else {
    js.insert(std::string(name), JsonValue(value));
  }
}

}  // namespace

JsonOutputStreamSerializer::JsonOutputStreamSerializer() : root(JsonValue::OBJECT) {
  chain.push_back(&root);
}

JsonOutputStreamSerializer::~JsonOutputStreamSerializer() {
}

ISerializer::SerializerType JsonOutputStreamSerializer::type() const {
  return ISerializer::OUTPUT;
}

ISerializer::FormatType JsonOutputStreamSerializer::format() const {
  return ISerializer::HumanReadable;
}

bool JsonOutputStreamSerializer::beginObject(Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), false);
  JsonValue& parent = *chain.back();
  JsonValue obj(JsonValue::OBJECT);
  if (parent.isObject()) {
    chain.push_back(&parent.insert(std::string(name), obj));
  } else {
    chain.push_back(&parent.pushBack(obj));
  }
  return true;
}

bool JsonOutputStreamSerializer::endObject() {
  assert(!chain.empty());
  XI_RETURN_EC_IF(chain.empty(), false);
  XI_RETURN_EC_IF_NOT(chain.back()->isObject(), false);
  chain.pop_back();
  return true;
}

bool JsonOutputStreamSerializer::beginArray(size_t& size, Common::StringView name) {
  XI_UNUSED(size);
  XI_RETURN_EC_IF(chain.empty(), false);
  JsonValue val(JsonValue::ARRAY);
  auto& parent = chain.back();
  if (parent->isArray()) {
    JsonValue& res = parent->pushBack(val);
    chain.push_back(&res);
    return true;
  } else if (parent->isObject()) {
    JsonValue& res = parent->insert(std::string(name), val);
    chain.push_back(&res);
    return true;
  } else {
    return false;
  }
}

bool JsonOutputStreamSerializer::beginStaticArray(const size_t size, Common::StringView name) {
  XI_UNUSED(size);
  auto _size = size;
  return beginArray(_size, name);
}

bool JsonOutputStreamSerializer::endArray() {
  assert(!chain.empty());
  XI_RETURN_EC_IF(chain.empty(), false);
  chain.pop_back();
  return true;
}

bool JsonOutputStreamSerializer::operator()(uint64_t& value, Common::StringView name) {
  int64_t v = static_cast<int64_t>(value);
  return operator()(v, name);
}

bool JsonOutputStreamSerializer::operator()(uint16_t& value, Common::StringView name) {
  uint64_t v = static_cast<uint64_t>(value);
  return operator()(v, name);
}

bool JsonOutputStreamSerializer::operator()(int16_t& value, Common::StringView name) {
  int64_t v = static_cast<int64_t>(value);
  return operator()(v, name);
}

bool JsonOutputStreamSerializer::operator()(uint32_t& value, Common::StringView name) {
  uint64_t v = static_cast<uint64_t>(value);
  return operator()(v, name);
}

bool JsonOutputStreamSerializer::operator()(int32_t& value, Common::StringView name) {
  int64_t v = static_cast<int64_t>(value);
  return operator()(v, name);
}

bool JsonOutputStreamSerializer::operator()(int64_t& value, Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), false);
  insertOrPush(*chain.back(), name, value);
  return true;
}

bool JsonOutputStreamSerializer::operator()(double& value, Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), false);
  insertOrPush(*chain.back(), name, value);
  return true;
}

namespace {
const std::vector<std::pair<std::string, std::string>> escapeSequences{{
    {"\b", "\\b"},
    {"\f", "\\f"},
    {"\n", "\\n"},
    {"\r", "\\r"},
    {"\t", "\\t"},
    {"\"", "\\\""},
    {"\\", "\\\\"},
}};
}  // namespace

bool JsonOutputStreamSerializer::operator()(std::string& value, Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), false);
  if (isOutput()) {
    std::string cp = value;
    for (auto i = escapeSequences.rbegin(); i != escapeSequences.rend(); ++i) {
      cp = Xi::replace(cp, i->first, i->second);
    }
    insertOrPush(*chain.back(), name, cp);
  } else {
    insertOrPush(*chain.back(), name, value);
    for (auto i = escapeSequences.begin(); i != escapeSequences.end(); ++i) {
      value = Xi::replace(value, i->second, i->first);
    }
  }
  return true;
}

bool JsonOutputStreamSerializer::operator()(uint8_t& value, Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), false);
  insertOrPush(*chain.back(), name, static_cast<int64_t>(value));
  return true;
}

bool JsonOutputStreamSerializer::operator()(bool& value, Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), false);
  insertOrPush(*chain.back(), name, value);
  return true;
}

bool JsonOutputStreamSerializer::binary(void* value, size_t size, Common::StringView name) {
  std::string hex = Common::toHex(value, size);
  return (*this)(hex, name);
}

bool JsonOutputStreamSerializer::binary(std::string& value, Common::StringView name) {
  return binary(value.data(), value.size(), name);
}

bool JsonOutputStreamSerializer::binary(Xi::ByteVector& value, Common::StringView name) {
  return binary(value.data(), value.size(), name);
}

bool JsonOutputStreamSerializer::maybe(bool& value, Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), false);
  if (!value) {
    insertOrPush(*chain.back(), name, JsonValue::NIL);
  }
  return true;
}

bool JsonOutputStreamSerializer::typeTag(TypeTag& tag, Common::StringView name) {
  XI_RETURN_EC_IF(tag.text() == TypeTag::NoTextTag, false);
  TypeTag::text_type tTag = tag.text();
  XI_RETURN_EC_IF_NOT(this->operator()(tTag, name), false);
  return true;
}

bool JsonOutputStreamSerializer::flag(std::vector<TypeTag>& flag, Common::StringView name) {
  if (flag.empty()) {
    bool hasFlag = false;
    return maybe(hasFlag, name);
  } else {
    size_t count = flag.size();
    XI_RETURN_EC_IF_NOT(beginArray(count, name), false);
    for (size_t i = 0; i < count; ++i) {
      XI_RETURN_EC_IF_NOT(typeTag(flag[i], ""), false);
    }
    XI_RETURN_EC_IF_NOT(endArray(), false);
    return true;
  }
}
