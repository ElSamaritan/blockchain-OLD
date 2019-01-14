/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include <exception>
#include <system_error>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/variant.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Global.h>

namespace Xi {
class Error {
 public:
  explicit Error(std::exception_ptr e);
  explicit Error(std::error_code ec);
  XI_DEFAULT_MOVE(Error);
  XI_DEFAULT_COPY(Error);
  ~Error() = default;

  std::string message() const;

 private:
  boost::variant<std::exception_ptr, std::error_code> m_error;
};
}  // namespace Xi
