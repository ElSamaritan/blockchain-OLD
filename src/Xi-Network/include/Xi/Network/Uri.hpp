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

#include <memory>
#include <string>
#include <string_view>
#include <optional>
#include <cinttypes>

#include <Xi/Global.hh>
#include <Xi/Result.h>

namespace Xi {
namespace Network {

class Uri {
 public:
  Result<Uri> fromString(const std::string& str);

 private:
  explicit Uri();

 public:
  explicit Uri(const Uri& other);
  Uri& operator=(const Uri& other);

  explicit Uri(Uri&& other);
  Uri& operator=(Uri&& other);

  ~Uri();

  const std::string& scheme() const;
  const std::string& host() const;
  const std::optional<uint16_t>& port() const;
  const std::string& path() const;
  const std::string& query() const;
  const std::string& fragment() const;

 private:
  struct _Impl;
  std::unique_ptr<_Impl> m_impl;
};

}  // namespace Network
}  // namespace Xi
