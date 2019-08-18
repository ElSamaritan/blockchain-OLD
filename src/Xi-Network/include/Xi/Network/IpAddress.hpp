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

#include <variant>
#include <vector>
#include <string>
#include <cinttypes>
#include <optional>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/Byte.hh>
#include <Serialization/ISerializer.h>

#include "Xi/Network/Port.hpp"

namespace Xi {
namespace Network {

class IpAddress;
using IpAddressVector = std::vector<IpAddress>;

class IpAddress {
 public:
  enum Type {
    v4 = 1 << 0,
    v6 = 1 << 1,
  };

 public:
  static Result<IpAddress> fromString(const std::string& str);
  static Result<IpAddressVector> resolve(const std::string& host);
  static Result<IpAddressVector> resolve(const std::string& host, const Port port);
  static Result<IpAddress> resolveAny(const std::string& host);
  static Result<IpAddress> resolveAny(const std::string& host, const Type type);

 public:
  XI_DEFAULT_COPY(IpAddress);
  XI_DEFAULT_MOVE(IpAddress);

  Type type() const;

  const Byte* data() const;
  size_t size() const;

  std::string toString() const;

 private:
  explicit IpAddress();

 private:
  using v4_storage = ByteArray<4>;
  using v6_storage = ByteArray<16>;

  std::variant<v4_storage, v6_storage> m_data;
};

std::string toString(const IpAddress& addr);

}  // namespace Network
}  // namespace Xi
