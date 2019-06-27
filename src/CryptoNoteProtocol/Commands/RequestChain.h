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

#include <vector>

#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <crypto/CryptoTypes.h>
#include <CryptoNoteCore/CryptoNote.h>

#include "CryptoNoteProtocol/Commands/CryptoNoteProtocolCommand.h"

namespace CryptoNote {
struct NOTIFY_REQUEST_CHAIN {
  const static int ID = BC_COMMANDS_POOL_BASE + 4;

  struct request {
    std::vector<Crypto::Hash>
        block_hashes; /*IDs of the first 10 blocks are sequential, next goes with pow(2,n) offset, like 2, 4, 8, 16, 32,
                      64 and so on, and the last one is always genesis block */

    KV_BEGIN_SERIALIZATION KV_MEMBER(block_hashes) KV_END_SERIALIZATION
  };
};

struct NOTIFY_RESPONSE_CHAIN_ENTRY_request {
  BlockHeight start_height;
  BlockHeight total_height;
  std::vector<Crypto::Hash> block_hashes;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(start_height)
  KV_MEMBER(total_height)
  KV_MEMBER(block_hashes)
  KV_END_SERIALIZATION
};

struct NOTIFY_RESPONSE_CHAIN_ENTRY {
  const static int ID = BC_COMMANDS_POOL_BASE + 5;
  typedef NOTIFY_RESPONSE_CHAIN_ENTRY_request request;
};
}  // namespace CryptoNote
