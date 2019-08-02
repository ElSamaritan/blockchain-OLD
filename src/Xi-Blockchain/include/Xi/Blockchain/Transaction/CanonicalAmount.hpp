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
#include <Xi/TypeSafe/Integral.hpp>
#include <Serialization/ISerializer.h>

#include "Xi/Blockchain/Transaction/Amount.hpp"

namespace Xi {
namespace Blockchain {
namespace Transaction {

struct CanonicalAmount : TypeSafe::EnableIntegralFromThis<uint64_t, CanonicalAmount> {
  using EnableIntegralFromThis::EnableIntegralFromThis;

  operator Amount() const;

  friend bool serialize(CanonicalAmount&, Common::StringView, CryptoNote::ISerializer&);
};

[[nodiscard]] bool serialize(CanonicalAmount& value, Common::StringView name, CryptoNote::ISerializer& serializer);

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

namespace CryptoNote {
using CanonicalAmount = Xi::Blockchain::Transaction::CanonicalAmount;
}
