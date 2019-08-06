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

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>
#include <Xi/Crypto/PublicKey.hpp>

#include "Xi/Blockchain/Transaction/CanonicalAmount.hpp"
#include "Xi/Blockchain/Transaction/OutputTarget.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

struct AmountOutput {
  /// New amount generated once mined.
  CanonicalAmount amount;
  /// Target to receive the amount
  OutputTarget target;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(amount)
  KV_MEMBER(target)
  KV_END_SERIALIZATION
};

inline Amount generatedAmount(const AmountOutput& out) {
  return out.amount.native();
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

namespace CryptoNote {
using TransactionAmountOutput = Xi::Blockchain::Transaction::AmountOutput;
}
