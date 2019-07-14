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

#include <cinttypes>
#include <utility>
#include <set>
#include <unordered_set>

namespace Xi {

template <typename _ValueT>
size_t merge(std::set<_ValueT>& out, std::set<_ValueT>&& values) {
#if defined(_MSC_VER)
  size_t actualInserted = 0;
  for (auto&& iValue : values) {
    if (out.insert(std::move(iValue)).second) {
      actualInserted += 1;
    }
  }
  return actualInserted;
#else
  size_t prevSize = out.size();
  out.merge(std::move(values));
  return out.size() - prevSize;
#endif
}

template <typename _ValueT>
size_t merge(std::unordered_set<_ValueT>& out, std::unordered_set<_ValueT>&& values) {
#if defined(_MSC_VER)
  size_t actualInserted = 0;
  for (auto&& iValue : values) {
    if (out.insert(std::move(iValue)).second) {
      actualInserted += 1;
    }
  }
  return actualInserted;
#else
  size_t prevSize = out.size();
  out.merge(std::move(values));
  return out.size() - prevSize;
#endif
}

}  // namespace Xi
