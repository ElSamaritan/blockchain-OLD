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

#include "CryptoNoteCore/EligibleIndex.h"

#include <algorithm>

const CryptoNote::EligibleIndex CryptoNote::EligibleIndex::Always{0, 0};
const CryptoNote::EligibleIndex CryptoNote::EligibleIndex::Never{std::numeric_limits<uint32_t>::max(),
                                                                 std::numeric_limits<uint64_t>::max()};

CryptoNote::EligibleIndex CryptoNote::EligibleIndex::lowerBound(const CryptoNote::EligibleIndex &lhs,
                                                                const CryptoNote::EligibleIndex &rhs) {
  return EligibleIndex{std::max(lhs.Height, rhs.Height), std::max(lhs.Timestamp, rhs.Timestamp)};
}

CryptoNote::EligibleIndex::EligibleIndex() : Height{0}, Timestamp{0}, MixinUpgrades{} {
}
CryptoNote::EligibleIndex::EligibleIndex(uint32_t height, uint64_t timestamp)
    : Height{height}, Timestamp{timestamp}, MixinUpgrades{} {
}

bool CryptoNote::EligibleIndex::isSatisfiedByIndex(const CryptoNote::EligibleIndex &index) const {
  return Height <= index.Height && Timestamp <= index.Timestamp;
}
