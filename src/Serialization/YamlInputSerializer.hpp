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

#pragma once

#include <memory>

#include "Serialization/ISerializer.h"

namespace CryptoNote {

class YamlInputSerializer final : public CryptoNote::ISerializer {
 public:
  static std::unique_ptr<YamlInputSerializer> parse(const std::string& content);

 public:
  ~YamlInputSerializer() override;

  SerializerType type() const override;
  FormatType format() const override;

  [[nodiscard]] bool beginObject(Common::StringView name) override;
  [[nodiscard]] bool endObject() override;
  [[nodiscard]] bool beginArray(size_t& size, Common::StringView name) override;
  [[nodiscard]] bool beginStaticArray(const size_t size, Common::StringView name) override;
  [[nodiscard]] bool endArray() override;

  [[nodiscard]] bool operator()(uint8_t& value, Common::StringView name) override;
  [[nodiscard]] bool operator()(int16_t& value, Common::StringView name) override;
  [[nodiscard]] bool operator()(uint16_t& value, Common::StringView name) override;
  [[nodiscard]] bool operator()(int32_t& value, Common::StringView name) override;
  [[nodiscard]] bool operator()(uint32_t& value, Common::StringView name) override;
  [[nodiscard]] bool operator()(int64_t& value, Common::StringView name) override;
  [[nodiscard]] bool operator()(uint64_t& value, Common::StringView name) override;
  [[nodiscard]] bool operator()(double& value, Common::StringView name) override;
  [[nodiscard]] bool operator()(bool& value, Common::StringView name) override;
  [[nodiscard]] bool operator()(std::string& value, Common::StringView name) override;
  [[nodiscard]] bool binary(void* value, size_t size, Common::StringView name) override;
  [[nodiscard]] bool binary(std::string& value, Common::StringView name) override;
  [[nodiscard]] bool binary(Xi::ByteVector& value, Common::StringView name) override;
  [[nodiscard]] bool maybe(bool& value, Common::StringView name) override;
  [[nodiscard]] bool typeTag(TypeTag& tag, Common::StringView name) override;
  [[nodiscard]] bool flag(std::vector<TypeTag>& flag, Common::StringView name) override;

  template <typename T>
  [[nodiscard]] bool operator()(T& value, Common::StringView name) {
    return ISerializer::operator()(value, name);
  }

 private:
  explicit YamlInputSerializer();

 private:
  struct _Impl;
  std::unique_ptr<_Impl> m_impl;
};

template <typename _ValueT>
[[nodiscard]] bool fromYaml(const std::string& content, _ValueT& out) {
  try {
    auto serializer = YamlInputSerializer::parse(content);
    XI_RETURN_EC_IF_NOT(serializer, false);
    XI_RETURN_EC_IF_NOT((*serializer)(out, ""), false);
    XI_RETURN_SC(true);
  } catch (const std::exception& e) {
    XI_PRINT_EC("YAML parsing threw: %s", e.what());
    XI_RETURN_EC(false);
  } catch (...) {
    XI_PRINT_EC("YAML parsing threw: UNKNOWN");
    XI_RETURN_EC(false);
  }
}

}  // namespace CryptoNote
