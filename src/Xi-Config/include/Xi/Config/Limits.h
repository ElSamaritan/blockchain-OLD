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
#include <algorithm>
#include <chrono>

#include <Xi/Global.hh>
#include <Xi/Byte.hh>
#include <Xi/Time.h>
#include <Xi/Algorithm/Math.h>
#include <Serialization/ISerializer.h>

#include "Xi/Config/VersionContainer.hpp"

namespace Xi {
namespace Config {
namespace Limits {

inline constexpr std::chrono::seconds maximumTransactionLivetimeSpan() {
  return 24_h;
}
inline constexpr std::chrono::seconds maximumTransactionLivetimeSpanFromAltBlocks() {
  return 7 * 24_h;
}

/*!
 * \brief minimumTransactionLivetimeSpansUntilDeletion is the amount of lifetime spans a transaction must be hold in
 * the transaction pool before it can be deleted.
 *
 * Thus the minimum amount of time a transaction will remain in the transaction pool is:
 *   maximumTransactionLivetimeSpan() * minimumTransactionLivetimeSpansUntilDeletion()
 */
inline constexpr uint64_t minimumTransactionLivetimeSpansUntilDeletion() {
  return 7;
}

class Configuration {
 public:
  XI_PROPERTY(uint64_t, maximum)
  XI_PROPERTY(uint64_t, initial)
  XI_PROPERTY(uint64_t, windowSize)
  XI_PROPERTY(uint64_t, increaseRate)

  KV_BEGIN_SERIALIZATION
  KV_MEMBER_RENAME(maximum(), maximum)
  KV_MEMBER_RENAME(initial(), initial)
  KV_MEMBER_RENAME(windowSize(), window_size)
  KV_MEMBER_RENAME(increaseRate(), increase_rate)
  KV_END_SERIALIZATION
};

using Container = VersionContainer<Limits::Configuration>;

}  // namespace Limits
}  // namespace Config
}  // namespace Xi
