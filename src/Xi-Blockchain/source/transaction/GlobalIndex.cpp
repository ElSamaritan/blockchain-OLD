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

#include "Xi/Blockchain/Transaction/GlobalIndex.hpp"

#include <algorithm>
#include <utility>

#include <Xi/Algorithm/Math.h>

namespace Xi {
namespace Blockchain {
namespace Transaction {

bool deltaEncodeGlobalIndices(GlobalIndexVector input, GlobalDeltaIndexVector &out) {
  if (input.empty()) {
    out.clear();
    XI_RETURN_SC(true);
  }
  std::sort(input.begin(), input.end());
  auto copy = input;
  for (size_t i = 1; i < copy.size(); ++i) {
    copy[i] = input[i] - input[i - 1];
  }
  out = std::move(copy);
  XI_RETURN_SC(true);
}

bool deltaDecodeGlobalIndices(GlobalDeltaIndexVector input, GlobalIndexVector &out, const GlobalIndex offset) {
  GlobalIndexVector res = input;
  if (res.empty()) {
    out.clear();
    XI_RETURN_SC(true);
  }
  XI_RETURN_EC_IF(hasAdditionOverflow(res.front(), offset, std::addressof(res.front())), false);
  for (size_t i = 1; i < res.size(); i++) {
    XI_RETURN_EC_IF(hasAdditionOverflow(res[i], res[i - 1], std::addressof(res[i])), false);
  }
  out = std::move(res);
  XI_RETURN_SC(true);
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
