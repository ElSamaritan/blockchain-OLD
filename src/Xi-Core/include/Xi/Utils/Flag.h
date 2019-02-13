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

#include <type_traits>

#define XI_MAKE_FLAG_OPERATIONS(_FlagT)                                                                             \
  inline constexpr _FlagT operator|(_FlagT a, _FlagT b) noexcept {                                                  \
    return _FlagT(((std::underlying_type<_FlagT>::type)a) | ((std::underlying_type<_FlagT>::type)b));               \
  }                                                                                                                 \
  inline _FlagT &operator|=(_FlagT &a, _FlagT b) noexcept {                                                         \
    return (_FlagT &)(((std::underlying_type<_FlagT>::type &)a) |= ((std::underlying_type<_FlagT>::type)b));        \
  }                                                                                                                 \
  inline constexpr _FlagT operator&(_FlagT a, _FlagT b) noexcept {                                                  \
    return _FlagT(((std::underlying_type<_FlagT>::type)a) & ((std::underlying_type<_FlagT>::type)b));               \
  }                                                                                                                 \
  inline _FlagT &operator&=(_FlagT &a, _FlagT b) noexcept {                                                         \
    return (_FlagT &)(((std::underlying_type<_FlagT>::type &)a) &= ((std::underlying_type<_FlagT>::type)b));        \
  }                                                                                                                 \
  inline constexpr _FlagT operator~(_FlagT a) noexcept { return _FlagT(~((std::underlying_type<_FlagT>::type)a)); } \
  inline constexpr _FlagT operator^(_FlagT a, _FlagT b) noexcept {                                                  \
    return _FlagT(((std::underlying_type<_FlagT>::type)a) ^ ((std::underlying_type<_FlagT>::type)b));               \
  }                                                                                                                 \
  inline _FlagT &operator^=(_FlagT &a, _FlagT b) noexcept {                                                         \
    return (_FlagT &)(((std::underlying_type<_FlagT>::type &)a) ^= ((std::underlying_type<_FlagT>::type)b));        \
  }
