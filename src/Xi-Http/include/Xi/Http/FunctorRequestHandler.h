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

#include <functional>

#include <Xi/Global.h>

#include "Xi/Http/RequestHandler.h"

namespace Xi {
namespace Http {
/*!
 * \brief The FunctorRequestHandler class handles request by forwarding it to a functor object
 */
class FunctorRequestHandler : public RequestHandler {
 public:
  using function_t = std::function<Response(const Request&)>;

 public:
  /*!
   * \brief FunctorRequestHandler constructs a request handler
   * \param functor the function object that should be called to server the request.
   */
  FunctorRequestHandler(function_t functor);
  XI_DEFAULT_COPY(FunctorRequestHandler);
  XI_DEFAULT_MOVE(FunctorRequestHandler);
  virtual ~FunctorRequestHandler() override = default;

 protected:
  /*!
   * \brief doHandleRequest forwards the request to the functor provided
   */
  Response doHandleRequest(const Request& request) override;

 private:
  function_t m_functor;
};
}  // namespace Http
}  // namespace Xi
