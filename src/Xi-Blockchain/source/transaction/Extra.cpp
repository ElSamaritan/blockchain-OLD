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

#include "Xi/Blockchain/Transaction/Extra.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

const Extra Extra::Null{};

Extra::Extra() {
  nullify();
}

void Extra::nullify() {
  features = ExtraFeature::None;
  publicKey = std::nullopt;
  paymentId = std::nullopt;
}

bool Extra::isNull() const {
  XI_RETURN_SC_IF_NOT(features == ExtraFeature::None, false);
  XI_RETURN_SC_IF_NOT(publicKey, false);
  XI_RETURN_SC_IF_NOT(paymentId, false);
  XI_RETURN_SC(true);
}

bool Extra::serialize(CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer(features, "features"), false);
  XI_RETURN_EC_IF_NOT(serializer.optional(hasFlag(features, ExtraFeature::PublicKey), publicKey, "public_key"), false);
  XI_RETURN_EC_IF_NOT(serializer.optional(hasFlag(features, ExtraFeature::PaymentId), paymentId, "payment_id"), false);
  XI_RETURN_SC(true);
}

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi
