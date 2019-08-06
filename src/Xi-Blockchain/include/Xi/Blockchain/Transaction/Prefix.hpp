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
#include <Xi/Byte.hh>
#include <Serialization/ISerializer.h>
#include <Xi/Crypto/FastHash.hpp>

#include "Xi/Blockchain/Transaction/Amount.hpp"
#include "Xi/Blockchain/Transaction/Input.hpp"
#include "Xi/Blockchain/Transaction/Output.hpp"
#include "Xi/Blockchain/Transaction/Extra.hpp"
#include "Xi/Blockchain/Transaction/Feature.hpp"
#include "Xi/Blockchain/Transaction/Type.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

struct Prefix {
  uint8_t version{0};

  Type type{Type::None};
  Feature features{Feature::None};

  uint64_t unlockTime{0};
  std::optional<GlobalIndex> globalIndexOffset{std::nullopt};
  std::optional<uint16_t> staticRingSize{std::nullopt};

  InputVector inputs{};
  OutputVector outputs{};
  Extra extra{};

  Amount generatedAmount() const;
  Amount consumedAmount() const;

  [[nodiscard]] bool serialize(CryptoNote::ISerializer& serializer);

  Crypto::FastHash prefixHash() const;
  uint64_t prefixBinarySize() const;
  void nullifyPrefix();

 private:
  [[nodiscard]] bool serializeUnlock(CryptoNote::ISerializer& serializer);
  [[nodiscard]] bool serializeReward(CryptoNote::ISerializer& serializer);
  [[nodiscard]] bool serializeTransfer(CryptoNote::ISerializer& serializer);
  [[nodiscard]] bool serializeOutputs(CryptoNote::ISerializer& serializer);
};

inline Amount generatedAmount(const Prefix& prefix) {
  return prefix.generatedAmount();
}

inline Amount consumedAmount(const Prefix& prefix) {
  return prefix.consumedAmount();
}

inline Amount emission(const Prefix& prefix) {
  return generatedAmount(prefix) - consumedAmount(prefix);
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

namespace CryptoNote {
using TransactionPrefix = Xi::Blockchain::Transaction::Prefix;
}
