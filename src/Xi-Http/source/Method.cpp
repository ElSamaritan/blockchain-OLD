﻿/* ============================================================================================== *
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

#include "Xi/Http/Method.h"

#include <stdexcept>

#include <boost/beast.hpp>

static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Unknown) == boost::beast::http::verb::unknown,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");
static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Delete) == boost::beast::http::verb::delete_,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");
static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Get) == boost::beast::http::verb::get,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");
static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Head) == boost::beast::http::verb::head,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");
static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Post) == boost::beast::http::verb::post,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");
static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Put) == boost::beast::http::verb::put,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");
static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Connect) == boost::beast::http::verb::connect,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");
static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Options) == boost::beast::http::verb::options,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");
static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Trace) == boost::beast::http::verb::trace,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");
static_assert(static_cast<boost::beast::http::verb>(Xi::Http::Method::Patch) == boost::beast::http::verb::patch,
              "The methods are internally casted, you cannot assign different values to them then expected by the "
              "beast library.");

std::string Xi::to_string(Xi::Http::Method method) {
  switch (method) {
    case Http::Method::Delete:
      return "DELETE";
    case Http::Method::Get:
      return "GET";
    case Http::Method::Head:
      return "HEAD";
    case Http::Method::Post:
      return "POST";
    case Http::Method::Put:
      return "PUT";
    case Http::Method::Connect:
      return "CONNECT";
    case Http::Method::Options:
      return "OPTIONS";
    case Http::Method::Trace:
      return "TRACE";
    case Http::Method::Patch:
      return "PATCH";
    default:
      return "UNKNOWN";
  }
}
