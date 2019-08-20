/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#include "Serialization/YamlOutputSerializer.hpp"

#include <stack>

#include <Xi/ExternalIncludePush.h>
#include <yaml-cpp/yaml.h>
#include <Xi/ExternalIncludePop.h>

#include <Common/StringTools.h>

namespace CryptoNote {

struct YamlOutputSerializer::_Impl {
  YAML::Emitter emitter;

  enum EntityType {
    OBJECT,
    ARRAY,
  };
  std::stack<EntityType> stack{};

  explicit _Impl(std::ostream &stream_) : emitter{stream_} {
    /* */
  }

  template <typename _T>
  bool emit(_T &value, Common::StringView name) {
    XI_RETURN_EC_IF(stack.empty(), false);
    if (stack.top() == _Impl::OBJECT) {
      emitter << YAML::Key << std::string{name} << YAML::Value;
      emitter << value;
      return true;
    } else if (stack.top() == _Impl::ARRAY) {
      emitter << value;
      return true;
    } else {
      return false;
    }
  }
};

YamlOutputSerializer::YamlOutputSerializer(std::ostream &stream) : m_impl{new _Impl{stream}} {
  /* */
}

YamlOutputSerializer::~YamlOutputSerializer() {
  /* */
}

ISerializer::SerializerType YamlOutputSerializer::type() const {
  return ISerializer::OUTPUT;
}

ISerializer::FormatType YamlOutputSerializer::format() const {
  return ISerializer::HumanReadable;
}

bool YamlOutputSerializer::beginObject(Common::StringView name) {
  if (m_impl->stack.empty()) {
    m_impl->emitter << YAML::BeginMap;
    m_impl->stack.push(_Impl::OBJECT);
    return true;
  } else if (m_impl->stack.top() == _Impl::OBJECT) {
    m_impl->emitter << YAML::Key;
    m_impl->emitter << std::string{name};
    m_impl->emitter << YAML::Value << YAML::BeginMap;
    m_impl->stack.push(_Impl::OBJECT);
    return true;
  } else if (m_impl->stack.top() == _Impl::ARRAY) {
    m_impl->emitter << YAML::BeginMap;
    m_impl->stack.push(_Impl::OBJECT);
    return true;
  } else {
    return false;
  }
}

bool YamlOutputSerializer::endObject() {
  XI_RETURN_EC_IF(m_impl->stack.empty(), false);
  XI_RETURN_EC_IF_NOT(m_impl->stack.top() == _Impl::OBJECT, false);
  m_impl->stack.pop();
  m_impl->emitter << YAML::EndMap;
  return true;
}

bool YamlOutputSerializer::beginArray(size_t &, Common::StringView name) {
  if (m_impl->stack.empty()) {
    m_impl->emitter << YAML::BeginSeq;
    m_impl->stack.push(_Impl::ARRAY);
    return true;
  } else if (m_impl->stack.top() == _Impl::OBJECT) {
    m_impl->emitter << YAML::Key;
    m_impl->emitter << std::string{name};
    m_impl->emitter << YAML::Value << YAML::BeginSeq;
    m_impl->stack.push(_Impl::ARRAY);
    return true;
  } else if (m_impl->stack.top() == _Impl::ARRAY) {
    m_impl->emitter << YAML::BeginSeq;
    m_impl->stack.push(_Impl::ARRAY);
    return true;
  } else {
    return false;
  }
}

bool YamlOutputSerializer::beginStaticArray(const size_t size, Common::StringView name) {
  auto _ = size;
  return beginArray(_, name);
}

bool YamlOutputSerializer::endArray() {
  XI_RETURN_EC_IF(m_impl->stack.empty(), false);
  XI_RETURN_EC_IF_NOT(m_impl->stack.top() == _Impl::ARRAY, false);
  m_impl->stack.pop();
  m_impl->emitter << YAML::EndSeq;
  return true;
}

bool YamlOutputSerializer::operator()(uint8_t &value, Common::StringView name) {
  uint16_t _ = value;
  return m_impl->emit(_, name);
}

bool YamlOutputSerializer::operator()(int16_t &value, Common::StringView name) {
  return m_impl->emit(value, name);
}

bool YamlOutputSerializer::operator()(uint16_t &value, Common::StringView name) {
  return m_impl->emit(value, name);
}

bool YamlOutputSerializer::operator()(int32_t &value, Common::StringView name) {
  return m_impl->emit(value, name);
}

bool YamlOutputSerializer::operator()(uint32_t &value, Common::StringView name) {
  return m_impl->emit(value, name);
}

bool YamlOutputSerializer::operator()(int64_t &value, Common::StringView name) {
  return m_impl->emit(value, name);
}

bool YamlOutputSerializer::operator()(uint64_t &value, Common::StringView name) {
  return m_impl->emit(value, name);
}

bool YamlOutputSerializer::operator()(double &value, Common::StringView name) {
  return m_impl->emit(value, name);
}

bool YamlOutputSerializer::operator()(bool &value, Common::StringView name) {
  return m_impl->emit(value, name);
}

bool YamlOutputSerializer::operator()(std::string &value, Common::StringView name) {
  return m_impl->emit(value, name);
}

bool YamlOutputSerializer::binary(void *value, size_t size, Common::StringView name) {
  auto hex = Common::toHex(value, size);
  return m_impl->emit(hex, name);
}

bool YamlOutputSerializer::binary(std::string &value, Common::StringView name) {
  auto hex = Common::toHex(value.data(), value.size());
  return m_impl->emit(hex, name);
}

bool YamlOutputSerializer::binary(Xi::ByteVector &value, Common::StringView name) {
  auto hex = Common::toHex(value.data(), value.size());
  return m_impl->emit(hex, name);
}

bool YamlOutputSerializer::maybe(bool &value, Common::StringView name) {
  XI_RETURN_EC_IF(m_impl->stack.empty(), false);
  if (value) {
    return true;
  } else {
    return m_impl->emit(YAML::Null, name);
  }
}

bool YamlOutputSerializer::typeTag(TypeTag &tag, Common::StringView name) {
  XI_RETURN_EC_IF(tag.text() == TypeTag::NoTextTag, false);
  auto text = tag.text();
  return m_impl->emit(text, name);
}

bool YamlOutputSerializer::flag(std::vector<TypeTag> &flag, Common::StringView name) {
  auto size = flag.size();
  XI_RETURN_EC_IF_NOT(beginArray(size, name), false);
  for (auto &iFlag : flag) {
    auto text = iFlag.text();
    XI_RETURN_EC_IF(text == TypeTag::NoTextTag, false);
    XI_RETURN_EC_IF_NOT(m_impl->emit(text, ""), false);
  }
  XI_RETURN_EC_IF_NOT(endArray(), false);
  return true;
}

}  // namespace CryptoNote
