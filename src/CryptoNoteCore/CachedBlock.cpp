// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "CachedBlock.h"

#include <Xi/Config.h>
#include <Common/Varint.h>

#include "CryptoNoteTools.h"
#include "crypto/cnx/cnx.h"
#include "CryptoNoteCore/CryptoNoteSerialization.h"

using namespace Crypto;
using namespace CryptoNote;

CachedBlock::CachedBlock(const BlockTemplate& _block) : block(_block) {}

CachedBlock::CachedBlock(BlockTemplate&& _block) : block(std::move(_block)) {}

const BlockTemplate& CachedBlock::getBlock() const { return block; }

const Crypto::Hash& CachedBlock::getTransactionTreeHash() const {
  if (!transactionTreeHash.is_initialized()) {
    std::vector<Crypto::Hash> transactionHashes;
    transactionHashes.reserve(block.transactionHashes.size() + 1);
    transactionHashes.push_back(getObjectHash(block.baseTransaction));
    if (block.staticRewardHash) {
      transactionHashes.push_back(Crypto::Hash::compute(block.staticRewardHash->span()).takeOrThrow());
    }
    transactionHashes.insert(transactionHashes.end(), block.transactionHashes.begin(), block.transactionHashes.end());
    transactionTreeHash = Crypto::Hash();
    Crypto::Hash::computeMerkleTree(transactionHashes, *transactionTreeHash).throwOnError();
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
    Xi::Config::Hashes::compute(*this, blockLongHash.get(), block.version);
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
    size_t hardCodedTransactions = block.staticRewardHash ? 2 : 1;
    auto transactionCount =
        Common::asBinaryArray(Tools::get_varint_data(block.transactionHashes.size() + hardCodedTransactions));
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
      if (auto base = std::get_if<BaseInput>(&in)) {
        blockIndex = base->height.toIndex();
      } else {
        blockIndex = 0;
      }
    }
  }

  return blockIndex.get();
}

uint32_t CachedBlock::getNonceOffset() const { return 2; }

const CachedTransaction& CachedBlock::coinbase() const {
  if (!baseTransaction.is_initialized()) {
    baseTransaction = CachedTransaction{block.baseTransaction};
  }
  return *baseTransaction;
}

bool CachedBlock::hasStaticReward() const { return getBlock().staticRewardHash.has_value(); }

BlockHeight CachedBlock::height() const { return BlockHeight::fromIndex(getBlockIndex()); }
