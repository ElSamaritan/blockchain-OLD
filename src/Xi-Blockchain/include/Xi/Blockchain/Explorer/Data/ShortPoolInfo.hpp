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

#include <Serialization/ISerializer.h>
#include <CryptoNoteCore/CryptoNote.h>

namespace Xi {
namespace Blockchain {
namespace Explorer {

using PoolHash = ::Crypto::Hash;

struct ShortPoolInfo {
  /// unique hash reflecting the current pool state (Null if empty)
  PoolHash state_hash;

  /// number of transactions in pool
  uint64_t count;

  uint64_t cumulative_size;

  uint64_t cumulative_fees;

  KV_BEGIN_SERIALIZATION

  KV_MEMBER(state_hash)
  KV_MEMBER(count)
  KV_MEMBER(cumulative_size)
  KV_MEMBER(cumulative_fees)

  KV_END_SERIALIZATION
};

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi
