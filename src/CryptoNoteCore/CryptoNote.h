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
#include <optional>
#include <variant>

#include <Xi/ExternalIncludePush.h>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Blockchain/Block/Header.hpp>
#include <Xi/Blockchain/Block/Version.hpp>
#include <Xi/Blockchain/Block/Height.hpp>

#include <Xi/Algorithm/GenericComparison.h>
#include <Xi/Algorithm/GenericHash.h>
#include <Serialization/FlagSerialization.hpp>
#include <crypto/CryptoTypes.h>

#include "CryptoNoteCore/Transactions/Transaction.h"

namespace CryptoNote {

using BlockVersion = Xi::Blockchain::Block::Version;
using BlockHeight = Xi::Blockchain::Block::Height;
using BlockOffset = Xi::Blockchain::Block::Offset;
using BlockNonce = Xi::Blockchain::Block::Nonce;
using BlockHeader = Xi::Blockchain::Block::Header;

struct BlockTemplate : public BlockHeader {
  Transaction baseTransaction;
  std::optional<Crypto::Hash> staticRewardHash;
  std::vector<Crypto::Hash> transactionHashes;
};

struct AccountPublicAddress {
  Crypto::PublicKey spendPublicKey;
  Crypto::PublicKey viewPublicKey;

  bool isValid() const { return spendPublicKey.isValid() && viewPublicKey.isValid(); }
};

struct FeeAddress {
  AccountPublicAddress address;
  uint64_t amount;
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

/*!
 * \brief The FullBlock struct is a type safe alternative to RawBlock where members are serialized by their type rather
 * than being emplaced as already serialized in binary form.
 *
 * \attention If you do not trust the source of a full block you may want to check the integrity of the block. The block
 * template transactions hashes MUST correspond to the transactions hashes of transactions emplaced, and their ordering
 * MUST be the same.
 *
 * \example
 * \code{.cpp}
 * if(blockTemplate.transactionHashes.size() != transactions.size()) {
 *  return false;
 * }
 *
 * for(size_t i = 0; i < transactions.size(); ++i) {
 *  if(blockTemplate.transactionHashes[i] != sha3_256(toBinary(transactions[i])) {
 *    return false;
 * }
 * \endcode
 */
struct FullBlock {
  BlockTemplate blockTemplate;
  std::vector<Transaction> transactions;
};

struct LiteBlock {
  BinaryArray blockTemplate;
};

}  // namespace CryptoNote

XI_MAKE_GENERIC_HASH_OVERLOAD(CryptoNote, AccountPublicAddress)
