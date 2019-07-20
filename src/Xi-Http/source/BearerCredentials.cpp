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

#include "Xi/Http/BearerCredentials.h"

namespace Xi {
namespace Http {

BearerCredentials::BearerCredentials(const std::string &token) : m_token{token} {
}

void BearerCredentials::setToken(const std::string &token) {
  m_token = token;
}

const std::string &BearerCredentials::token() const {
  return m_token;
}

}  // namespace Http

std::string to_string(const Http::BearerCredentials &bearer) {
  return bearer.token();
}

template <>
Http::BearerCredentials lexical_cast<Xi::Http::BearerCredentials>(const std::string &value) {
  return Http::BearerCredentials{value};
}

}  // namespace Xi
