/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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
#include <map>

#include <Serialization/ISerializer.h>
#include <crypto/CryptoTypes.h>
#include <CryptoNoteCore/CryptoNoteSerialization.h>

namespace XiSync {
struct BatchInfo {
  uint32_t StartIndex;
  uint32_t Count;
  uint64_t BinarySize;
  std::map<uint32_t, Crypto::Hash> Checkpoints;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(StartIndex)
  KV_MEMBER(Count)
  KV_MEMBER(BinarySize)
  KV_MEMBER(Checkpoints)
  KV_END_SERIALIZATION
};
}  // namespace XiSync
