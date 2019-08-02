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

#include "Serialization/ISerializer.h"

namespace CryptoNote {

class NullOutputSerializer final : public ISerializer {
 public:
  NullOutputSerializer() {
  }
  virtual ~NullOutputSerializer() override {
  }

  inline ISerializer::SerializerType type() const override {
    return ISerializer::OUTPUT;
  }
  inline ISerializer::FormatType format() const override {
    return ISerializer::Machinery;
  }

  [[nodiscard]] inline bool beginObject(Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool endObject() override {
    return true;
  }

  [[nodiscard]] inline bool beginArray(size_t&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool beginStaticArray(const size_t, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool endArray() override {
    return true;
  }

  [[nodiscard]] inline bool operator()(uint8_t&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool operator()(int16_t&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool operator()(uint16_t&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool operator()(int32_t&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool operator()(uint32_t&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool operator()(int64_t&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool operator()(uint64_t&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool operator()(double&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool operator()(bool&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool operator()(std::string&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool binary(void*, size_t, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool binary(std::string&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool binary(Xi::ByteVector&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool maybe(bool&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool typeTag(TypeTag&, Common::StringView) override {
    return true;
  }
  [[nodiscard]] inline bool flag(std::vector<TypeTag>&, Common::StringView) override {
    return true;
  }

  template <typename T>
  [[nodiscard]] bool operator()(T& value, Common::StringView name) {
    return ISerializer::operator()(value, name);
  }
};

}  // namespace CryptoNote
