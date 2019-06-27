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
#include <string>
#include <vector>
#include <variant>

#include <Serialization/ISerializer.h>
#include <Serialization/OptionalSerialization.hpp>
#include <Serialization/VariantSerialization.hpp>
#include <CryptoNoteCore/CryptoNote.h>

#include "Xi/Blockchain/Explorer/Data/ShortBlockInfo.hpp"
#include "Xi/Blockchain/Explorer/Data/ShortTransactionInfo.hpp"
#include "Xi/Blockchain/Explorer/Data/TransactionExtraInfo.hpp"

namespace Xi {
namespace Blockchain {
namespace Explorer {

using PaymentId = CryptoNote::PaymentId;

struct ShortTransactionBaseInputInfo {
  Block::Height height;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(height)
  KV_END_SERIALIZATION
};

struct ShortTransactionKeyInputInfo {
  CryptoNote::Amount amount;
  uint64_t ring_size;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(amount)
  KV_MEMBER(ring_size)
  KV_END_SERIALIZATION
};

using ShortTransactionInputInfo = std::variant<ShortTransactionBaseInputInfo, ShortTransactionKeyInputInfo>;
using ShortTransactionInputInfoVector = std::vector<ShortTransactionInputInfo>;

struct ShortTransactionKeyOutputInfo {
  CryptoNote::Amount amount;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(amount)
  KV_END_SERIALIZATION
};

using ShortTransactionOutputInfo = std::variant<ShortTransactionKeyOutputInfo>;
using ShortTransactionOutputInfoVector = std::vector<ShortTransactionOutputInfo>;

struct TransactionInfo : ShortTransactionInfo {
  TransactionExtraInfo extra;

  ShortTransactionInputInfoVector inputs;
  ShortTransactionOutputInfoVector outputs;

  std::optional<ShortBlockInfo> block;

  KV_BEGIN_SERIALIZATION
  KV_BASE(ShortTransactionInfo)

  KV_MEMBER(extra)

  KV_MEMBER(inputs)
  KV_MEMBER(outputs)

  KV_MEMBER(block)

  KV_END_SERIALIZATION
};

using TransactionInfoVector = std::vector<TransactionInfo>;

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Explorer::ShortTransactionInputInfo, 0, 0x0001, "base")
XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Explorer::ShortTransactionInputInfo, 1, 0x0002, "key")

XI_SERIALIZATION_VARIANT_TAG(Xi::Blockchain::Explorer::ShortTransactionOutputInfo, 0, 0x0001, "key")
