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

// Thanks to Lily Ballard for this straight solution:
//      https://stackoverflow.com/questions/14861018/center-text-in-fixed-width-field-with-stream-manipulators-in-c

#pragma once

#include <string>
#include <iostream>
#include <iomanip>

namespace CommonCLI {
template <typename charT, typename traits = std::char_traits<charT> >
class center_helper {
  std::basic_string<charT, traits> str_;

 public:
  center_helper(std::basic_string<charT, traits> str) : str_(str) {}
  template <typename a, typename b>
  friend std::basic_ostream<a, b>& operator<<(std::basic_ostream<a, b>& s, const center_helper<a, b>& c);
};

template <typename charT, typename traits = std::char_traits<charT> >
center_helper<charT, traits> centered(std::basic_string<charT, traits> str) {
  return center_helper<charT, traits>(str);
}

// redeclare for std::string directly so we can support anything that implicitly converts to std::string
center_helper<std::string::value_type, std::string::traits_type> centered(const std::string& str) {
  return center_helper<std::string::value_type, std::string::traits_type>(str);
}

template <typename charT, typename traits>
std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& s,
                                              const center_helper<charT, traits>& c) {
  std::streamsize w = static_cast<std::streamsize>(s.width());
  if (w > static_cast<std::streamsize>(c.str_.length())) {
    std::streamsize left = (w + static_cast<std::streamsize>(c.str_.length())) / 2;
    s.width(left);
    s << c.str_;
    s.width(w - left);
    s << "";
  } else {
    s << c.str_;
  }
  return s;
}
}  // namespace CommonCLI
