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

#include "Serialization/ConsoleOutputSerializer.hpp"

#include <stack>

#include <Xi/ExternalIncludePush.h>
#include <yaml-cpp/yaml.h>
#include <rang.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Common/StringTools.h>

namespace CryptoNote {

struct ConsoleOutputSerializer::_Impl {
  YAML::Emitter emitter;
  std::ostream &stream;

  enum EntityType {
    OBJECT,
    ARRAY,
  };
  std::stack<EntityType> stack{};

  explicit _Impl(std::ostream &stream_) : emitter{stream_}, stream{stream_} {
    rang::setControlMode(rang::control::Auto);
  }

  template <typename _T>
  bool emit(_T &value, Common::StringView name, rang::fg fg = rang::fg::reset, rang::style st = rang::style::reset) {
    XI_RETURN_EC_IF(stack.empty(), false);
    if (stack.top() == _Impl::OBJECT) {
      stream << rang::fg::green;
      emitter << YAML::Key << std::string{name} << YAML::Value;
      stream << fg << st;
      emitter << value;
      stream << rang::fg::reset << rang::style::reset;
      return true;
    } else if (stack.top() == _Impl::ARRAY) {
      stream << fg << st;
      emitter << value;
      stream << rang::fg::reset << rang::style::reset;
      return true;
    } else {
      return false;
    }
  }
};

ConsoleOutputSerializer::ConsoleOutputSerializer(std::ostream &stream) : m_impl{new _Impl{stream}} { /* */
}

ConsoleOutputSerializer::~ConsoleOutputSerializer() { /* */
}

ISerializer::SerializerType ConsoleOutputSerializer::type() const {
  return ISerializer::OUTPUT;
}

bool ConsoleOutputSerializer::beginObject(Common::StringView name) {
  if (m_impl->stack.empty()) {
    m_impl->emitter << YAML::BeginMap;
    m_impl->stack.push(_Impl::OBJECT);
    return true;
  } else if (m_impl->stack.top() == _Impl::OBJECT) {
    m_impl->emitter << YAML::Key;
    m_impl->stream << rang::fg::green;
    m_impl->emitter << std::string{name};
    m_impl->stream << rang::fg::reset;
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

bool ConsoleOutputSerializer::endObject() {
  XI_RETURN_EC_IF(m_impl->stack.empty(), false);
  XI_RETURN_EC_IF_NOT(m_impl->stack.top() == _Impl::OBJECT, false);
  m_impl->stack.pop();
  m_impl->emitter << YAML::EndMap;
  return true;
}

bool ConsoleOutputSerializer::beginArray(size_t &, Common::StringView name) {
  if (m_impl->stack.empty()) {
    m_impl->emitter << YAML::BeginSeq;
    m_impl->stack.push(_Impl::ARRAY);
    return true;
  } else if (m_impl->stack.top() == _Impl::OBJECT) {
    m_impl->emitter << YAML::Key;
    m_impl->stream << rang::fg::green;
    m_impl->emitter << std::string{name};
    m_impl->stream << rang::fg::reset;
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

bool ConsoleOutputSerializer::beginStaticArray(const size_t size, Common::StringView name) {
  auto _ = size;
  return beginArray(_, name);
}

bool ConsoleOutputSerializer::endArray() {
  XI_RETURN_EC_IF(m_impl->stack.empty(), false);
  XI_RETURN_EC_IF_NOT(m_impl->stack.top() == _Impl::ARRAY, false);
  m_impl->stack.pop();
  m_impl->emitter << YAML::EndSeq;
  return true;
}

bool ConsoleOutputSerializer::operator()(uint8_t &value, Common::StringView name) {
  uint16_t _ = value;
  return m_impl->emit(_, name);
}

bool ConsoleOutputSerializer::operator()(int16_t &value, Common::StringView name) {
  return m_impl->emit(value, name, rang::fg::gray, value < 0 ? rang::style::italic : rang::style::reset);
}

bool ConsoleOutputSerializer::operator()(uint16_t &value, Common::StringView name) {
  return m_impl->emit(value, name, rang::fg::gray);
}

bool ConsoleOutputSerializer::operator()(int32_t &value, Common::StringView name) {
  return m_impl->emit(value, name, rang::fg::gray, value < 0 ? rang::style::italic : rang::style::reset);
}

bool ConsoleOutputSerializer::operator()(uint32_t &value, Common::StringView name) {
  return m_impl->emit(value, name, rang::fg::gray);
}

bool ConsoleOutputSerializer::operator()(int64_t &value, Common::StringView name) {
  return m_impl->emit(value, name, rang::fg::gray, value < 0 ? rang::style::italic : rang::style::reset);
}

bool ConsoleOutputSerializer::operator()(uint64_t &value, Common::StringView name) {
  return m_impl->emit(value, name, rang::fg::gray);
}

bool ConsoleOutputSerializer::operator()(double &value, Common::StringView name) {
  return m_impl->emit(value, name, rang::fg::gray, value < 0 ? rang::style::italic : rang::style::reset);
}

bool ConsoleOutputSerializer::operator()(bool &value, Common::StringView name) {
  return m_impl->emit(value, name, value ? rang::fg::green : rang::fg::red);
}

bool ConsoleOutputSerializer::operator()(std::string &value, Common::StringView name) {
  return m_impl->emit(value, name, rang::fg::gray);
}

bool ConsoleOutputSerializer::binary(void *value, size_t size, Common::StringView name) {
  auto hex = Common::toHex(value, size);
  return m_impl->emit(hex, name, rang::fg::yellow, rang::style::italic);
}

bool ConsoleOutputSerializer::binary(std::string &value, Common::StringView name) {
  auto hex = Common::toHex(value.data(), value.size());
  return m_impl->emit(hex, name, rang::fg::yellow, rang::style::italic);
}

bool ConsoleOutputSerializer::binary(Xi::ByteVector &value, Common::StringView name) {
  auto hex = Common::toHex(value.data(), value.size());
  return m_impl->emit(hex, name, rang::fg::yellow, rang::style::italic);
}

bool ConsoleOutputSerializer::maybe(bool &value, Common::StringView name) {
  XI_RETURN_EC_IF(m_impl->stack.empty(), false);
  if (value) {
    return true;
  } else {
    return m_impl->emit(YAML::Null, name, rang::fg::red);
  }
}

bool ConsoleOutputSerializer::typeTag(TypeTag &tag, Common::StringView name) {
  XI_RETURN_EC_IF(tag.text() == TypeTag::NoTextTag, false);
  auto text = tag.text();
  return m_impl->emit(text, name, rang::fg::cyan, rang::style::bold);
}

bool ConsoleOutputSerializer::flag(std::vector<TypeTag> &flag, Common::StringView name) {
  auto size = flag.size();
  XI_RETURN_EC_IF_NOT(beginArray(size, name), false);
  for (auto &iFlag : flag) {
    auto text = iFlag.text();
    XI_RETURN_EC_IF(text == TypeTag::NoTextTag, false);
    XI_RETURN_EC_IF_NOT(m_impl->emit(text, "", rang::fg::cyan, rang::style::bold), false);
  }
  XI_RETURN_EC_IF_NOT(endArray(), false);
  return true;
}

}  // namespace CryptoNote
