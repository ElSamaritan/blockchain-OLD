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

#include <crypto/Types/Hash.h>

#include "Xi/Blockchain/Block/Version.hpp"
#include "Xi/Blockchain/Block/Nonce.hpp"

namespace Xi {
namespace Blockchain {
namespace Block {

struct Header {
  Version version;
  Version upgradeVote;
  Nonce nonce;
  uint64_t timestamp;
  ::Crypto::Hash previousBlockHash;

  [[nodiscard]] bool serialize(CryptoNote::ISerializer& serializer);

  ::Crypto::Hash headerHash() const;
  ::Crypto::Hash proofOfWorkPrefix() const;

 private:
  [[nodiscard]] bool serialize(CryptoNote::ISerializer& serializer, bool isPoWPrefix);
};

}  // namespace Block
}  // namespace Blockchain

}  // namespace Xi
