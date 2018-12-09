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

#include "JsonInputValueSerializer.h"

#include <cassert>
#include <stdexcept>

#include "Common/StringTools.h"

using Common::JsonValue;
using namespace CryptoNote;

JsonInputValueSerializer::JsonInputValueSerializer(const Common::JsonValue& _value) {
  if (!_value.isObject()) {
    throw std::runtime_error("Serializer doesn't support this type of serialization: Object expected.");
  }

  chain.push_back(&_value);
}

JsonInputValueSerializer::JsonInputValueSerializer(Common::JsonValue&& _value) : value(std::move(_value)) {
  if (!this->value.isObject()) {
    throw std::runtime_error("Serializer doesn't support this type of serialization: Object expected.");
  }

  chain.push_back(&this->value);
}

JsonInputValueSerializer::~JsonInputValueSerializer() {}

ISerializer::SerializerType JsonInputValueSerializer::type() const { return ISerializer::INPUT; }

bool JsonInputValueSerializer::beginObject(Common::StringView name) {
  const JsonValue* parent = chain.back();

  if (parent->isArray()) {
    const JsonValue& v = (*parent)[idxs.back()++];
    chain.push_back(&v);
    return true;
  }

  if (parent->contains(std::string(name))) {
    const JsonValue& v = (*parent)(std::string(name));
    chain.push_back(&v);
    return true;
  }

  return false;
}

void JsonInputValueSerializer::endObject() {
  assert(!chain.empty());
  chain.pop_back();
}

bool JsonInputValueSerializer::beginArray(size_t& size, Common::StringView name) {
  const JsonValue* parent = chain.back();
  std::string strName(name);

  if (parent->contains(strName)) {
    const JsonValue& arr = (*parent)(strName);
    size = arr.size();
    chain.push_back(&arr);
    idxs.push_back(0);
    return true;
  }

  size = 0;
  return false;
}

void JsonInputValueSerializer::endArray() {
  assert(!chain.empty());
  assert(!idxs.empty());

  chain.pop_back();
  idxs.pop_back();
}

bool JsonInputValueSerializer::operator()(uint16_t& _value, Common::StringView name) { return getNumber(name, _value); }

bool JsonInputValueSerializer::operator()(int16_t& _value, Common::StringView name) { return getNumber(name, _value); }

bool JsonInputValueSerializer::operator()(uint32_t& _value, Common::StringView name) { return getNumber(name, _value); }

bool JsonInputValueSerializer::operator()(int32_t& _value, Common::StringView name) { return getNumber(name, _value); }

bool JsonInputValueSerializer::operator()(int64_t& _value, Common::StringView name) { return getNumber(name, _value); }

bool JsonInputValueSerializer::operator()(uint64_t& _value, Common::StringView name) { return getNumber(name, _value); }

bool JsonInputValueSerializer::operator()(double& _value, Common::StringView name) { return getNumber(name, _value); }

bool JsonInputValueSerializer::operator()(uint8_t& _value, Common::StringView name) { return getNumber(name, _value); }

bool JsonInputValueSerializer::operator()(std::string& _value, Common::StringView name) {
  auto ptr = getValue(name);
  if (ptr == nullptr) {
    return false;
  }
  _value = ptr->getString();
  return true;
}

bool JsonInputValueSerializer::operator()(bool& _value, Common::StringView name) {
  auto ptr = getValue(name);
  if (ptr == nullptr) {
    return false;
  }
  _value = ptr->getBool();
  return true;
}

bool JsonInputValueSerializer::binary(void* _value, size_t size, Common::StringView name) {
  auto ptr = getValue(name);
  if (ptr == nullptr) {
    return false;
  }

  Common::fromHex(ptr->getString(), _value, size);
  return true;
}

bool JsonInputValueSerializer::binary(std::string& _value, Common::StringView name) {
  auto ptr = getValue(name);
  if (ptr == nullptr) {
    return false;
  }

  std::string valueHex = ptr->getString();
  _value = Common::asString(Common::fromHex(valueHex));

  return true;
}

const JsonValue* JsonInputValueSerializer::getValue(Common::StringView name) {
  const JsonValue& val = *chain.back();
  if (val.isArray()) {
    return &val[idxs.back()++];
  }

  std::string strName(name);
  return val.contains(strName) ? &val(strName) : nullptr;
}
