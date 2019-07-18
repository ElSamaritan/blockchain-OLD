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
#include <array>

#include <Xi/Global.hh>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>

#include "Xi/Config/VersionContainer.hpp"

namespace Xi {
namespace Config {
namespace Transaction {

class Fusion {
 public:
  XI_PROPERTY(uint64_t, maximumSize)
  XI_PROPERTY(uint64_t, minimumInputCount)
  XI_PROPERTY(uint64_t, ratioLimit)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(maximumSize(), max_size)
  KV_MEMBER_RENAME(minimumInputCount(), min_input)
  KV_MEMBER_RENAME(ratioLimit(), ratio_limit)
  KV_END_SERIALIZATION
};

class Transfer {
  XI_PROPERTY(uint64_t, maximumSize)
  XI_PROPERTY(uint64_t, minimumFee)
  XI_PROPERTY(uint64_t, freeBuckets)
  XI_PROPERTY(uint64_t, rateNominator)
  XI_PROPERTY(uint64_t, rateDenominator)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(maximumSize(), max_size)
  KV_MEMBER_RENAME(minimumFee(), min_fee)
  KV_MEMBER_RENAME(freeBuckets(), free_buckets)
  KV_MEMBER_RENAME(rateNominator(), rate_nominator)
  KV_MEMBER_RENAME(rateDenominator(), rate_denominator)
  KV_END_SERIALIZATION
};

class Mixin {
 public:
  XI_PROPERTY(uint8_t, minimum)
  XI_PROPERTY(uint8_t, maximum)
  XI_PROPERTY(uint64_t, upgradeSize)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(minimum(), min)
  KV_MEMBER_RENAME(maximum(), max)
  KV_MEMBER_RENAME(upgradeSize(), upgrade_size)
  KV_END_SERIALIZATION
};

class Configuration {
 public:
  XI_PROPERTY(Transfer, transfer)
  XI_PROPERTY(Fusion, fusion)
  XI_PROPERTY(Mixin, mixin)
  XI_PROPERTY(uint32_t, futureUnlockLimit)
  XI_PROPERTY(std::vector<uint16_t>, supportedVersions)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(transfer(), transfer)
  KV_MEMBER_RENAME(fusion(), fusion)
  KV_MEMBER_RENAME(mixin(), mixin)
  KV_MEMBER_RENAME(futureUnlockLimit(), future_unlock_limit)
  KV_MEMBER_RENAME(supportedVersions(), supported_versions)
  KV_END_SERIALIZATION
};

using Container = VersionContainer<Transaction::Configuration>;

}  // namespace Transaction
}  // namespace Config
}  // namespace Xi
