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

#include <optional>
#include <cinttypes>

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Xi/Crypto/KeyImage.hpp>

#include "Xi/Blockchain/Transaction/CanonicalAmount.hpp"
#include "Xi/Blockchain/Transaction/GlobalIndex.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

struct AmountInput {
  /// Amount of the used input, all used outputs must correspond to this amount.
  CanonicalAmount amount{0};

  /// Double spending protection.
  Crypto::KeyImage keyImage{Crypto::KeyImage::Null};

  /// Delta encoded global indices of outputs used (real one + mixins)
  GlobalDeltaIndexVector outputIndices{};

  [[nodiscard]] bool serialize(CryptoNote::ISerializer& serializer, std::optional<uint16_t> ringSize = std::nullopt);
};

inline Amount consumedAmount(const AmountInput& input) {
  return input.amount.native();
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

namespace CryptoNote {
using KeyInput = Xi::Blockchain::Transaction::AmountInput;
}
