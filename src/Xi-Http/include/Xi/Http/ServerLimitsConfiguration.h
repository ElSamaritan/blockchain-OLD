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

#include <chrono>

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>
#include <Serialization/ChronoSerialization.h>

namespace Xi {
namespace Http {

class ServerLimitsConfiguration {
 public:
  /// Timeout once reached on request connection is killed.
  XI_PROPERTY(std::chrono::seconds, readTimeout, std::chrono::seconds{20})
  /// Timeout once reached while writing response connection is killed.
  XI_PROPERTY(std::chrono::seconds, writeTimeout, std::chrono::seconds{20})
  /// Max bytes read per second.
  XI_PROPERTY(size_t, readLimit, 256 * 1024)
  /// Max bytes written per second.
  XI_PROPERTY(size_t, writeLimit, 2 * 1024 * 1024)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(readTimeout(), read_timeout)
  KV_MEMBER_RENAME(writeTimeout(), write_timeout)
  KV_MEMBER_RENAME(readLimit(), read_limit)
  KV_MEMBER_RENAME(writeLimit(), write_limit)
  KV_END_SERIALIZATION
};

}  // namespace Http
}  // namespace Xi
