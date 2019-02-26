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

#include <vector>
#include <string>

#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <crypto/CryptoTypes.h>
#include <CryptoNoteCore/CryptoNote.h>

#include "CryptoNoteProtocol/Commands/CryptoNoteProtocolCommand.h"
#include "CryptoNoteProtocol/Structures/RawBlockLegacy.h"

namespace CryptoNote {
struct NOTIFY_REQUEST_GET_OBJECTS_request {
  std::vector<Crypto::Hash> txs;
  std::vector<Crypto::Hash> blocks;

  void serialize(ISerializer& s) {
    serializeAsBinary(txs, "txs", s);
    serializeAsBinary(blocks, "blocks", s);
  }
};

struct NOTIFY_REQUEST_GET_OBJECTS {
  const static int ID = BC_COMMANDS_POOL_BASE + 2;
  typedef NOTIFY_REQUEST_GET_OBJECTS_request request;
};

struct NOTIFY_RESPONSE_GET_OBJECTS_request {
  std::vector<std::string> txs;
  std::vector<RawBlockLegacy> blocks;
  std::vector<Crypto::Hash> missed_ids;
  uint32_t current_blockchain_height;
};

struct NOTIFY_RESPONSE_GET_OBJECTS {
  const static int ID = BC_COMMANDS_POOL_BASE + 3;
  typedef NOTIFY_RESPONSE_GET_OBJECTS_request request;
};
}  // namespace CryptoNote
