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

#include <Xi/ExternalIncludePush.h>
#include <boost/beast/core/basic_stream.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/core/rate_policy.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <Xi/ExternalIncludePop.h>

namespace Xi {
namespace Http {

// clang-format off
using ServerStream = boost::beast::basic_stream<
    boost::beast::net::ip::tcp,
    boost::beast::net::io_context::executor_type,
    boost::beast::simple_rate_policy
>;

using SslServerStream = boost::beast::ssl_stream<
    ServerStream
>;

using ClientStream = boost::beast::tcp_stream;

using SslClientStream = boost::beast::ssl_stream<
    ClientStream
>;
// clang-format on

}  // namespace Http
}  // namespace Xi
