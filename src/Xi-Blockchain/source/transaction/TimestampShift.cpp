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

#include "Xi/Blockchain/Block/TimestampShift.hpp"

#include <exception>
#include <limits>

namespace Xi {
namespace Blockchain {
namespace Block {

bool serialize(TimestampShift &value, Common::StringView name, CryptoNote::ISerializer &serializer) {
  XI_RETURN_EC_IF_NOT(serializer(value.value, name), false);
  XI_RETURN_SC(true);
}

uint64_t TimestampShift::apply(uint64_t origin) const {
  uint64_t reval = 0;
  exceptional_if_not<OutOfRangeError>(tryApply(origin, reval));
  return reval;
}

bool TimestampShift::tryApply(uint64_t origin, uint64_t &out) const {
  if (native() < 0) {
    const auto unsignedValue = static_cast<uint64_t>(-1 * native());
    XI_RETURN_EC_IF(unsignedValue > origin, false);
    out = origin - unsignedValue;
    XI_RETURN_SC(true);
  } else {
    const auto unsignedValue = static_cast<uint64_t>(native());
    XI_RETURN_EC_IF(origin + unsignedValue < origin, false);
    out = origin + unsignedValue;
    XI_RETURN_SC(true);
  }
}

TimestampShift makeTimestampShift(uint64_t from, uint64_t to) {
  static_assert(std::numeric_limits<int64_t>::min() <= (-1 * std::numeric_limits<int64_t>::max()), "");
  if (to < from) {
    const auto shift = from - to;
    exceptional_if<OutOfRangeError>(shift > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));
    return TimestampShift{-1 * static_cast<int64_t>(shift)};
  } else {
    const auto shift = to - from;
    exceptional_if<OutOfRangeError>(shift > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()));
    return TimestampShift{static_cast<int64_t>(shift)};
  }
}

}  // namespace Block
}  // namespace Blockchain
}  // namespace Xi
