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

#include "Xi/Crypto/Hash/Shake.hh"

#include <Xi/Exceptions.hpp>

#include "Xi/Crypto/Hash/Exceptions.hpp"

XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Shake::Hash128, 128)
XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Shake::Hash256, 256)

void Xi::Crypto::Hash::Shake::compute(Xi::ConstByteSpan data, Hash128 &out) {
  exceptional_if_not<OpenSSLError>(xi_crypto_hash_shake_128(data.data(), data.size(), out.data()) ==
                                   XI_RETURN_CODE_SUCCESS);
}

void Xi::Crypto::Hash::Shake::compute(Xi::ConstByteSpan data, Hash256 &out) {
  exceptional_if_not<OpenSSLError>(xi_crypto_hash_shake_256(data.data(), data.size(), out.data()) ==
                                   XI_RETURN_CODE_SUCCESS);
}
