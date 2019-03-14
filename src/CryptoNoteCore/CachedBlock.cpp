// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "CachedBlock.h"
#include <Common/Varint.h>
#include <Xi/Config.h>
#include "CryptoNoteTools.h"
#include "crypto/cnx/cnx.h"

using namespace Crypto;
using namespace CryptoNote;

CachedBlock::CachedBlock(const BlockTemplate& block) : block(block) {}

const BlockTemplate& CachedBlock::getBlock() const { return block; }

const Crypto::Hash& CachedBlock::getTransactionTreeHash() const {
  if (!transactionTreeHash.is_initialized()) {
    std::vector<Crypto::Hash> transactionHashes;
    transactionHashes.reserve(block.transactionHashes.size() + 1);
    transactionHashes.push_back(getObjectHash(block.baseTransaction));
    if (!block.staticReward.isNull()) {
      transactionHashes.push_back(getObjectHash(block.staticReward));
    }
    transactionHashes.insert(transactionHashes.end(), block.transactionHashes.begin(), block.transactionHashes.end());
    transactionTreeHash = Crypto::Hash();
    Crypto::tree_hash(transactionHashes.data(), transactionHashes.size(), transactionTreeHash.get());
  }

  return transactionTreeHash.get();
}

const Crypto::Hash& CachedBlock::getBlockHash() const {
  if (!blockHash.is_initialized()) {
    BinaryArray blockBinaryArray = getBlockHashingBinaryArray();
    blockHash = getObjectHash(blockBinaryArray);
  }

  return blockHash.get();
}

const Crypto::Hash& CachedBlock::getBlockLongHash() const {
  if (!blockLongHash.is_initialized()) {
    blockLongHash = Hash{};
    Xi::Config::Hashes::compute(*this, blockLongHash.get(), block.majorVersion);
  }

  return blockLongHash.get();
}

const BinaryArray& CachedBlock::getBlockHashingBinaryArray() const {
  if (!blockHashingBinaryArray.is_initialized()) {
    blockHashingBinaryArray = BinaryArray();
    auto& result = blockHashingBinaryArray.get();
    if (!toBinaryArray(static_cast<const BlockHeader&>(block), result)) {
      blockHashingBinaryArray.reset();
      throw std::runtime_error("Can't serialize BlockHeader");
    }

    const auto& treeHash = getTransactionTreeHash();
    result.insert(result.end(), treeHash.begin(), treeHash.end());
    auto transactionCount = Common::asBinaryArray(
        Tools::get_varint_data(block.transactionHashes.size() + (block.staticReward.isNull() ? 1 : 2)));
    result.insert(result.end(), transactionCount.begin(), transactionCount.end());
  }

  return blockHashingBinaryArray.get();
}

uint32_t CachedBlock::getBlockIndex() const {
  if (!blockIndex.is_initialized()) {
    if (block.baseTransaction.inputs.size() != 1) {
      blockIndex = 0;
    } else {
      const auto& in = block.baseTransaction.inputs[0];
      if (in.type() != typeid(BaseInput)) {
        blockIndex = 0;
      } else {
        blockIndex = boost::get<BaseInput>(in).blockIndex;
      }
    }
  }

  return blockIndex.get();
}

uint32_t CachedBlock::getNonceOffset() const {
  if (!nonceOffset.is_initialized()) {
    BinaryArray result;
    toBinaryArray(static_cast<const BlockHeader&>(block), result);
    nonceOffset = result.size() - sizeof(block.nonce);
  }
  return nonceOffset.get();
}

bool CachedBlock::hasStaticReward() const { return !block.staticReward.isNull(); }
