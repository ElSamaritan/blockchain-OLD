﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
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

#include "Common/JsonValue.h"
#include "ISerializer.h"

namespace CryptoNote {

// deserialization
class JsonInputValueSerializer : public ISerializer {
 public:
  JsonInputValueSerializer(const Common::JsonValue& value);
  JsonInputValueSerializer(Common::JsonValue&& value);
  virtual ~JsonInputValueSerializer() override;

  SerializerType type() const override;
  FormatType format() const override;

  [[nodiscard]] virtual bool beginObject(Common::StringView name) override;
  [[nodiscard]] virtual bool endObject() override;

  [[nodiscard]] virtual bool beginArray(size_t& size, Common::StringView name) override;
  [[nodiscard]] virtual bool beginStaticArray(const size_t size, Common::StringView name) override;
  [[nodiscard]] virtual bool endArray() override;

  [[nodiscard]] virtual bool operator()(uint8_t& value, Common::StringView name) override;
  [[nodiscard]] virtual bool operator()(int16_t& value, Common::StringView name) override;
  [[nodiscard]] virtual bool operator()(uint16_t& value, Common::StringView name) override;
  [[nodiscard]] virtual bool operator()(int32_t& value, Common::StringView name) override;
  [[nodiscard]] virtual bool operator()(uint32_t& value, Common::StringView name) override;
  [[nodiscard]] virtual bool operator()(int64_t& value, Common::StringView name) override;
  [[nodiscard]] virtual bool operator()(uint64_t& value, Common::StringView name) override;
  [[nodiscard]] virtual bool operator()(double& value, Common::StringView name) override;
  [[nodiscard]] virtual bool operator()(bool& value, Common::StringView name) override;
  [[nodiscard]] virtual bool operator()(std::string& value, Common::StringView name) override;
  [[nodiscard]] virtual bool binary(void* value, size_t size, Common::StringView name) override;
  [[nodiscard]] virtual bool binary(std::string& value, Common::StringView name) override;
  [[nodiscard]] virtual bool binary(Xi::ByteVector& value, Common::StringView name) override;
  [[nodiscard]] virtual bool maybe(bool& value, Common::StringView name) override;
  [[nodiscard]] virtual bool typeTag(TypeTag& tag, Common::StringView name) override;
  [[nodiscard]] virtual bool flag(std::vector<TypeTag>& flag, Common::StringView name) override;

  template <typename T>
  [[nodiscard]] bool operator()(T& value, Common::StringView name) {
    return ISerializer::operator()(value, name);
  }

 private:
  Common::JsonValue value;
  std::vector<const Common::JsonValue*> chain;
  std::vector<size_t> idxs;

  const Common::JsonValue* getValue(Common::StringView name);

  template <typename T>
  bool getNumber(Common::StringView name, T& v) {
    auto ptr = getValue(name);

    if (!ptr) {
      return false;
    }

    v = static_cast<T>(ptr->getInteger());
    return true;
  }
};

}  // namespace CryptoNote
