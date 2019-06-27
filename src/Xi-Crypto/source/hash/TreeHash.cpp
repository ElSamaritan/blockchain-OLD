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

#include "Xi/Crypto/Hash/TreeHash.hh"

#include <Xi/Exceptions.hpp>

#include "Xi/Crypto/Hash/Exceptions.hpp"

void Xi::Crypto::Hash::treeHash(const Xi::ConstByteSpan &data, size_t count, Xi::ByteSpan out) {
  exceptional_if<InvalidSizeError>(count == 0);
  exceptional_if<InvalidSizeError>(out.size_bytes() < XI_HASH_FAST_HASH_SIZE);
  exceptional_if_not<InvalidSizeError>(data.size_bytes() == count * XI_HASH_FAST_HASH_SIZE);
  const auto ec = xi_crypto_hash_tree_hash(reinterpret_cast<const xi_byte_t(*)[XI_HASH_FAST_HASH_SIZE]>(data.data()),
                                           count, out.data());
  exceptional_if_not<TreeHashError>(ec == XI_RETURN_CODE_SUCCESS);
}
