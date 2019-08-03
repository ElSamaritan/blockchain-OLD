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
#include <vector>

#include <Xi/Global.hh>

namespace Xi {
namespace Blockchain {
namespace Transaction {

using GlobalIndex = uint32_t;
using GlobalIndexVector = std::vector<GlobalIndex>;

using GlobalDeltaIndex = uint32_t;
using GlobalDeltaIndexVector = std::vector<GlobalDeltaIndex>;

[[nodiscard]] bool deltaEncodeGlobalIndices(const GlobalIndexVector& input, GlobalDeltaIndexVector& out);
[[nodiscard]] bool deltaDecodeGlobalIndices(const GlobalDeltaIndexVector& input, GlobalIndexVector& out,
                                            const GlobalIndex offset = 0);

}  // namespace Transaction
}  // namespace Blockchain
}  // namespace Xi

namespace CryptoNote {
using GlobalIndex = Xi::Blockchain::Transaction::GlobalIndex;
using GlobalIndexVector = Xi::Blockchain::Transaction::GlobalIndexVector;

using GlobalDeltaIndex = Xi::Blockchain::Transaction::GlobalDeltaIndex;
using GlobalDeltaIndexVector = Xi::Blockchain::Transaction::GlobalDeltaIndexVector;

using Xi::Blockchain::Transaction::deltaDecodeGlobalIndices;
using Xi::Blockchain::Transaction::deltaEncodeGlobalIndices;
}  // namespace CryptoNote
