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
#include <vector>

#include <Xi/Blockchain/Block/Block.hpp>
#include <Xi/Blockchain/Transaction/Transaction.hpp>
#include <Serialization/ISerializer.h>

namespace XiMiner {

struct BlockSubmissionResult {
  XI_PROPERTY(Xi::Blockchain::Block::Hash, hash)
  XI_PROPERTY(Xi::Blockchain::Transaction::Amount, reward)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(hash(), hash)
  KV_MEMBER_RENAME(reward(), reward)
  KV_END_SERIALIZATION
};

}  // namespace XiMiner
