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

#include <vector>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Xi/Algorithm/GenericComparison.h>
#include <Xi/Algorithm/GenericHash.h>
#include <crypto/CryptoTypes.h>

#include "CryptoNoteCore/Transactions/Transaction.h"

namespace CryptoNote {

struct BlockHeader {
  uint8_t majorVersion;
  uint8_t minorVersion;
  uint32_t nonce;
  uint64_t timestamp;
  Crypto::Hash previousBlockHash;
};

struct BlockTemplate : public BlockHeader {
  Transaction baseTransaction;
  Transaction staticReward;  ///< Gets only serialized if a static reward is expected and set (!isNull).
  std::vector<Crypto::Hash> transactionHashes;
};

struct AccountPublicAddress {
  Crypto::PublicKey spendPublicKey;
  Crypto::PublicKey viewPublicKey;
};

XI_MAKE_GENERIC_COMPARISON(AccountPublicAddress)
XI_MAKE_GENERIC_HASH_FUNC(CryptoNote::AccountPublicAddress)

struct AccountKeys {
  AccountPublicAddress address;
  Crypto::SecretKey spendSecretKey;
  Crypto::SecretKey viewSecretKey;
};

struct KeyPair {
  Crypto::PublicKey publicKey;
  Crypto::SecretKey secretKey;
};

using BinaryArray = std::vector<uint8_t>;

struct RawBlock {
  BinaryArray block;  // BlockTemplate
  std::vector<BinaryArray> transactions;
};

struct LiteBlock {
  BinaryArray blockTemplate;
};

}  // namespace CryptoNote

XI_MAKE_GENERIC_HASH_OVERLOAD(CryptoNote, AccountPublicAddress)
