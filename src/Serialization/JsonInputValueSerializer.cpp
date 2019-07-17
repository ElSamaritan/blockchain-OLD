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
#include <unordered_set>
#include <cstring>

#include <Xi/Global.hh>
#include <Xi/Exceptions.hpp>

#include "Common/StringTools.h"

using Common::JsonValue;
using namespace CryptoNote;

namespace {
static const JsonValue EmptyArray{JsonValue::ARRAY};
}

JsonInputValueSerializer::JsonInputValueSerializer(const Common::JsonValue& _value) : value(_value) {
  Xi::exceptional_if_not<Xi::InvalidArgumentError>(value.isArray() || value.isObject(),
                                                   "root json entity must be an object or array.");
  chain.push_back(&value);
}

JsonInputValueSerializer::JsonInputValueSerializer(Common::JsonValue&& _value) : value(std::move(_value)) {
  Xi::exceptional_if_not<Xi::InvalidArgumentError>(value.isArray() || value.isObject(),
                                                   "root json entity must be an object or array.");
  chain.push_back(&value);
}

JsonInputValueSerializer::~JsonInputValueSerializer() {
}

ISerializer::SerializerType JsonInputValueSerializer::type() const {
  return ISerializer::INPUT;
}

bool JsonInputValueSerializer::beginObject(Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), false);
  const JsonValue* parent = chain.back();

  if (parent->isArray()) {
    XI_RETURN_EC_IF(idxs.empty(), false);
    XI_RETURN_EC_IF_NOT(idxs.back() < parent->size(), false);
    const JsonValue& v = (*parent)[idxs.back()++];
    XI_RETURN_EC_IF_NOT(v.isObject(), false);
    chain.push_back(&v);
    return true;
  }

  if (parent->contains(std::string(name))) {
    const JsonValue& v = (*parent)(std::string(name));
    XI_RETURN_EC_IF_NOT(v.isObject(), false);
    chain.push_back(&v);
    return true;
  }

  return false;
}

bool JsonInputValueSerializer::endObject() {
  XI_RETURN_EC_IF(chain.empty(), false);
  XI_RETURN_EC_IF_NOT(chain.back()->isObject(), false);
  chain.pop_back();
  return true;
}

bool JsonInputValueSerializer::beginArray(size_t& size, Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), false);
  const JsonValue* parent = chain.back();
  std::string strName(name);

  if (parent->isArray()) {
    XI_RETURN_EC_IF_NOT(idxs.back() < parent->size(), false);
    const JsonValue& v = (*parent)[idxs.back()++];
    XI_RETURN_EC_IF_NOT(v.isArray(), false);
    size = v.size();
    chain.push_back(&v);
    idxs.push_back(0);
    return true;
  } else {
    XI_RETURN_EC_IF_NOT(parent->isObject(), false);
    if (parent->contains(strName)) {
      const JsonValue& arr = (*parent)(strName);
      XI_RETURN_EC_IF_NOT(arr.isArray(), false);
      size = arr.size();
      chain.push_back(&arr);
      idxs.push_back(0);
      return true;
    } else {
      size = 0;
      chain.push_back(std::addressof(EmptyArray));
      idxs.push_back(0);
      return true;
    }
  }
}

bool JsonInputValueSerializer::beginStaticArray(const size_t size, Common::StringView name) {
  size_t actualSize = ~size;
  XI_RETURN_EC_IF_NOT(beginArray(actualSize, name), false);
  XI_RETURN_EC_IF_NOT(actualSize == size, false);
  return true;
}

bool JsonInputValueSerializer::endArray() {
  assert(!chain.empty());
  assert(!idxs.empty());

  XI_RETURN_EC_IF(chain.empty(), false);
  XI_RETURN_EC_IF(idxs.empty(), false);
  XI_RETURN_EC_IF_NOT(chain.back()->isArray(), false);

  chain.pop_back();
  idxs.pop_back();

  return true;
}

bool JsonInputValueSerializer::operator()(uint16_t& _value, Common::StringView name) {
  return getNumber(name, _value);
}

bool JsonInputValueSerializer::operator()(int16_t& _value, Common::StringView name) {
  return getNumber(name, _value);
}

bool JsonInputValueSerializer::operator()(uint32_t& _value, Common::StringView name) {
  return getNumber(name, _value);
}

bool JsonInputValueSerializer::operator()(int32_t& _value, Common::StringView name) {
  return getNumber(name, _value);
}

bool JsonInputValueSerializer::operator()(int64_t& _value, Common::StringView name) {
  return getNumber(name, _value);
}

bool JsonInputValueSerializer::operator()(uint64_t& _value, Common::StringView name) {
  return getNumber(name, _value);
}

bool JsonInputValueSerializer::operator()(double& _value, Common::StringView name) {
  return getNumber(name, _value);
}

bool JsonInputValueSerializer::operator()(uint8_t& _value, Common::StringView name) {
  return getNumber(name, _value);
}

bool JsonInputValueSerializer::operator()(std::string& _value, Common::StringView name) {
  auto ptr = getValue(name);
  XI_RETURN_EC_IF(ptr == nullptr, false);
  XI_RETURN_EC_IF_NOT(ptr->isString(), false);
  _value = ptr->getString();
  return true;
}

bool JsonInputValueSerializer::operator()(bool& _value, Common::StringView name) {
  auto ptr = getValue(name);
  XI_RETURN_EC_IF(ptr == nullptr, false);
  XI_RETURN_EC_IF_NOT(ptr->isBool(), false);
  _value = ptr->getBool();
  return true;
}

bool JsonInputValueSerializer::binary(void* _value, size_t size, Common::StringView name) {
  auto ptr = getValue(name);
  XI_RETURN_EC_IF(ptr == nullptr, false);
  XI_RETURN_EC_IF_NOT(ptr->isString(), false);
  const auto readSize = Common::fromHex(ptr->getString(), _value, size);
  return readSize == size;
}

bool JsonInputValueSerializer::binary(std::string& _value, Common::StringView name) {
  auto ptr = getValue(name);
  if (ptr == nullptr) {
    _value = "";
    return true;
  }
  XI_RETURN_EC_IF_NOT(ptr->isString(), false);
  std::string valueHex = ptr->getString();
  _value = Common::asString(Common::fromHex(valueHex));

  return true;
}

bool JsonInputValueSerializer::binary(Xi::ByteVector& _value, Common::StringView name) {
  std::string blob{};
  XI_RETURN_EC_IF_NOT(binary(blob, name), false);
  if (!blob.empty()) {
    _value.resize(blob.size());
    std::memcpy(_value.data(), blob.data(), _value.size());
    return true;
  } else {
    return true;
  }
}

bool JsonInputValueSerializer::maybe(bool& _value, Common::StringView name) {
  auto maybeValue = getValue(name);
  _value = maybeValue == nullptr ? false : !maybeValue->isNil();
  return true;
}

bool JsonInputValueSerializer::typeTag(TypeTag& tag, Common::StringView name) {
  TypeTag::text_type tTag{TypeTag::NoTextTag};
  XI_RETURN_EC_IF_NOT(this->operator()(tTag, name), false);
  XI_RETURN_EC_IF(tTag == TypeTag::NoTextTag, false);
  tag = TypeTag{TypeTag::NoBinaryTag, tTag};
  return true;
}

bool JsonInputValueSerializer::flag(std::vector<TypeTag>& flag, Common::StringView name) {
  flag.clear();
  bool hasFlag = false;
  XI_RETURN_EC_IF_NOT(maybe(hasFlag, name), false);
  XI_RETURN_EC_IF_NOT(hasFlag, true);

  size_t size = 0;
  XI_RETURN_EC_IF_NOT(beginArray(size, name), false);
  XI_RETURN_EC_IF(size > 14, false);

  std::unordered_set<std::string> processedFlags{};
  for (size_t i = 0; i < size; ++i) {
    TypeTag iTag = TypeTag::Null;
    XI_RETURN_EC_IF_NOT(typeTag(iTag, ""), false);
    XI_RETURN_EC_IF_NOT(processedFlags.insert(iTag.text()).second, false);
    flag.push_back(iTag);
  }
  XI_RETURN_EC_IF_NOT(endArray(), false);
  return true;
}

const JsonValue* JsonInputValueSerializer::getValue(Common::StringView name) {
  XI_RETURN_EC_IF(chain.empty(), nullptr);
  const JsonValue& val = *chain.back();
  if (val.isArray()) {
    XI_RETURN_EC_IF(idxs.empty(), nullptr);
    XI_RETURN_EC_IF_NOT(idxs.back() < val.size(), nullptr);
    return &val[idxs.back()++];
  }

  std::string strName(name);
  return val.contains(strName) ? &val(strName) : nullptr;
}
