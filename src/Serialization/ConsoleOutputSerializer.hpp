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

#include <ostream>
#include <memory>

#include "Serialization/ISerializer.h"

namespace CryptoNote {

class ConsoleOutputSerializer final : public ISerializer {
 public:
  explicit ConsoleOutputSerializer(std::ostream& stream);
  ~ConsoleOutputSerializer() override;

  // ISerializer =====================================================================================================
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
  // ISerializer =====================================================================================================

 private:
  struct _Impl;
  std::unique_ptr<_Impl> m_impl;
};

}  // namespace CryptoNote
