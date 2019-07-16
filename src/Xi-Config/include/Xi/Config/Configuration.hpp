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

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>

#include "Xi/Config/General.hpp"
#include "Xi/Config/Coin.h"
#include "Xi/Config/Network.h"
#include "Xi/Config/Upgrade.hpp"

namespace Xi {
namespace Config {

class Configuration {
 public:
  XI_PROPERTY(General::Configuration, general)
  XI_PROPERTY(Coin::Configuration, coin)
  XI_PROPERTY(Network::Configuration, network)
  XI_PROPERTY(std::vector<Upgrade::Configuration>, upgrades)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(general(), general)
  KV_MEMBER_RENAME(coin(), coin)
  KV_MEMBER_RENAME(network(), network)
  KV_MEMBER_RENAME(upgrades(), upgrades)
  KV_END_SERIALIZATION
};

}  // namespace Config
}  // namespace Xi
