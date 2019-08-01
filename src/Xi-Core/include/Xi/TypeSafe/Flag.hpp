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

#include <type_traits>

#define XI_TYPESAFE_FLAG_MAKE_OPERATIONS(_FlagT)                                                             \
  inline constexpr _FlagT operator|(_FlagT a, _FlagT b) noexcept {                                           \
    return _FlagT(((std::underlying_type<_FlagT>::type)a) | ((std::underlying_type<_FlagT>::type)b));        \
  }                                                                                                          \
  inline _FlagT &operator|=(_FlagT &a, _FlagT b) noexcept {                                                  \
    return (_FlagT &)(((std::underlying_type<_FlagT>::type &)a) |= ((std::underlying_type<_FlagT>::type)b)); \
  }                                                                                                          \
  inline constexpr _FlagT operator&(_FlagT a, _FlagT b) noexcept {                                           \
    return _FlagT(((std::underlying_type<_FlagT>::type)a) & ((std::underlying_type<_FlagT>::type)b));        \
  }                                                                                                          \
  inline _FlagT &operator&=(_FlagT &a, _FlagT b) noexcept {                                                  \
    return (_FlagT &)(((std::underlying_type<_FlagT>::type &)a) &= ((std::underlying_type<_FlagT>::type)b)); \
  }                                                                                                          \
  inline constexpr _FlagT operator~(_FlagT a) noexcept {                                                     \
    return _FlagT(~((std::underlying_type<_FlagT>::type)a));                                                 \
  }                                                                                                          \
  inline constexpr _FlagT operator^(_FlagT a, _FlagT b) noexcept {                                           \
    return _FlagT(((std::underlying_type<_FlagT>::type)a) ^ ((std::underlying_type<_FlagT>::type)b));        \
  }                                                                                                          \
  inline _FlagT &operator^=(_FlagT &a, _FlagT b) noexcept {                                                  \
    return (_FlagT &)(((std::underlying_type<_FlagT>::type &)a) ^= ((std::underlying_type<_FlagT>::type)b)); \
  }

template <typename _FlagT>
[[nodiscard]] inline constexpr bool hasFlag(_FlagT value, _FlagT flags) {
  static_assert(std::is_enum_v<_FlagT>, "flags are only well defined for enum types.");
  return (value & flags) == flags;
}

template <typename _FlagT>
[[nodiscard]] inline constexpr _FlagT discardFlag(_FlagT value, _FlagT flags) {
  static_assert(std::is_enum_v<_FlagT>, "flags are only well defined for enum types.");
  return value & (~flags);
}

template <typename _FlagT>
[[nodiscard]] inline constexpr bool hasAnyOtherFlag(_FlagT value, _FlagT flags) {
  static_assert(std::is_enum_v<_FlagT>, "flags are only well defined for enum types.");
  return static_cast<std::underlying_type_t<_FlagT>>(discardFlag(value, flags)) != 0;
}
