// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <cstdint>
#include <functional>
#include <system_error>
#include <vector>
#include <future>
#include <string>

#include <Xi/Result.h>

#include "crypto/crypto.h"
#include "CryptoNoteCore/CryptoNoteBasic.h"
#include "CryptoNoteCore/Currency.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"

#include "BlockchainExplorer/BlockchainExplorerData.h"
#include "CryptoNoteCore/Transactions/ITransactionBuilder.h"

namespace CryptoNote {

class INodeObserver {
 public:
  virtual ~INodeObserver() = default;
  virtual void peerCountUpdated(size_t count) {
    XI_UNUSED(count);
  }
  virtual void localBlockchainUpdated(BlockHeight height) {
    XI_UNUSED(height);
  }
  virtual void lastKnownBlockHeightUpdated(BlockHeight height) {
    XI_UNUSED(height);
  }
  virtual void poolChanged() {
  }
  virtual void blockchainSynchronized(BlockHeight topHeight) {
    XI_UNUSED(topHeight);
  }
  virtual void chainSwitched(BlockHeight newTopHeight, BlockHeight commonRoot,
                             const std::vector<Crypto::Hash>& hashes) {
    XI_UNUSED(newTopHeight, commonRoot, hashes);
  }
};

struct OutEntry {
  uint32_t outGlobalIndex;
  Crypto::PublicKey outKey;
};

struct OutsForAmount {
  uint64_t amount;
  std::vector<OutEntry> outs;
};

struct TransactionShortInfo {
  Crypto::Hash txId;
  TransactionPrefix txPrefix;
};

struct BlockShortEntry {
  Crypto::Hash blockHash;
  bool hasBlock;
  CryptoNote::BlockTemplate block;
  std::vector<TransactionShortInfo> txsShortInfo;
};

struct BlockHeaderInfo {
  BlockHeight height;
  BlockVersion version;
  BlockVersion upgradeVote;
  uint64_t timestamp;
  Crypto::Hash hash;
  Crypto::Hash prevHash;
  BlockNonce nonce;
  bool isAlternative;
  uint32_t depth;  // last block index = current block index + depth
  uint64_t difficulty;
  uint64_t reward;
};

class INode {
 public:
  typedef std::function<void(std::error_code)> Callback;

  virtual ~INode() {
  }
  virtual bool addObserver(INodeObserver* observer) = 0;
  virtual bool removeObserver(INodeObserver* observer) = 0;

  // precondition: must be called in dispatcher's thread
  virtual void init(const Callback& callback) = 0;
  // precondition: must be called in dispatcher's thread
  virtual bool shutdown() = 0;

  // precondition: all of following methods must not be invoked in dispatcher's thread
  virtual size_t getPeerCount() const = 0;
  virtual BlockHeight getLastLocalBlockHeight() const = 0;
  virtual BlockHeight getLastKnownBlockHeight() const = 0;
  virtual BlockVersion getLastKnownBlockVersion() const = 0;
  virtual uint32_t getLocalBlockCount() const = 0;
  virtual uint32_t getKnownBlockCount() const = 0;
  virtual uint64_t getLastLocalBlockTimestamp() const = 0;
  virtual BlockHeight getNodeHeight() const = 0;
  virtual void getFeeInfo() = 0;
  virtual const Currency& currency() const = 0;

  /*!
   * \brief ping sends a getFeeInfo request to the server checking if the connection can be established
   * \return and error code if not connected otherwise a ddefault initialized error code.
   */
  virtual std::error_code ping() = 0;

  virtual void getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount,
                                          std::vector<Crypto::Hash>& blockHashes, const Callback& callback) = 0;
  virtual void getTransactionHashesByPaymentId(const PaymentId& paymentId, std::vector<Crypto::Hash>& transactionHashes,
                                               const Callback& callback) = 0;

  virtual BlockHeaderInfo getLastLocalBlockHeaderInfo() const = 0;
  virtual void getLastBlockHeaderInfo(BlockHeaderInfo& info, const Callback& callback) = 0;

  virtual void relayTransaction(const Transaction& transaction, const Callback& callback) = 0;
  virtual void getRandomOutsByAmounts(
      std::map<uint64_t, uint64_t>&& amounts,
      std::vector<CryptoNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result,
      const Callback& callback) = 0;
  virtual void getNewBlocks(std::vector<Crypto::Hash>&& knownBlockIds, std::vector<RawBlock>& newBlocks,
                            BlockHeight& startHeight, const Callback& callback) = 0;
  virtual void getTransactionOutsGlobalIndices(const Crypto::Hash& transactionHash,
                                               std::vector<uint32_t>& outsGlobalIndices, const Callback& callback) = 0;
  virtual void queryBlocks(std::vector<Crypto::Hash>&& knownBlockIds, uint64_t timestamp,
                           std::vector<BlockShortEntry>& newBlocks, BlockHeight& startHeight,
                           const Callback& callback) = 0;
  virtual void getPoolSymmetricDifference(std::vector<Crypto::Hash>&& knownPoolTxIds, Crypto::Hash knownBlockId,
                                          bool& isBcActual, std::vector<std::unique_ptr<ITransactionReader>>& newTxs,
                                          std::vector<Crypto::Hash>& deletedTxIds, const Callback& callback) = 0;

  virtual void getBlocks(const std::vector<BlockHeight>& blockHeights, std::vector<std::vector<BlockDetails>>& blocks,
                         const Callback& callback) = 0;
  virtual void getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<BlockDetails>& blocks,
                         const Callback& callback) = 0;
  virtual void getRawBlocksByRange(BlockHeight height, uint32_t count, std::vector<RawBlock>& blocks,
                                   const Callback& callback) = 0;
  virtual void getBlock(const BlockHeight blockHeight, BlockDetails& block, const Callback& callback) = 0;
  virtual void getTransactions(const std::vector<Crypto::Hash>& transactionHashes,
                               std::vector<TransactionDetails>& transactions, const Callback& callback) = 0;
  virtual void isSynchronized(bool& syncStatus, const Callback& callback) = 0;
  virtual std::optional<FeeAddress> feeAddress() const = 0;

  // --------------------------------------- Convenient Boilerplate Code -------------------------------------------
 public:
  std::future<Xi::Result<void>> init();
  std::future<Xi::Result<BlockHeaderInfo>> getLastBlockHeaderInfo();
  std::future<Xi::Result<std::vector<RawBlock>>> getRawBlocksByRange(BlockHeight height, uint32_t count);
};

}  // namespace CryptoNote
