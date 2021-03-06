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

#include <Xi/Global.hh>
#include <Serialization/VariantSerialization.hpp>

#include "Xi/Blockchain/Transaction/KeyOutputTarget.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

// clang-format off
XI_SERIALIZATION_VARIANT_INVARIANT(
  OutputTarget,
    KeyOutputTarget
)
// clang-format on

bool operator<(const OutputTarget& lhs, const OutputTarget& rhs);

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Transaction::OutputTarget, 0, 0x01, "key")

namespace CryptoNote {
using TransactionOutputTarget = Xi::Blockchain::Transaction::OutputTarget;
}
