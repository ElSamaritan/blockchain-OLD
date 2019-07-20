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

#include "Xi/Http/Request.h"
#include "Xi/Http/Response.h"

namespace Xi {
namespace Http {
/*!
 * \brief The RequestHandler class handles incoming request of the server.
 *
 * The RequestHandler class is the main entrypoint for incoming request and abstract their handling. Further the
 * RequestHandler provides some builder functions for building Response quicker.
 */
class RequestHandler {
 public:
  RequestHandler() = default;
  virtual ~RequestHandler() = default;

  /*!
   * \brief operator () is called for a request to handle
   * \param request the incoming request
   * \return the response that should be returned
   *
   * \attention do not override this method, it is exception safe, therefore you can always throw within doHandleRequest
   * and the handler will turn it into a internal server error.
   */
  Response operator()(const Request& request);

  /*!
   * \brief makeBadRequest returns a bad request response
   * \param why the message to display to the user
   *
   * \attention \param why should never contain any sensible data about what happened on the server.
   */
  Response makeBadRequest(const std::string& why = "");

  /*!
   * \brief makeNotFound returns a not found response
   * \param why the message to display to the user
   *
   * \attention \param why should never contain any sensible data about what happened on the server.
   */
  Response makeNotFound(const std::string& why = "");

  /*!
   * \brief makeNotFound returns a not authorized repsone
   * \param why the message to display to the user
   *
   * \attention \param why should never contain any sensible data about what happened on the server.
   */
  Response makeUnauthorized(const std::string& why = "");

  /*!
   * \brief makeNotImplemented returns a not implemented response
   * \param why the message to display to the user
   *
   * \attention \param why should never contain any sensible data about what happened on the server.
   */
  Response makeNotImplemented();

  /*!
   * \brief makeInternalServerError returns a internal server error response
   * \param why the message to display to the user
   *
   * \attention \param why should never contain any sensible data about what happened on the server.
   */
  Response makeInternalServerError(const std::string& why = "");

 protected:
  /*!
   * \brief doHandleRequest override this method to implement you custom request handling logic
   */
  virtual Response doHandleRequest(const Request& request) = 0;

  /*!
   * These methods implement the corresponding non virtual functions. You are free to throw exceptions from here
   * and they will be transformed into internal server errors.
   */

  virtual Response doMakeBadRequest(const std::string& why);
  virtual Response doMakeNotFound(const std::string& why);
  virtual Response doMakeUnauthorized(const std::string& why);
  virtual Response doMakeNotImplemented();
  virtual Response doMakeInternalServerError(const std::string& why);

 private:
  void emplaceContentEncoding(const Request& request, Response& response);
  Response fail(std::exception_ptr ex);
};
}  // namespace Http
}  // namespace Xi
