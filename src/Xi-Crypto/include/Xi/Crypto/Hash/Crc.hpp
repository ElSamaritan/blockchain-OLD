/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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
#include <Xi/Byte.hh>

#include "Xi/Crypto/Hash/Hash.hh"

namespace Xi {
namespace Crypto {
namespace Hash {
namespace Crc {
XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash16, 16);
XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash32, 32);

void compute(ConstByteSpan data, Hash16 &out);
void compute(ConstByteSpan data, Hash32 &out);
}  // namespace Crc
}  // namespace Hash
}  // namespace Crypto
}  // namespace Xi
