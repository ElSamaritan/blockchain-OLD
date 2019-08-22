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

#include "Xi/Http/StatusCode.h"

#include <boost/beast.hpp>

static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::Ok) == boost::beast::http::status::ok,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::Created) ==
                  boost::beast::http::status::created,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::Accepted) ==
                  boost::beast::http::status::accepted,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::NoContent) ==
                  boost::beast::http::status::no_content,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::MovedPermanently) ==
                  boost::beast::http::status::moved_permanently,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::Found) == boost::beast::http::status::found,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::SeeOther) ==
                  boost::beast::http::status::see_other,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::TemporaryRedirect) ==
                  boost::beast::http::status::temporary_redirect,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::PermanentRedirect) ==
                  boost::beast::http::status::permanent_redirect,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::BadRequest) ==
                  boost::beast::http::status::bad_request,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::Unauthorized) ==
                  boost::beast::http::status::unauthorized,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::Forbidden) ==
                  boost::beast::http::status::forbidden,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::NotFound) ==
                  boost::beast::http::status::not_found,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::MethodNotAllowed) ==
                  boost::beast::http::status::method_not_allowed,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::RequestTimeout) ==
                  boost::beast::http::status::request_timeout,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::Gone) == boost::beast::http::status::gone,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::LengthRequired) ==
                  boost::beast::http::status::length_required,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::UnsupportedMediaType) ==
                  boost::beast::http::status::unsupported_media_type,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::TooManyRequests) ==
                  boost::beast::http::status::too_many_requests,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::InternalServerError) ==
                  boost::beast::http::status::internal_server_error,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::NotImplemented) ==
                  boost::beast::http::status::not_implemented,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::BadGateway) ==
                  boost::beast::http::status::bad_gateway,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::ServiceUnavailable) ==
                  boost::beast::http::status::service_unavailable,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::GatewayTimeout) ==
                  boost::beast::http::status::gateway_timeout,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::HttpVersionNotSupported) ==
                  boost::beast::http::status::http_version_not_supported,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::NetworkAuthenticationRequired) ==
                  boost::beast::http::status::network_authentication_required,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");
static_assert(static_cast<boost::beast::http::status>(Xi::Http::StatusCode::NetworkConnectTimeout) ==
                  boost::beast::http::status::network_connect_timeout_error,
              "The status code must correspond to the equivalent beast library status code because they internally "
              "converted seamlessly.");

std::string Xi::to_string(Xi::Http::StatusCode status) {
  switch (status) {
    case Http::StatusCode::Ok:
      return "Ok";
    case Http::StatusCode::Created:
      return "Created";
    case Http::StatusCode::Accepted:
      return "Accepted";
    case Http::StatusCode::NoContent:
      return "NoContent";
    case Http::StatusCode::MovedPermanently:
      return "MovedPermanently";
    case Http::StatusCode::Found:
      return "Found";
    case Http::StatusCode::SeeOther:
      return "SeeOther";
    case Http::StatusCode::TemporaryRedirect:
      return "TemporaryRedirect";
    case Http::StatusCode::PermanentRedirect:
      return "PermanentRedirect";
    case Http::StatusCode::BadRequest:
      return "BadRequest";
    case Http::StatusCode::Unauthorized:
      return "Unauthorized";
    case Http::StatusCode::Forbidden:
      return "Forbidden";
    case Http::StatusCode::NotFound:
      return "NotFound";
    case Http::StatusCode::MethodNotAllowed:
      return "MethodNotAllowed";
    case Http::StatusCode::RequestTimeout:
      return "RequestTimeout";
    case Http::StatusCode::Gone:
      return "Gone";
    case Http::StatusCode::LengthRequired:
      return "LengthRequired";
    case Http::StatusCode::UnsupportedMediaType:
      return "UnsupportedMediaType";
    case Http::StatusCode::TooManyRequests:
      return "TooManyRequests";
    case Http::StatusCode::InternalServerError:
      return "InternalServerError";
    case Http::StatusCode::NotImplemented:
      return "NotImplemented";
    case Http::StatusCode::BadGateway:
      return "BadGateway";
    case Http::StatusCode::ServiceUnavailable:
      return "ServiceUnavailable";
    case Http::StatusCode::GatewayTimeout:
      return "GatewayTimeout";
    case Http::StatusCode::HttpVersionNotSupported:
      return "HttpVersionNotSupported";
    case Http::StatusCode::NetworkAuthenticationRequired:
      return "NetworkAuthenticationRequired";
    case Http::StatusCode::NetworkConnectTimeout:
      return "NetworkConnectTimeout";
    default:
      return "Unknown";
  }
}

bool Xi::Http::isSuccessCode(const Xi::Http::StatusCode code) {
  return static_cast<int>(code) >= 200 && static_cast<int>(code) < 300;
}
