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

#include <string>

#include <Xi/Algorithm/Math.h>
#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>

#include "Xi/Config/NetworkType.h"
#include "Xi/Config/AddressPrefix.hpp"

namespace Xi {
namespace Config {
namespace Coin {

/*
 * How to generate a premine:
 * - Compile your code
 * - Run xi-wallet, ignore that it can't connect to the daemon, and generate an address. Save this and the keys
 * somewhere safe.
 * - Launch the daemon with these arguments:
 *        xi-daemon --print-genesis-tx --genesis-block-reward-address <premine wallet address>
 * - Take the hash printed, and replace it with the hash below in GENESIS_COINBASE_TX_HEX
 * - Recompile, setup your seed nodes, and start mining
 * - You should see your premine appear in the previously generated wallet.
 */

class Configuration {
 public:
  XI_PROPERTY(std::string, name)
  XI_PROPERTY(std::string, ticker)
  XI_PROPERTY(AddressPrefix, prefix)

  XI_PROPERTY(uint8_t, decimals)
  XI_PROPERTY(uint64_t, totalSupply)
  XI_PROPERTY(uint64_t, premine)

  XI_PROPERTY(uint16_t, blockTime)
  XI_PROPERTY(uint32_t, rewardUnlockTime)
  XI_PROPERTY(uint32_t, emissionSpeed)

  XI_PROPERTY(uint64_t, startTimestamp)
  XI_PROPERTY(std::string, genesisTransaction)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(name(), name)
  KV_MEMBER_RENAME(ticker(), ticker)
  KV_MEMBER_RENAME(prefix(), prefix)

  KV_MEMBER_RENAME(decimals(), decimals)
  KV_MEMBER_RENAME(totalSupply(), total_supply)
  KV_MEMBER_RENAME(premine(), premine)

  KV_MEMBER_RENAME(blockTime(), block_time)
  KV_MEMBER_RENAME(rewardUnlockTime(), reward_unlock_time)
  KV_MEMBER_RENAME(emissionSpeed(), emission_speed)
  XI_RETURN_EC_IF_NOT(emissionSpeed() <= 8 * sizeof(uint64_t), false);

  KV_MEMBER_RENAME(startTimestamp(), start_timestamp)
  KV_MEMBER_RENAME(genesisTransaction(), genesis_transaction)
  KV_END_SERIALIZATION

  uint32_t expectedBlocksPerDay() const {
    return (24ULL * 60ULL * 60ULL) / blockTime();
  }
};

}  // namespace Coin
}  // namespace Config
}  // namespace Xi
