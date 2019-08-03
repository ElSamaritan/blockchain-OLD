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
#include <optional>

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
  enum FormatType { HumanReadable, Machinery };

  virtual ~ISerializer() {
  }

  virtual SerializerType type() const = 0;
  virtual FormatType format() const = 0;

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

  [[nodiscard]] bool isInput() const {
    return type() == INPUT;
  }
  [[nodiscard]] bool isOutput() const {
    return type() == OUTPUT;
  }

  [[nodiscard]] bool isHumanReadable() const {
    return format() == HumanReadable;
  }
  [[nodiscard]] bool isMachinery() const {
    return format() == Machinery;
  }

  template <typename T, typename... _ArgsT>
  [[nodiscard]] bool operator()(T& value, Common::StringView name, _ArgsT&&... args);

  template <typename _T>
  [[nodiscard]] bool optional(bool expected, std::optional<_T>& value, Common::StringView name);
  template <typename _T>
  [[nodiscard]] bool optional(bool expected, std::vector<_T>& value, Common::StringView name);
};

template <typename T, typename... _ArgsT>
[[nodiscard]] bool ISerializer::operator()(T& value, Common::StringView name, _ArgsT&&... args) {
  return serialize(value, name, *this, std::forward<_ArgsT>(args)...);
}

template <typename T>
[[nodiscard]] bool ISerializer::optional(bool expected, std::optional<T>& value, Common::StringView name) {
  if (isInput()) {
    if (expected) {
      value.emplace();
      return (*this)(*value, name);
    } else {
      value = std::nullopt;
      XI_RETURN_SC(true);
    }
  } else if (isOutput()) {
    if (expected) {
      XI_RETURN_EC_IF_NOT(value.has_value(), false);
      return (*this)(*value, name);
    } else {
      XI_RETURN_EC_IF(value.has_value(), false);
      XI_RETURN_SC(true);
    }
  } else {
    XI_RETURN_EC(false);
  }
}

template <typename T>
[[nodiscard]] bool ISerializer::optional(bool expected, std::vector<T>& value, Common::StringView name) {
  if (isInput()) {
    if (expected) {
      return (*this)(value, name);
    } else {
      value.clear();
      XI_RETURN_SC(true);
    }
  } else if (isOutput()) {
    if (expected) {
      XI_RETURN_EC_IF(value.empty(), false);
      return (*this)(value, name);
    } else {
      XI_RETURN_EC_IF_NOT(value.empty(), false);
      XI_RETURN_SC(true);
    }
  } else {
    XI_RETURN_EC(false);
  }
}

template <typename T, typename... _ArgsT>
[[nodiscard]] bool serialize(T& value, Common::StringView name, ISerializer& serializer, _ArgsT&&... args) {
  XI_RETURN_EC_IF_NOT(serializer.beginObject(name), false);
  XI_RETURN_EC_IF_NOT(serialize(value, serializer, std::forward<_ArgsT>(args)...), false);
  XI_RETURN_EC_IF_NOT(serializer.endObject(), false);
  return true;
}

template <typename T, typename... _ArgsT>
[[nodiscard]] bool serialize(T& value, ISerializer& serializer, _ArgsT&&... args) {
  return value.serialize(serializer, std::forward<_ArgsT>(args)...);
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

#define KV_PUSH_VIRTUAL_OBJECT(NAME)                                                                           \
  if (!s.beginObject(#NAME)) {                                                                                 \
    std::cout << "[" << __FILE__ << ":" << __LINE__ << "] virtual object push serialization failed: " << #NAME \
              << std::endl;                                                                                    \
    return false;                                                                                              \
  }

#define KV_POP_VIRTUAL_OBJECT()                                                                                   \
  if (!s.endObject()) {                                                                                           \
    std::cout << "[" << __FILE__ << ":" << __LINE__ << "] virtual object pop serialization failed." << std::endl; \
    return false;                                                                                                 \
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

#define KV_PUSH_VIRTUAL_OBJECT(NAME) \
  if (!s.beginObject(#NAME)) {       \
    return false;                    \
  }

#define KV_POP_VIRTUAL_OBJECT() \
  if (!s.endObject()) {         \
    return false;               \
  }

#endif
