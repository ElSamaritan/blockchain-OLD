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
#include <Xi/TypeSafe/Flag.hpp>
#include <Serialization/FlagSerialization.hpp>

namespace Xi {
namespace Blockchain {
namespace Transaction {

enum struct Feature {
  None = 0,
  UniformUnlock = 1 << 0,
  GlobalIndexOffset = 1 << 1,
  StaticRingSize = 1 << 2,
};

XI_TYPESAFE_FLAG_MAKE_OPERATIONS(Feature)
XI_SERIALIZATION_FLAG(Feature)

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_FLAG_RANGE(Xi::Blockchain::Transaction::Feature, UniformUnlock, StaticRingSize)
XI_SERIALIZATION_FLAG_TAG(Xi::Blockchain::Transaction::Feature, UniformUnlock, "uniform_unlock")
XI_SERIALIZATION_FLAG_TAG(Xi::Blockchain::Transaction::Feature, GlobalIndexOffset, "global_index_offset")
XI_SERIALIZATION_FLAG_TAG(Xi::Blockchain::Transaction::Feature, StaticRingSize, "static_ring_size")

namespace CryptoNote {
using TransactionFeature = Xi::Blockchain::Transaction::Feature;
}
