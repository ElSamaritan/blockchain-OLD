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

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>

#include "Xi/Blockchain/Transaction/GlobalIndex.hpp"
#include "Xi/Blockchain/Transaction/Amount.hpp"
#include "Xi/Blockchain/Transaction/Hash.hpp"
#include "Xi/Blockchain/Transaction/RewardInput.hpp"
#include "Xi/Blockchain/Transaction/AmountInput.hpp"
#include "Xi/Blockchain/Transaction/Input.hpp"
#include "Xi/Blockchain/Transaction/KeyOutputTarget.hpp"
#include "Xi/Blockchain/Transaction/OutputTarget.hpp"
#include "Xi/Blockchain/Transaction/AmountOutput.hpp"
#include "Xi/Blockchain/Transaction/Output.hpp"
#include "Xi/Blockchain/Transaction/PaymentId.hpp"
#include "Xi/Blockchain/Transaction/Prefix.hpp"
#include "Xi/Blockchain/Transaction/Signature.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

struct Transaction : public Prefix {
  static const Transaction Null;

  std::optional<SignatureCollection> signatures;

  explicit Transaction();
  XI_DEFAULT_COPY(Transaction);
  XI_DEFAULT_MOVE(Transaction);
  ~Transaction();

  Hash hash() const;
  uint64_t binarySize() const;
  uint64_t signaturesBinarySize() const;

  bool isNull() const;
  void nullify();
  void prune();

  [[nodiscard]] bool serialize(CryptoNote::ISerializer& serializer);

 private:
  [[nodiscard]] bool serializeReward(CryptoNote::ISerializer& serializer);
  [[nodiscard]] bool serializeTransfer(CryptoNote::ISerializer& serializer);
};

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

namespace CryptoNote {
using Transaction = Xi::Blockchain::Transaction::Transaction;
}
