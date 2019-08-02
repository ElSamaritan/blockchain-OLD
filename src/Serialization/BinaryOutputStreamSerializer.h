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

#include "Common/IOutputStream.h"
#include "ISerializer.h"
#include "SerializationOverloads.h"

namespace CryptoNote {

class BinaryOutputStreamSerializer final : public ISerializer {
 public:
  BinaryOutputStreamSerializer(Common::IOutputStream& strm) : stream(strm) {
  }
  virtual ~BinaryOutputStreamSerializer() override {
  }

  virtual ISerializer::SerializerType type() const override;
  FormatType format() const override;

  bool useVarInt();
  void setUseVarInt(bool use);

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
  void checkedWrite(const char* buf, size_t size);
  Common::IOutputStream& stream;
  bool m_useVarInt{true};
};

}  // namespace CryptoNote
