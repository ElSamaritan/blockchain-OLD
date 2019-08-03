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

#include "Xi/Blockchain/Transaction/OutputTarget.hpp"

#include <Xi/Exceptions.hpp>

namespace Xi {
namespace Blockchain {
namespace Transaction {

bool operator<(const OutputTarget& lhs, const OutputTarget& rhs) {
  XI_RETURN_SC_IF(lhs.index() < rhs.index(), true);
  XI_RETURN_SC_IF(lhs.index() > rhs.index(), false);
  if (std::holds_alternative<KeyOutputTarget>(lhs)) {
    const auto& lhsKey = std::get<KeyOutputTarget>(lhs);
    const auto& rhsKey = std::get<KeyOutputTarget>(rhs);
    XI_RETURN_SC(lhsKey.key < rhsKey.key);
  } else {
    exceptional<InvalidVariantTypeError>();
  }
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
