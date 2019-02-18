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
#include <cinttypes>

#include <crypto/CryptoTypes.h>
#include <CryptoNoteCore/CryptoNote.h>

#include "CryptoNoteProtocol/Commands/CryptoNoteProtocolCommand.h"

namespace CryptoNote {
struct NOTIFY_MISSING_TXS_REQUEST {
  uint32_t current_blockchain_height;
  std::vector<Crypto::Hash> missing_txs;
};

struct NOTIFY_MISSING_TXS_RESPONSE {
  std::vector<BinaryArray> txs;
};

struct NOTIFY_MISSING_TXS_REQUEST_ENTRY {
  const static int ID = BC_COMMANDS_POOL_BASE + 8;
  typedef NOTIFY_MISSING_TXS_REQUEST request;
};

struct NOTIFY_MISSING_TXS_RESPONSE_ENTRY {
  const static int ID = BC_COMMANDS_POOL_BASE + 9;
  typedef NOTIFY_MISSING_TXS_RESPONSE request;
};
}  // namespace CryptoNote
