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
#include <Serialization/EnumSerialization.hpp>

namespace Xi {
namespace Blockchain {
namespace Transaction {

enum struct Type {
  None = 0,
  Reward = 1,
  Transfer = 2,
};

XI_SERIALIZATION_ENUM(Type)

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

XI_SERIALIZATION_ENUM_RANGE(Xi::Blockchain::Transaction::Type, Reward, Transfer)
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Transaction::Type, Reward, "reward")
XI_SERIALIZATION_ENUM_TAG(Xi::Blockchain::Transaction::Type, Transfer, "transfer")

namespace CryptoNote {
using TransactionType = Xi::Blockchain::Transaction::Type;
}
