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

#include <Xi/ExternalIncludePush.h>
#include <boost/optional/optional_io.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <Xi/ExternalIncludePop.h>

#include "Xi/Http/Request.h"
#include "Xi/Http/Response.h"

namespace Xi {
namespace Http {
/*!
 * \brief The BeastConversion struct converts high level data structures to low level and vice versa
 */
struct BeastConversion {
  using beast_request_t = boost::beast::http::request<boost::beast::http::string_body>;
  using beast_response_t = boost::beast::http::response<boost::beast::http::string_body>;

  using api_request_t = Request;
  using api_response_t = Response;

  /*!
   * \brief operator () converts the low level request into a high level api request
   *
   * This method is used from the server to yield high level api requests
   */
  api_request_t operator()(const beast_request_t& request) const;

  /*!
   * \brief operator () converts the low level response into a high level api response
   *
   * This method is used by the client to yield high level api responses
   */
  api_response_t operator()(const beast_response_t& response) const;

  /*!
   * \brief operator () converts a high level api request into a low level request
   *
   * This method is used by the client to send a given request using the low level library
   */
  beast_request_t operator()(const api_request_t& request) const;

  /*!
   * \brief operator () converts a high level api response into a low level response
   *
   * This method is used by the server to send back a high level api response using the low level library
   */
  beast_response_t operator()(const api_response_t& response) const;

  void copyHeaders(const HeaderContainer& headers, beast_request_t& request) const;
  void copyHeaders(const HeaderContainer& headers, beast_response_t& response) const;
  void copyHeaders(const beast_request_t& request, HeaderContainer& headers) const;
  void copyHeaders(const beast_response_t& response, HeaderContainer& headers) const;

  std::string decodeBody(boost::iostreams::array_source source, boost::optional<ContentEncoding> encoding) const;
  std::string encodeBody(boost::iostreams::array_source source, boost::optional<ContentEncoding> encoding) const;
};
}  // namespace Http
}  // namespace Xi
