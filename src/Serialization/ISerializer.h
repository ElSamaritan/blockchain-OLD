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
#include <cstdint>

#include <Xi/Byte.hh>
#include <Common/StringView.h>

#include "Serialization/TypeTag.hpp"

#if !defined(NDEBUG)
#define XI_DEBUG_SERIALIZATION
#endif

namespace CryptoNote {

class ISerializer {
 public:
  enum SerializerType { INPUT, OUTPUT };

  virtual ~ISerializer() {}

  virtual SerializerType type() const = 0;

  [[nodiscard]] virtual bool beginObject(Common::StringView name) = 0;
  [[nodiscard]] virtual bool endObject() = 0;
  [[nodiscard]] virtual bool beginArray(size_t& size, Common::StringView name) = 0;
  [[nodiscard]] virtual bool beginStaticArray(const size_t size, Common::StringView name) = 0;
  [[nodiscard]] virtual bool endArray() = 0;

  [[nodiscard]] virtual bool operator()(uint8_t& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool operator()(int16_t& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool operator()(uint16_t& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool operator()(int32_t& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool operator()(uint32_t& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool operator()(int64_t& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool operator()(uint64_t& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool operator()(double& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool operator()(bool& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool operator()(std::string& value, Common::StringView name) = 0;

  // read/write binary block
  [[nodiscard]] virtual bool binary(void* value, size_t size, Common::StringView name) = 0;
  [[nodiscard]] virtual bool binary(std::string& value, Common::StringView name) = 0;
  [[nodiscard]] virtual bool binary(Xi::ByteVector& value, Common::StringView name) = 0;

  // optionals
  [[nodiscard]] virtual bool maybe(bool& value, Common::StringView name) = 0;

  // variants
  [[nodiscard]] virtual bool typeTag(TypeTag& tag, Common::StringView name) = 0;

  // flags
  [[nodiscard]] virtual bool flag(std::vector<TypeTag>& flag, Common::StringView name) = 0;

  [[nodiscard]] bool isInput() const { return type() == INPUT; }
  [[nodiscard]] bool isOutput() const { return type() == OUTPUT; }

  template <typename T>
  [[nodiscard]] bool operator()(T& value, Common::StringView name);
};

template <typename T>
[[nodiscard]] bool ISerializer::operator()(T& value, Common::StringView name) {
  return serialize(value, name, *this);
}

template <typename T>
[[nodiscard]] bool serialize(T& value, Common::StringView name, ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer.beginObject(name), false);
  XI_RETURN_EC_IF_NOT(serialize(value, serializer), false);
  XI_RETURN_EC_IF_NOT(serializer.endObject(), false);
  return true;
}

template <typename T>
[[nodiscard]] bool serialize(T& value, ISerializer& serializer) {
  return value.serialize(serializer);
}

struct Null {
  /* */
};

template <>
[[nodiscard]] inline bool serialize<Null>(Null&, Common::StringView name, ISerializer& serializer) {
  bool hasValue = true;
  XI_RETURN_EC_IF_NOT(serializer.maybe(hasValue, name), false);
  return !hasValue;
}

#ifdef __clang__
template <>
[[nodiscard]] inline bool ISerializer::operator()(size_t& value, Common::StringView name) {
  return operator()(*reinterpret_cast<uint64_t*>(&value), name);
}
#endif

}  // namespace CryptoNote

#define KV_BEGIN_SERIALIZATION [[nodiscard]] bool serialize(::CryptoNote::ISerializer& s) {
#define KV_END_SERIALIZATION \
  return true;               \
  }

#if defined(XI_DEBUG_SERIALIZATION)
#include <iostream>

#define KV_MEMBER(member)                                                                                      \
  if (!s(member, #member)) {                                                                                   \
    std::cout << "[" << __FILE__ << ":" << __LINE__ << "] member serialization failed: " #member << std::endl; \
    return false;                                                                                              \
  }

#define KV_MEMBER_RENAME(MEMBER, NAME)                                                                         \
  if (!s(MEMBER, #NAME)) {                                                                                     \
    std::cout << "[" << __FILE__ << ":" << __LINE__ << "] member serialization failed: " #MEMBER << std::endl; \
    return false;                                                                                              \
  }

#define KV_BASE(BASE_CLASS)                                                                                 \
  if (!this->BASE_CLASS::serialize(s)) {                                                                    \
    std::cout << "[" << __FILE__ << ":" << __LINE__ << "] base class serialization failed: " << #BASE_CLASS \
              << std::endl;                                                                                 \
    return false;                                                                                           \
  }
#else
#define KV_MEMBER(member)    \
  if (!s(member, #member)) { \
    return false;            \
  }

#define KV_MEMBER_RENAME(MEMBER, NAME) \
  if (!s(MEMBER, #NAME)) {             \
    return false;                      \
  }

#define KV_BASE(BASE_CLASS)              \
  if (!this->BASE_CLASS::serialize(s)) { \
    return false;                        \
  }

#endif
