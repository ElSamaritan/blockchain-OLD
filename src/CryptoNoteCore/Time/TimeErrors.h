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
#include <system_error>

#include <Xi/Error.h>

namespace CryptoNote {

namespace error {

enum class TimeError { SUCCESS = 0, NEGATIVE_POSIX_TIMESTAMP };

class TimeErrorCategory : public std::error_category {
 public:
  static TimeErrorCategory INSTANCE;

  virtual const char* name() const noexcept { return "TimeErrorCategory"; }

  virtual std::error_condition default_error_condition(int ev) const noexcept {
    return std::error_condition(ev, *this);
  }

  virtual std::string message(int ev) const {
    TimeError code = static_cast<TimeError>(ev);

    switch (code) {
      case TimeError::SUCCESS:
        return "No Error";
      case TimeError::NEGATIVE_POSIX_TIMESTAMP:
        return "Implemenation returned a negative amount of seconds since epoch. DateTimes pre POSIX epoch are not "
               "supported.";
      default:
        return "Unknown error";
    }
  }

 private:
  TimeErrorCategory() = default;
};

inline std::error_code make_error_code(CryptoNote::error::TimeError e) {
  return std::error_code(static_cast<int>(e), CryptoNote::error::TimeErrorCategory::INSTANCE);
}

}  // namespace error
}  // namespace CryptoNote

namespace Xi {
inline Error make_error(CryptoNote::error::TimeError e) { return Error{CryptoNote::error::make_error_code(e)}; }
}  // namespace Xi

namespace std {

template <>
struct is_error_code_enum<CryptoNote::error::TimeError> : public true_type {};

}  // namespace std
