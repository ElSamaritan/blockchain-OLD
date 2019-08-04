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

#include "Xi/Http/RequestHandler.h"

#include <Xi/Global.hh>

Xi::Http::ServerLimitsConfiguration Xi::Http::RequestHandler::limits() const {
  return ServerLimitsConfiguration{/* */};
}

Xi::Http::Response Xi::Http::RequestHandler::operator()(const Xi::Http::Request &request) {
  try {
    auto response = doHandleRequest(request);
    if (!response.body().empty()) {
      emplaceContentEncoding(request, response);
    }
    response.headers().setAcceptedContentEncodings(
        {ContentEncoding::Gzip, ContentEncoding::Deflate, ContentEncoding::Identity});
    return response;
  } catch (...) {
    try {
      return fail(std::current_exception());
    } catch (...) {
      // TODO logging
      Response response{};
      response.setStatus(StatusCode::InternalServerError);
      return response;
    }
  }
}

Xi::Http::Response Xi::Http::RequestHandler::makeBadRequest(const std::string &why) {
  return doMakeBadRequest(why);
}

Xi::Http::Response Xi::Http::RequestHandler::makeNotFound(const std::string &why) {
  return doMakeNotFound(why);
}

Xi::Http::Response Xi::Http::RequestHandler::makeUnauthorized(const std::string &why) {
  return doMakeUnauthorized(why);
}

Xi::Http::Response Xi::Http::RequestHandler::makeNotImplemented() {
  return doMakeNotImplemented();
}

Xi::Http::Response Xi::Http::RequestHandler::makeInternalServerError(const std::string &why) {
  return doMakeInternalServerError(why);
}

Xi::Http::Response Xi::Http::RequestHandler::doMakeBadRequest(const std::string &why) {
  return Response{StatusCode::BadRequest, why};
}

Xi::Http::Response Xi::Http::RequestHandler::doMakeNotFound(const std::string &why) {
  return Response{StatusCode::NotFound, why};
}

Xi::Http::Response Xi::Http::RequestHandler::doMakeUnauthorized(const std::string &why) {
  return Response{StatusCode::Unauthorized, why};
}

Xi::Http::Response Xi::Http::RequestHandler::doMakeNotImplemented() {
  return Response{StatusCode::NotImplemented};
}

Xi::Http::Response Xi::Http::RequestHandler::doMakeInternalServerError(const std::string &why) {
  return Response{StatusCode::InternalServerError, why};
}

void Xi::Http::RequestHandler::emplaceContentEncoding(const Xi::Http::Request &request, Xi::Http::Response &response) {
  try {
    if (request.headers().acceptsContentEncoding(ContentEncoding::Gzip))
      response.headers().setContentEncoding(ContentEncoding::Gzip);
    else if (request.headers().acceptsContentEncoding(ContentEncoding::Deflate))
      response.headers().setContentEncoding(ContentEncoding::Deflate);
    else
      response.headers().setContentEncoding(ContentEncoding::Identity);
  } catch (...) {
    response.headers().setContentEncoding(ContentEncoding::Identity);
  }
}

Xi::Http::Response Xi::Http::RequestHandler::fail(std::exception_ptr ex) {
  XI_UNUSED(ex);
  return makeInternalServerError("");
}
