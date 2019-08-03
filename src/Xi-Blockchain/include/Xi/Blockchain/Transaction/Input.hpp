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

#include <Xi/Global.hh>
#include <Xi/Span.hpp>
#include <Serialization/VariantSerialization.hpp>

#include "Xi/Blockchain/Transaction/RewardInput.hpp"
#include "Xi/Blockchain/Transaction/AmountInput.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

// clang-format off
XI_SERIALIZATION_VARIANT_INVARIANT(
  Input,
    RewardInput,
    AmountInput
)
// clang-format on

using InputVector = std::vector<Input>;
XI_DECLARE_SPANS(Input)

bool operator<(const Input& lhs, const Input& rhs);

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Transaction::Input, 0, 0x01, "base")
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Transaction::Input, 1, 0x02, "key")

namespace CryptoNote {
using TransactionInput = Xi::Blockchain::Transaction::Input;
}
