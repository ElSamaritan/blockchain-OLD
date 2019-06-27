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
#include <Xi/Byte.hh>

#include "Xi/Crypto/Hash/Hash.hh"

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>

int xi_crypto_hash_sha3_224(const xi_byte_t *data, size_t length, xi_crypto_hash_224 out);
int xi_crypto_hash_sha3_256(const xi_byte_t *data, size_t length, xi_crypto_hash_256 out);
int xi_crypto_hash_sha3_384(const xi_byte_t *data, size_t length, xi_crypto_hash_384 out);
int xi_crypto_hash_sha3_512(const xi_byte_t *data, size_t length, xi_crypto_hash_512 out);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

namespace Xi {
namespace Crypto {
namespace Hash {
namespace Sha3 {
XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash224, 224)
XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash256, 256)
XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash384, 384)
XI_CRYPTO_HASH_DECLARE_HASH_TYPE(Hash512, 512)

void compute(ConstByteSpan data, Hash224 &out);
void compute(ConstByteSpan data, Hash256 &out);
void compute(ConstByteSpan data, Hash384 &out);
void compute(ConstByteSpan data, Hash512 &out);
}  // namespace Sha3
}  // namespace Hash
}  // namespace Crypto
}  // namespace Xi

#endif
