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
#include <optional>

#include <crypto/Types/Hash.h>
#include <Serialization/ISerializer.h>

#include "CryptoNoteCore/Blockchain/BlockFeatures.hpp"
#include "CryptoNoteCore/Blockchain/BlockHeaderNonce.hpp"

namespace CryptoNote {

struct BlockHeader {
  uint8_t majorVersion;
  uint8_t minorVersion;
  BlockFeatures features;
  BlockHeaderNonce nonce;
  uint64_t timestamp;
  Crypto::Hash previousBlockHash;

  [[nodiscard]] bool serialize(ISerializer& serializer);
};

}  // namespace CryptoNote
