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

#include "Xi/Http/BasicCredentials.h"

#include <string>
#include <algorithm>
#include <stdexcept>

bool Xi::Http::BasicCredentials::validateUsername(const std::string &username) {
  return !username.empty() && std::find(username.begin(), username.end(), ':') == username.end();
}

Xi::Http::BasicCredentials::BasicCredentials(const std::string &_username, const std::string &_password)
    : m_username{_username}, m_password{_password} {
  if (!validateUsername(_username))
    throw std::invalid_argument{"provided username is invalid."};
}

void Xi::Http::BasicCredentials::setUsername(const std::string &_username) {
  if (!validateUsername(_username))
    throw std::invalid_argument{"provided username is invalid."};
  m_username = _username;
}

const std::string &Xi::Http::BasicCredentials::username() const {
  return m_username;
}

void Xi::Http::BasicCredentials::setPassword(const std::string &_password) {
  m_password = _password;
}

const std::string &Xi::Http::BasicCredentials::password() const {
  return m_password;
}

namespace Xi {
std::string to_string(const Xi::Http::BasicCredentials &cred) {
  return cred.username() + ":" + cred.password();
}

template <>
Xi::Http::BasicCredentials lexical_cast<Xi::Http::BasicCredentials>(const std::string &value) {
  const auto splitPos = std::find(value.begin(), value.end(), ':');
  if (splitPos == value.end())
    throw std::invalid_argument{"provided credentials are not correctly encoded"};
  const std::string username{value.begin(), splitPos};
  const std::string password{splitPos + 1, value.end()};
  return Http::BasicCredentials{username, password};
}
}  // namespace Xi
