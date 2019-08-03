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
#include <optional>

#include <Xi/Crypto/FastHash.hpp>
#include <Xi/Crypto/Hash/Crc.hpp>

#include "Xi/Blockchain/Block/Hash.hpp"
#include "Xi/Blockchain/Block/Version.hpp"
#include "Xi/Blockchain/Block/Nonce.hpp"
#include "Xi/Blockchain/Block/MergeMiningTag.hpp"
#include "Xi/Blockchain/Block/Feature.hpp"
#include "Xi/Blockchain/Block/TimestampShift.hpp"

namespace Xi {
namespace Blockchain {
namespace Block {

struct Header {
  Version version;

  /// lightweighted optionals encoding
  Feature features{Feature::None};

  Nonce nonce;
  TimestampShift timestamp{0};
  Crypto::FastHash previousBlockHash;

  std::optional<PrunableMergeMiningTag> mergeMiningTag;
  std::optional<Crypto::Hash::Crc::Hash16> staticRewardHash;

  [[nodiscard]] bool serialize(CryptoNote::ISerializer& serializer);

  Hash headerHash() const;
  Hash proofOfWorkPrefix() const;
  Hash proofOfWorkHash(const Hash& transactionTreeHash) const;

 private:
  [[nodiscard]] bool serialize(CryptoNote::ISerializer& serializer, bool isPoWPrefix);
};

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi
