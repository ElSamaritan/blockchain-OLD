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
#include <Xi/Crypto/FastHash.hpp>
#include <CryptoNoteCore/CryptoNote.h>

#include "CryptoNoteProtocol/Commands/CryptoNoteProtocolCommand.h"

namespace CryptoNote {
struct NOTIFY_REQUEST_TX_POOL_request {
  std::vector<Crypto::Hash> transactions;

  KV_BEGIN_SERIALIZATION KV_MEMBER(transactions) KV_END_SERIALIZATION
};

struct NOTIFY_REQUEST_TX_POOL {
  const static int ID = BC_COMMANDS_POOL_BASE + 6;
  typedef NOTIFY_REQUEST_TX_POOL_request request;
};

}  // namespace CryptoNote
