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

#include <Xi/Algorithm/Math.h>

namespace Xi {
namespace Blockchain {
namespace Transaction {

bool deltaEncodeGlobalIndices(const GlobalIndexVector &input, GlobalDeltaIndexVector &out) {
  out = input;
  std::sort(out.begin(), out.end());
  for (size_t i = 1; i < out.size(); ++i) {
    out[i] = out[i] - out[i - 1];
  }
  XI_RETURN_SC(true);
}

bool deltaDecodeGlobalIndices(const GlobalDeltaIndexVector &input, GlobalIndexVector &out, const GlobalIndex offset) {
  out = input;
  XI_RETURN_SC_IF(out.empty(), true);
  XI_RETURN_EC_IF(hasAdditionOverflow(out.front(), offset, std::addressof(out.front())), false);
  for (size_t i = 1; i < out.size(); i++) {
    XI_RETURN_EC_IF(hasAdditionOverflow(out[i], out[i - 1], std::addressof(out[i])), false);
  }
  XI_RETURN_SC(true);
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
