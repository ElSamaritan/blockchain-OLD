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

#include "CryptoNoteCore/Transactions/Transaction.h"

#include <array>

#include <Xi/Byte.hh>
#include <Xi/Exceptions.hpp>
#include <Common/VectorOutputStream.h>
#include <Serialization/BinaryOutputStreamSerializer.h>

#include "CryptoNoteCore/CryptoNoteSerialization.h"

const CryptoNote::Transaction CryptoNote::Transaction::Null{};

CryptoNote::Transaction::Transaction() { nullify(); }

Crypto::Hash CryptoNote::Transaction::hash() const {
  if (auto pruned = std::get_if<Crypto::Hash>(std::addressof(signatures))) {
    std::array<Crypto::Hash, 2> hashes{};
    hashes[0] = prefixHash();
    hashes[1] = *pruned;
    return Crypto::Hash::computeMerkleTree(hashes).takeOrThrow();
  } else if (auto raw = std::get_if<TransactionSignatureCollection>(std::addressof(signatures))) {
    if (raw->empty()) {
      return prefixHash();
    } else {
      std::array<Crypto::Hash, 2> hashes{};
      hashes[0] = prefixHash();
      hashes[1] = Crypto::Hash::computeObjectHash(*raw).takeOrThrow();
      return Crypto::Hash::computeMerkleTree(hashes).takeOrThrow();
    }
  } else {
    Xi::exceptional<Xi::InvalidVariantTypeError>("invalid variant type for transaction signatures");
  }
}

Crypto::Hash CryptoNote::TransactionPrefix::prefixHash() const {
  return Crypto::Hash::computeObjectHash<TransactionPrefix>(*this).takeOrThrow();
}

bool CryptoNote::Transaction::isNull() const { return version == 0; }

void CryptoNote::Transaction::nullify() {
  version = 0;
  unlockTime = 0;
  inputs.clear();
  outputs.clear();
  extra.clear();
  signatures = TransactionSignatureCollection{};
}

void CryptoNote::Transaction::prune() {
  if (auto pruned = std::get_if<Crypto::Hash>(std::addressof(signatures))) {
    XI_UNUSED(pruned);  // already pruned
  } else if (auto raw = std::get_if<TransactionSignatureCollection>(std::addressof(signatures))) {
    if (raw->empty()) {
      signatures = TransactionSignatureCollection{};
    } else {
      signatures = Crypto::Hash::computeObjectHash(*raw).takeOrThrow();
    }
  } else {
    Xi::exceptional<Xi::InvalidVariantTypeError>("invalid variant type for transaction signatures");
  }
}
