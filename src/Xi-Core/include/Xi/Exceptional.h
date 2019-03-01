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

#include <stdexcept>
#include <utility>
#include <type_traits>

namespace Xi {
template <typename _ExceptionT>
inline void exceptional() {
  static_assert(std::is_base_of<std::exception, _ExceptionT>::value, "Only exceptions can be exceptional.");
  throw _ExceptionT{};
}

template <typename _ExceptionT>
inline void exceptional(const char* s) {
  static_assert(std::is_base_of<std::exception, _ExceptionT>::value, "Only exceptions can be exceptional.");
  throw _ExceptionT{s};
}
}  // namespace Xi

#define XI_DECLARE_EXCEPTIONAL_CATEGORY(CAT)                \
  class CAT##Exception : public std::exception {            \
   private:                                                 \
    std::string m;                                          \
                                                            \
   protected:                                               \
    CAT##Exception(const char* s) : m{s} {}                 \
                                                            \
   public:                                                  \
    virtual ~CAT##Exception() = default;                    \
                                                            \
    const char* what() const noexcept { return m.c_str(); } \
  };

#define XI_DECLARE_EXCEPTIONAL_INSTANCE(X, MSG, CAT)      \
  class X##Error : public CAT##Exception {                \
    X##Error() : CAT##Exception(MSG) {}                   \
    X##Error(const char* s) : CAT##Exception(s) {}        \
                                                          \
    friend void ::Xi::exceptional<X##Error>();            \
    friend void ::Xi::exceptional<X##Error>(const char*); \
  };