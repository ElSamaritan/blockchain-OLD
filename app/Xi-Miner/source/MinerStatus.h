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

#include <crypto/CryptoTypes.h>
#include <Serialization/ISerializer.h>
#include <CryptoNoteCore/CryptoNoteSerialization.h>

namespace XiMiner {
struct MinerStatus {
  double current_hashrate;
  double average_hashrate;
  uint32_t blocks_mined;
  uint32_t active_threads;
  Crypto::Hash top_block;
  uint64_t difficulty;
  std::string algorithm;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(current_hashrate)
  KV_MEMBER(average_hashrate)
  KV_MEMBER(blocks_mined)
  KV_MEMBER(active_threads)
  KV_MEMBER(top_block)
  KV_MEMBER(difficulty)
  KV_MEMBER(algorithm)
  KV_END_SERIALIZATION
};
}  // namespace XiMiner
