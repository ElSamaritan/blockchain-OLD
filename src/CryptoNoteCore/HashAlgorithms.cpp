/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018 Galaxia Project Developers                                                      *
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

#include "CryptoNoteCore/HashAlgorithms.h"

#include "crypto/cnx/cnx.h"
#include "crypto/hash-predef.h"

void CryptoNote::Hashes::InitBlockchain::operator()(const CryptoNote::CachedBlock &block, Crypto::Hash &hash) const {
  const auto &rawHashingBlock = block.getBlockHashingBinaryArray();
  cn_slow_hash_v0(rawHashingBlock.data(), rawHashingBlock.size(), hash);
}

void CryptoNote::Hashes::CNX_v0::operator()(const CryptoNote::CachedBlock &block, Crypto::Hash &hash) const {
  const auto &rawHashingBlock = block.getParentBlockHashingBinaryArray(true);
  Crypto::CNX::Hash_v0{}(rawHashingBlock.data(), rawHashingBlock.size(), hash);
}
