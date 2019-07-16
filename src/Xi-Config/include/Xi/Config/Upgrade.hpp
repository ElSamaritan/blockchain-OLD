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

#include <Xi/Global.hh>
#include <Xi/Blockchain/Block/Height.hpp>
#include <Serialization/ISerializer.h>
#include <Serialization/OptionalSerialization.hpp>

#include "Xi/Config/MinerReward.h"
#include "Xi/Config/StaticReward.h"
#include "Xi/Config/Limits.h"
#include "Xi/Config/Time.h"
#include "Xi/Config/Transaction.h"
#include "Xi/Config/Difficulty.h"
#include "Xi/Config/MergeMining.hpp"

namespace Xi {
namespace Config {
namespace Upgrade {

class Configuration {
 public:
  XI_PROPERTY(Blockchain::Block::Height, height)
  XI_PROPERTY(bool, fork)

  XI_PROPERTY(std::optional<MinerReward::Configuration>, minerReward)
  XI_PROPERTY(std::optional<StaticReward::Configuration>, staticReward)
  XI_PROPERTY(std::optional<Time::Configuration>, time)
  XI_PROPERTY(std::optional<Limits::Configuration>, limit)
  XI_PROPERTY(std::optional<Transaction::Configuration>, transaction)
  XI_PROPERTY(std::optional<Difficulty::Configuration>, difficulty)
  XI_PROPERTY(std::optional<MergeMining::Configuration>, mergeMining)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(height(), height)
  KV_MEMBER_RENAME(fork(), fork)

  KV_MEMBER_RENAME(minerReward(), miner_reward)
  KV_MEMBER_RENAME(staticReward(), static_reward)
  KV_MEMBER_RENAME(time(), time)
  KV_MEMBER_RENAME(limit(), limit)
  KV_MEMBER_RENAME(transaction(), transaction)
  KV_MEMBER_RENAME(difficulty(), difficulty)
  KV_MEMBER_RENAME(mergeMining(), merge_mining)
  KV_END_SERIALIZATION
};

}  // namespace Upgrade
}  // namespace Config
}  // namespace Xi
