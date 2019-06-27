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

#include "Xi/Crypto/Hash/Sha3.hh"

#include <Xi/Exceptions.hpp>

#include "Xi/Crypto/Hash/Exceptions.hpp"

XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Sha3::Hash224, 224)
XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Sha3::Hash256, 256)
XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Sha3::Hash384, 384)
XI_CRYPTO_HASH_DECLARE_HASH_IMPLEMENTATION(Xi::Crypto::Hash::Sha3::Hash512, 512)

void Xi::Crypto::Hash::Sha3::compute(Xi::ConstByteSpan data, Hash224 &out) {
  exceptional_if_not<OpenSSLError>(xi_crypto_hash_sha3_224(data.data(), data.size(), out.data()) ==
                                   XI_RETURN_CODE_SUCCESS);
}

void Xi::Crypto::Hash::Sha3::compute(Xi::ConstByteSpan data, Hash256 &out) {
  exceptional_if_not<OpenSSLError>(xi_crypto_hash_sha3_256(data.data(), data.size(), out.data()) ==
                                   XI_RETURN_CODE_SUCCESS);
}

void Xi::Crypto::Hash::Sha3::compute(Xi::ConstByteSpan data, Hash384 &out) {
  exceptional_if_not<OpenSSLError>(xi_crypto_hash_sha3_384(data.data(), data.size(), out.data()) ==
                                   XI_RETURN_CODE_SUCCESS);
}

void Xi::Crypto::Hash::Sha3::compute(Xi::ConstByteSpan data, Hash512 &out) {
  exceptional_if_not<OpenSSLError>(xi_crypto_hash_sha3_512(data.data(), data.size(), out.data()) ==
                                   XI_RETURN_CODE_SUCCESS);
}
