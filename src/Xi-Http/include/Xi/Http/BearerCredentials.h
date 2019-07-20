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

#include <string>
#include <stdexcept>

#include <Xi/Global.hh>
#include <Xi/Algorithm/String.h>

namespace Xi {
namespace Http {
/*!
 * \brief The BearerCredentials class wraps a bearer token authentication schema.
 */
class BearerCredentials final {
 public:
  XI_DEFAULT_COPY(BearerCredentials);
  XI_DEFAULT_MOVE(BearerCredentials);

  /*!
   * \brief BearerCredentials construct authentication credentials required for bearer http authorization
   * \param token The access token to use.
   */
  explicit BearerCredentials(const std::string& token = "");
  ~BearerCredentials() = default;

  void setToken(const std::string& token);
  const std::string& token() const;

 private:
  std::string m_token;
};
}  // namespace Http
std::string to_string(const Http::BearerCredentials& bearer);
template <>
Xi::Http::BearerCredentials lexical_cast<Xi::Http::BearerCredentials>(const std::string& value);
}  // namespace Xi
