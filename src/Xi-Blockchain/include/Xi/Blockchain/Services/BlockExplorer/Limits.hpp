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
namespace Services {
namespace BlockExplorer {

struct Limits {
  uint32_t detailed_blocks_limit = 1;
  uint32_t blocks_limit = 5;
  uint32_t short_blocks_limit = 31;

  uint32_t detailed_transactions_limit = 1;
  uint32_t transactions_limit = 10;
  uint32_t short_transactions_limit = 50;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(detailed_blocks_limit)
  KV_MEMBER(blocks_limit)
  KV_MEMBER(short_blocks_limit)

  KV_MEMBER(detailed_transactions_limit)
  KV_MEMBER(transactions_limit)
  KV_MEMBER(short_transactions_limit)
  KV_END_SERIALIZATION
};

}  // namespace BlockExplorer
}  // namespace Services
}  // namespace Blockchain
}  // namespace Xi
