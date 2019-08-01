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
#include <Xi/Byte.hh>
#include <Serialization/ISerializer.h>
#include <Xi/Crypto/FastHash.hpp>

#include "Xi/Blockchain/Transaction/Input.hpp"
#include "Xi/Blockchain/Transaction/Output.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

struct Prefix {
  uint8_t version;
  uint64_t unlockTime;

  InputVector inputs;
  OutputVector outputs;

  ByteVector extra;

  [[nodiscard]] bool serialize(CryptoNote::ISerializer& serializer);

  Crypto::FastHash prefixHash() const;
  uint64_t prefixBinarySize() const;
};

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

namespace CryptoNote {
using TransactionPrefix = Xi::Blockchain::Transaction::Prefix;
}
