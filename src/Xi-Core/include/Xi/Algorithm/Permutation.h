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

#include <vector>
#include <cinttypes>
#include <algorithm>
#include <iterator>

#include <Xi/Global.hh>

namespace Xi {

using Permutation = std::vector<uint64_t>;

inline Permutation makePermutation(size_t size) {
  Permutation reval{};
  reval.resize(size);
  for (size_t i = 0; i < size; ++i) {
    reval[i] = i;
  }
  return reval;
}

template <typename _RandIter, typename _Pred>
inline void sortPermutation(_RandIter begin, Permutation& permutation, const _Pred& pred) {
  std::sort(permutation.begin(), permutation.end(), [&pred, begin](uint64_t lhs, uint64_t rhs) {
    return pred(*std::next(begin, lhs), *std::next(begin, rhs));
  });
}

template <typename _RandIter>
inline void sortPermutation(_RandIter begin, Permutation& permutation) {
  return sortPermutation(begin, permutation, std::less<decltype(*begin)>{});
}

template <typename _RandIter>
inline void applyPermutation(_RandIter begin, Permutation permutation) {
  for (size_t i = 0; i < permutation.size(); ++i) {
    size_t current = i;
    while (i != permutation[current]) {
      size_t next = permutation[current];
      std::iter_swap(std::next(begin, current), std::next(begin, next));
      permutation[current] = current;
      current = next;
    }
    permutation[current] = current;
  }
}

}  // namespace Xi
