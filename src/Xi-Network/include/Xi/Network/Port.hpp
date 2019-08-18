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

#include <cinttypes>
#include <string>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/TypeSafe/Integral.hpp>
#include <Serialization/ISerializer.h>

namespace Xi {
namespace Network {

struct Port : TypeSafe::EnableIntegralFromThis<uint16_t, Port> {
  static const Port Any;
  static Result<Port> fromString(const std::string& str);

  using EnableIntegralFromThis::EnableIntegralFromThis;

  bool isAny() const;
  std::string toString() const;
  Port orDefault(Port def) const;

  friend bool serialize(Port&, Common::StringView, CryptoNote::ISerializer&);
};

[[nodiscard]] bool serialize(Port& port, Common::StringView name, CryptoNote::ISerializer& serializer);

}  // namespace Network
}  // namespace Xi
