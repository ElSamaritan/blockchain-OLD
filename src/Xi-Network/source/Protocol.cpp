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

#include "Xi/Network/Protocol.hpp"

#include <Xi/Exceptions.hpp>

namespace Xi {
namespace Network {

Result<Protocol> parseProtocol(const std::string &str) {
  XI_ERROR_TRY
  if (str == "http") {
    return success(Protocol::Http);
  } else if (str == "https") {
    return success(Protocol::Https);
  } else if (str == "xi") {
    return success(Protocol::Xi);
  } else if (str == "xis") {
    return success(Protocol::Xis);
  } else if (str == "xip") {
    return success(Protocol::Xip);
  } else {
    exceptional<NotFoundError>("Unknown network protocol: {}", str);
  }
  XI_ERROR_CATCH
}

Result<std::string> toString(const Protocol protocol) {
  XI_ERROR_TRY
  switch (protocol) {
    case Protocol::Http:
      return emplaceSuccess<std::string>("http");
    case Protocol::Https:
      return emplaceSuccess<std::string>("https");
    case Protocol::Xi:
      return emplaceSuccess<std::string>("xi");
    case Protocol::Xis:
      return emplaceSuccess<std::string>("xis");
    case Protocol::Xip:
      return emplaceSuccess<std::string>("xip");
  }
  exceptional<InvalidEnumValueError>("Unrecognized network protocol");
  XI_ERROR_CATCH
}

bool isHttpBased(const Protocol protocol) {
  switch (protocol) {
    case Protocol::Http:
    case Protocol::Https:
    case Protocol::Xi:
    case Protocol::Xis:
      return true;

    default:
      return false;
  }
}

bool isHttp(const Protocol protocol) {
  switch (protocol) {
    case Protocol::Http:
    case Protocol::Xi:
      return true;

    default:
      return false;
  }
}

bool isHttps(const Protocol protocol) {
  switch (protocol) {
    case Protocol::Https:
    case Protocol::Xis:
      return true;

    default:
      return false;
  }
}

}  // namespace Network
}  // namespace Xi
