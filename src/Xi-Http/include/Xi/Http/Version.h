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

namespace Xi {
namespace Http {
/*!
 * \brief The Version enum is the http protocol version to be used
 *
 * Http2.* is currently not supported.
 */
enum struct Version {
  Http_0_9 =
      9,  ///< HTTP/0.9 – The one-line protocol \link
          ///< https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#HTTP0.9_%E2%80%93_The_one-line_protocol
  Http1_0 =
      10,  ///< HTTP/1.0 – Building extensibility \link
           ///< https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#HTTP1.0_%E2%80%93_Building_extensibility
  Http1_1 =
      11  ///< HTTP/1.1 – The standardized protocol \link
          ///< https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Evolution_of_HTTP#HTTP1.1_%E2%80%93_The_standardized_protocol
};
}  // namespace Http
}  // namespace Xi
