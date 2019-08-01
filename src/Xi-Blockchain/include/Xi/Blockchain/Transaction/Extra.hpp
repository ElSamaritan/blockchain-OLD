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
#include <Xi/Crypto/PublicKey.hpp>
#include <Serialization/ISerializer.h>

#include "Xi/Blockchain/Transaction/ExtraFeature.hpp"
#include "Xi/Blockchain/Transaction/PaymentId.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

struct Extra {
  static const Extra Null;

  ExtraFeature features{ExtraFeature::None};
  std::optional<Crypto::PublicKey> publicKey{std::nullopt};
  std::optional<PaymentId> paymentId{std::nullopt};

  explicit Extra();

  void nullify();
  bool isNull() const;

  bool serialize(CryptoNote::ISerializer& serializer);
};

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

namespace CryptoNote {
using TransactionExtra = Xi::Blockchain::Transaction::Extra;
}  // namespace CryptoNote
