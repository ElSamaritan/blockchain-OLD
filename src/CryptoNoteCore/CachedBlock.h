﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <boost/optional.hpp>
#include <CryptoNoteCore/CryptoNote.h>

#include "CryptoNoteCore/Transactions/CachedTransaction.h"

namespace CryptoNote {

class CachedBlock {
 public:
  explicit CachedBlock(const BlockTemplate& block);
  explicit CachedBlock(BlockTemplate&& block);
  const BlockTemplate& getBlock() const;
  const BlockProofOfWork& getProofOfWorkBlob() const;
  const Crypto::Hash& getTransactionTreeHash() const;
  const Crypto::Hash& getBlockHash() const;
  const Crypto::Hash& getBlockProofOfWorkHash() const;
  uint32_t getBlockIndex() const;
  uint32_t getNonceOffset() const;

  const CachedTransaction& coinbase() const;
  bool hasStaticReward() const;
  BlockHeight height() const;

  void prune();

 private:
  BlockTemplate block;
  mutable boost::optional<uint32_t> blockIndex;
  mutable boost::optional<Crypto::Hash> transactionTreeHash;
  mutable boost::optional<Crypto::Hash> blockHash;
  mutable boost::optional<Crypto::Hash> blockProofOfWorkHash;
  mutable boost::optional<BlockProofOfWork> blockProofOfWork;
  mutable boost::optional<CachedTransaction> baseTransaction;
};

}  // namespace CryptoNote
