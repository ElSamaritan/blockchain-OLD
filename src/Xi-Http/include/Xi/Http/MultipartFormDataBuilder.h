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

#include <sstream>
#include <string>

#include <Xi/Global.hh>

#include "Xi/Http/Request.h"

namespace Xi {
namespace Http {
/*!
 * \brief The MultipartFormDataBuilder class encapsulates building multipart form data request with convinient methods.
 */
class MultipartFormDataBuilder {
 public:
  MultipartFormDataBuilder();
  XI_DELETE_COPY(MultipartFormDataBuilder);
  XI_DEFAULT_MOVE(MultipartFormDataBuilder);
  ~MultipartFormDataBuilder();

  /*!
   * \brief addFile adds a file to the request
   * \param name The form name of the file
   * \param filepath The path to the file that should be included in the request.
   * \param type The content type of the file.
   */
  void addFile(const std::string& name, const std::string& filepath, ContentType type = ContentType::Binary);

  /*!
   * \brief addField adds a key value pair to the request
   * \param name The form property identifier
   * \param value The value of the property
   */
  void addField(const std::string& name, const std::string& value);

  /*!
   * \brief request creates a request setup as multipart/form-data
   * \param target The server target path to send to
   * \param method The method used to send the requesst
   */
  Request request(const std::string& target, Method method = Method::Post) const;

 private:
  std::string m_boundary;
  std::stringstream m_body;
};
}  // namespace Http
}  // namespace Xi
