// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <map>
#include <vector>
#include <cinttypes>

#include <crypto/CryptoTypes.h>
#include <Serialization/ISerializer.h>

namespace CryptoNote {

union PackedOutIndex {
  struct {
    uint32_t blockIndex;
    uint16_t transactionIndex;
    uint16_t outputIndex;
  } data;

  uint64_t packedValue;
};

struct KeyOutputInfo {
  Crypto::PublicKey publicKey;
  Crypto::Hash transactionHash;
  uint64_t unlockTime;
  PackedOutIndex index;

  [[nodiscard]] bool serialize(CryptoNote::ISerializer& s);
};

using KeyOutputInfoVector = std::vector<KeyOutputInfo>;

}  // namespace CryptoNote
