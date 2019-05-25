/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include <string>
#include <stdexcept>

#include <Xi/Global.hh>
#include <Xi/Algorithm/String.h>

namespace Xi {
namespace Http {
/*!
 * \brief The BasicCredentials class wraps a basic username and password authentication
 */
class BasicCredentials final {
 public:
  static bool validateUsername(const std::string& username);

 public:
  XI_DEFAULT_COPY(BasicCredentials);
  XI_DEFAULT_MOVE(BasicCredentials);

  /*!
   * \brief BasicCredentials construct authentication credentials required for basic http authorization
   * \param username The username to be used cannot be empty or contain ':'
   * \param password The password to be used, can be empty
   * \exception std::invalid_argument username is empty or contains ':'
   */
  explicit BasicCredentials(const std::string& username, const std::string& password = "");
  ~BasicCredentials() = default;

  /*!
   * \brief setUsername sets the username required.
   * \exception std::invalid_argument username is empty or contains ':'
   */
  void setUsername(const std::string& username);

  const std::string& username() const;

  void setPassword(const std::string& password);

  const std::string& password() const;

 private:
  std::string m_username;
  std::string m_password;
};
}  // namespace Http
std::string to_string(const Http::BasicCredentials& status);
template <>
Xi::Http::BasicCredentials lexical_cast<Xi::Http::BasicCredentials>(const std::string& value);
}  // namespace Xi
