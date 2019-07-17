// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "CachedBlock.h"

#include <cstring>

#include <Xi/Config.h>
#include <Xi/Exceptions.hpp>
#include <Common/Varint.h>

#include "CryptoNoteTools.h"
#include "CryptoNoteCore/CryptoNoteSerialization.h"

using namespace Crypto;
using namespace CryptoNote;

CachedBlock::CachedBlock(const BlockTemplate& _block) : block(_block) {
}

CachedBlock::CachedBlock(BlockTemplate&& _block) : block(std::move(_block)) {
}

const BlockTemplate& CachedBlock::getBlock() const {
  return block;
}

const BlockProofOfWork& CachedBlock::getProofOfWorkBlob() const {
  if (!blockProofOfWork.is_initialized()) {
    blockProofOfWork = BlockProofOfWork{};
    std::memcpy(blockProofOfWork->nonceData(), block.nonce.data(), BlockNonce::bytes());
    ::Crypto::Hash powHash;

    if (std::holds_alternative<NoMergeMiningTag>(block.mergeMiningTag)) {
      powHash = getBlockProofOfWorkHash();
    } else if (std::holds_alternative<MergeMiningTag>(block.mergeMiningTag)) {
      const auto& mmt = std::get<MergeMiningTag>(block.mergeMiningTag);
      ::Crypto::HashVector hashes{};
      hashes.reserve(mmt.prefix.size() + 1 + mmt.postfix.size());
      std::copy(mmt.prefix.begin(), mmt.prefix.end(), std::back_inserter(hashes));
      hashes.emplace_back(getBlockProofOfWorkHash());
      std::copy(mmt.postfix.begin(), mmt.postfix.end(), std::back_inserter(hashes));
      powHash = ::Crypto::Hash::computeMerkleTree(hashes).takeOrThrow();
    } else if (std::holds_alternative<PrunedMergeMiningTag>(block.mergeMiningTag)) {
      const auto& pmmt = std::get<PrunedMergeMiningTag>(block.mergeMiningTag);
      powHash = pmmt.proofOfWorkPrefix;
    } else {
      Xi::exceptional<Xi::InvalidVariantTypeError>("invalid merge mining tag variant");
    }

    std::memcpy(blockProofOfWork->hashData(), powHash.data(), BlockHash::bytes());
  }

  return blockProofOfWork.get();
}

const Crypto::Hash& CachedBlock::getTransactionTreeHash() const {
  if (!transactionTreeHash.is_initialized()) {
    std::vector<Crypto::Hash> transactionHashes;
    transactionHashes.reserve(block.transactionHashes.size() + 1);
    transactionHashes.push_back(block.baseTransaction.hash());
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
    std::array<Crypto::Hash, 2> hashes{};
    hashes[0] = block.headerHash();
    hashes[1] = getTransactionTreeHash();
    blockHash = Crypto::Hash::computeMerkleTree(hashes).takeOrThrow();
  }

  return blockHash.get();
}

const Hash& CachedBlock::getBlockProofOfWorkHash() const {
  if (!blockProofOfWorkHash.is_initialized()) {
    blockProofOfWorkHash = getBlock().proofOfWorkHash(getTransactionTreeHash());
  }

  return blockProofOfWorkHash.get();
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

uint32_t CachedBlock::getNonceOffset() const {
  return 0;
}

const CachedTransaction& CachedBlock::coinbase() const {
  if (!baseTransaction.is_initialized()) {
    baseTransaction = CachedTransaction{block.baseTransaction};
  }
  return *baseTransaction;
}

bool CachedBlock::hasStaticReward() const {
  return getBlock().staticRewardHash.has_value();
}

BlockHeight CachedBlock::height() const {
  return BlockHeight::fromIndex(getBlockIndex());
}

void CachedBlock::prune() {
  if (std::holds_alternative<NoMergeMiningTag>(block.mergeMiningTag)) {
    /* No need */
  } else if (std::holds_alternative<MergeMiningTag>(block.mergeMiningTag)) {
    const auto& mmt = std::get<MergeMiningTag>(block.mergeMiningTag);
    ::Crypto::HashVector hashes{};
    hashes.reserve(mmt.prefix.size() + 1 + mmt.postfix.size());
    std::copy(mmt.prefix.begin(), mmt.prefix.end(), std::back_inserter(hashes));
    hashes.emplace_back(getBlockProofOfWorkHash());
    std::copy(mmt.postfix.begin(), mmt.postfix.end(), std::back_inserter(hashes));
    const auto powHash = ::Crypto::Hash::computeMerkleTree(hashes).takeOrThrow();
    block.mergeMiningTag = PrunedMergeMiningTag{powHash, mmt.binarySize()};
  } else if (std::holds_alternative<PrunedMergeMiningTag>(block.mergeMiningTag)) {
    /* Already pruned */
  } else {
    Xi::exceptional<Xi::InvalidVariantTypeError>("invalid merge mining tag variant");
  }
}
