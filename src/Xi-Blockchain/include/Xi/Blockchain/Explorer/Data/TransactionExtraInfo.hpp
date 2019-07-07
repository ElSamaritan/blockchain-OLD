﻿/* ============================================================================================== *
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
#include <string>
#include <optional>

#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <CryptoNoteCore/CryptoNote.h>

namespace Xi {
namespace Blockchain {
namespace Explorer {

struct TransactionExtraInfo {
  CryptoNote::BinaryArray raw;
  ::Crypto::PublicKey public_key;
  std::optional<CryptoNote::PaymentId> payment_id;

  KV_BEGIN_SERIALIZATION
  XI_RETURN_EC_IF_NOT(s.binary(raw.data(), raw.size(), "raw"), false);
  KV_MEMBER(public_key)
  KV_MEMBER(payment_id)
  KV_END_SERIALIZATION
};

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi