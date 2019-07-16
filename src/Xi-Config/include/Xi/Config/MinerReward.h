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

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>

#include "Xi/Config/VersionContainer.hpp"

/*!
 * \section Reward
 *
 * Window : Number of previous blocks for median size calculation, blocks with used to calculate penalities for larger
 *            blocks.
 * Zone   : Size in bytes until block penalties will be introduced. If a block is mined larger than the zone
 *            size the base reward will be adjusted. Further the transaction pool won't accept transactions larger than
 *            the zone.
 * CutOff : Number of lower digits cutted from the block reward to reduce block size. (0 <=> Disabled)
 *
 */

namespace Xi {
namespace Config {
namespace MinerReward {

class Configuration {
 public:
  XI_PROPERTY(uint64_t, windowSize)
  XI_PROPERTY(uint64_t, zone)
  XI_PROPERTY(uint8_t, cuttOff)
  XI_PROPERTY(uint64_t, reservedSize)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(windowSize(), window_size)
  KV_MEMBER_RENAME(zone(), zone)
  KV_MEMBER_RENAME(cuttOff(), cut_off)
  KV_MEMBER_RENAME(reservedSize(), reserved_size)
  KV_END_SERIALIZATION
};

using Container = VersionContainer<MinerReward::Configuration>;

}  // namespace MinerReward
}  // namespace Config
}  // namespace Xi
