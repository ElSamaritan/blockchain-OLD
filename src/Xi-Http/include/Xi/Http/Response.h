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
#include <sstream>

#include <Xi/Global.h>

#include "Xi/Http/StatusCode.h"
#include "Xi/Http/Version.h"
#include "Xi/Http/HeaderContainer.h"

namespace Xi {
namespace Http {
/*!
 * \brief The Response class is a wrapper for a http response that was recieved or will be send back to a client
 */
class Response final {
 public:
  XI_DEFAULT_MOVE(Response);
  XI_DEFAULT_COPY(Response);

  /*!
   * \brief Response creates a new response
   * \param code the status code of the response
   * \param body the response string body, with according content type you do not need to compress it
   * \param type the content type of the body
   */
  explicit Response(StatusCode code = StatusCode::Ok, const std::string& body = "",
                    ContentType type = ContentType::Text);
  ~Response() = default;

  /*!
   * \brief status the status code of this response
   */
  StatusCode status() const;

  /*!
   * \brief setStatus sets the status code
   */
  void setStatus(StatusCode code);

  /*!
   * \brief body the response text body
   *
   * To query its content type use \see headers
   */
  const std::string& body() const;

  /*!
   * \brief setBody sets the body to a copy of the provided text
   */
  void setBody(const std::string& body);

  /*!
   * \brief setBody moves the provided text into the body wihtout copying it
   */
  void setBody(std::string&& body);

  /*!
   * \brief isRedirect returns true iff the response encodes a redirection (3XX status).
   */
  bool isRedirection() const;

  /*!
   * \brief headers mutable headers wrapper
   */
  HeaderContainer& headers();

  /*!
   * \brief headers immutable deaders wrapper
   */
  const HeaderContainer& headers() const;

 private:
  HeaderContainer m_headers;
  std::string m_body;
  StatusCode m_statusCode;
};
}  // namespace Http
}  // namespace Xi
