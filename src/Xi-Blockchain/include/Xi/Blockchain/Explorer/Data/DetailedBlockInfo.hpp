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

#include "Xi/Blockchain/Explorer/Data/ShortTransactionInfo.hpp"
#include "Xi/Blockchain/Explorer/Data/BlockInfo.hpp"
#include "Xi/Blockchain/Explorer/Data/DetailedTransactionInfo.hpp"

namespace Xi {
namespace Blockchain {
namespace Explorer {

struct DetailedBlockInfo {
  /// Lightweighted infos.
  BlockInfo info;
  /// Contains all transactions including miner reward and static reward.
  DetailedTransactionInfoVector transactions;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(info)
  KV_MEMBER(transactions)
  KV_END_SERIALIZATION
};

using DetailedBlockInfoVector = std::vector<DetailedBlockInfo>;

}  // namespace Explorer
}  // namespace Blockchain
}  // namespace Xi
