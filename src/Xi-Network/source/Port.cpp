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

#include "Xi/Network/Port.hpp"

#include <Xi/Exceptions.hpp>
#include <Xi/Algorithm/String.h>

namespace Xi {
namespace Network {

const Port Port::Any{0};

Result<Port> Port::fromString(const std::string &str) {
  XI_ERROR_TRY
  return emplaceSuccess<Port>(lexical_cast<uint16_t>(str));
  XI_ERROR_CATCH
}

Result<Port> Port::fromProtocol(const Protocol protocol) {
  XI_ERROR_TRY
  switch (protocol) {
    case Protocol::Http:
      return success(Port{80});
    case Protocol::Https:
      return success(Port{443});
    case Protocol::Xi:
      return success(Port{22869});
    case Protocol::Xis:
      return success(Port{22869});
    case Protocol::Xip:
      return success(Port{22868});
  }
  exceptional<InvalidEnumValueError>("Unknown network protocol");
  XI_ERROR_CATCH
}

bool Port::isAny() const {
  return (*this) == Any;
}

std::string Port::toString() const {
  return std::to_string(native());
}

Port Port::orDefault(Port def) const {
  if (isAny()) {
    return def;
  } else {
    return *this;
  }
}

bool serialize(Port &port, Common::StringView name, CryptoNote::ISerializer &serializer) {
  return serializer(port.value, name);
}

}  // namespace Network
}  // namespace Xi
