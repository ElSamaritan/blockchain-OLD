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

#include "Xi/Crypto/Hash/Keccak.hh"

XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Keccak::Hash256, 256);
XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Keccak::Hash1600, 1600);

#include "Xi/Crypto/Hash/Exceptions.hpp"

void Xi::Crypto::Hash::Keccak::compute(Xi::ConstByteSpan data, Hash256 &out) {
  exceptional_if_not<KeccakError>(xi_crypto_hash_keccak(data.data(), data.size_bytes(), out.data(), Hash256::bytes()) ==
                                  XI_RETURN_CODE_SUCCESS);
}

void Xi::Crypto::Hash::Keccak::compute(Xi::ConstByteSpan data, Hash1600 &out) {
  exceptional_if_not<KeccakError>(xi_crypto_hash_keccak_1600(data.data(), data.size_bytes(), out.data()) ==
                                  XI_RETURN_CODE_SUCCESS);
}
