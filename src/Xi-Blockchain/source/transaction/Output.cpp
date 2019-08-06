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

#include "Xi/Blockchain/Transaction/Output.hpp"

#include <Xi/Exceptions.hpp>

namespace Xi {
namespace Blockchain {
namespace Transaction {

bool operator<(const Output& rhs, const Output& lhs) {
  XI_RETURN_SC_IF(lhs.index() < rhs.index(), true);
  XI_RETURN_SC_IF(lhs.index() > rhs.index(), false);
  if (std::holds_alternative<AmountOutput>(lhs)) {
    const auto& lhsAmount = std::get<AmountOutput>(lhs);
    const auto& rhsAmount = std::get<AmountOutput>(rhs);
    XI_RETURN_SC_IF(lhsAmount.amount < rhsAmount.amount, true);
    XI_RETURN_SC_IF(lhsAmount.amount > rhsAmount.amount, false);
    XI_RETURN_SC(lhsAmount.target < rhsAmount.target);
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

Amount generatedAmount(const Output& output) {
  if (std::holds_alternative<AmountOutput>(output)) {
    return generatedAmount(std::get<AmountOutput>(output));
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
