// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "NodeFactory.h"

#include <memory>
#include <future>

#include <Xi/Global.hh>
#include <Xi/Exceptions.hpp>

#include "NodeRpcProxy/NodeRpcProxy.h"
#include "NodeRpcProxy/NodeErrors.h"

namespace PaymentService {

class NodeRpcStub final : public CryptoNote::INode {
 public:
  ~NodeRpcStub() override {}
  bool addObserver(CryptoNote::INodeObserver* observer) override {
    XI_UNUSED(observer);
    return true;
  }
  bool removeObserver(CryptoNote::INodeObserver* observer) override {
    XI_UNUSED(observer);
    return true;
  }

  void init(const Callback& callback) override { XI_UNUSED(callback); }
  bool shutdown() override { return true; }

  size_t getPeerCount() const override { return 0; }
  CryptoNote::BlockVersion getLastKnownBlockVersion() const override { return CryptoNote::BlockVersion::Null; }
  CryptoNote::BlockHeight getLastLocalBlockHeight() const override { return CryptoNote::BlockHeight::Null; }
  CryptoNote::BlockHeight getLastKnownBlockHeight() const override { return CryptoNote::BlockHeight::Null; }
  uint32_t getLocalBlockCount() const override { return 0; }
  uint32_t getKnownBlockCount() const override { return 0; }
  uint64_t getLastLocalBlockTimestamp() const override { return 0; }
  CryptoNote::BlockHeight getNodeHeight() const override { return CryptoNote::BlockHeight::Null; }

  void getFeeInfo() override {}
  const CryptoNote::Currency& currency() const override { Xi::exceptional<Xi::NotInitializedError>(); }
  std::error_code ping() override { return std::error_code{}; }

  void getLastBlockHeaderInfo(CryptoNote::BlockHeaderInfo& info, const Callback& callback) override {
    XI_UNUSED(info);
    callback(std::error_code());
  }

  void getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount, std::vector<Crypto::Hash>& blockHashes,
                                  const Callback& callback) override {
    XI_UNUSED(timestampBegin, secondsCount, blockHashes);
    callback(std::error_code());
  }

  void getTransactionHashesByPaymentId(const CryptoNote::PaymentId& paymentId,
                                       std::vector<Crypto::Hash>& transactionHashes,
                                       const Callback& callback) override {
    XI_UNUSED(paymentId, transactionHashes);
    callback(std::error_code());
  }

  CryptoNote::BlockHeaderInfo getLastLocalBlockHeaderInfo() const override { return CryptoNote::BlockHeaderInfo(); }

  void relayTransaction(const CryptoNote::Transaction& transaction, const Callback& callback) override {
    XI_UNUSED(transaction);
    callback(std::error_code());
  }
  void getRandomOutsByAmounts(
      std::map<uint64_t, uint64_t>&& amounts,
      std::vector<CryptoNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result,
      const Callback& callback) override {
    XI_UNUSED(amounts, result, callback);
  }
  virtual void getRequiredMixinByAmounts(std::set<uint64_t>&& amounts, std::map<uint64_t, uint64_t>& out,
                                         const Callback& callback) override {
    XI_UNUSED(amounts, out, callback);
  }

  void getNewBlocks(std::vector<Crypto::Hash>&& knownBlockIds, std::vector<CryptoNote::RawBlock>& newBlocks,
                    CryptoNote::BlockHeight& startHeight, const Callback& callback) override {
    XI_UNUSED(knownBlockIds);
    XI_UNUSED(newBlocks);
    startHeight = CryptoNote::BlockHeight::Null;
    callback(std::error_code());
  }
  void getTransactionOutsGlobalIndices(const Crypto::Hash& transactionHash, std::vector<uint32_t>& outsGlobalIndices,
                                       const Callback& callback) override {
    XI_UNUSED(transactionHash, outsGlobalIndices, callback);
  }

  void queryBlocks(std::vector<Crypto::Hash>&& knownBlockIds, uint64_t timestamp,
                   std::vector<CryptoNote::BlockShortEntry>& newBlocks, CryptoNote::BlockHeight& startHeight,
                   const Callback& callback) override {
    XI_UNUSED(knownBlockIds, timestamp, newBlocks);
    startHeight = CryptoNote::BlockHeight::Null;
    callback(std::error_code());
  }

  void getPoolSymmetricDifference(std::vector<Crypto::Hash>&& knownPoolTxIds, Crypto::Hash knownBlockId,
                                  bool& isBcActual,
                                  std::vector<std::unique_ptr<CryptoNote::ITransactionReader>>& newTxs,
                                  std::vector<Crypto::Hash>& deletedTxIds, const Callback& callback) override {
    XI_UNUSED(knownPoolTxIds, knownBlockId, newTxs, deletedTxIds);
    isBcActual = true;
    callback(std::error_code());
  }

  void getBlocks(const std::vector<CryptoNote::BlockHeight>& blockHeights,
                 std::vector<std::vector<CryptoNote::BlockDetails>>& blocks, const Callback& callback) override {
    XI_UNUSED(blockHeights, blocks, callback);
  }

  void getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<CryptoNote::BlockDetails>& blocks,
                 const Callback& callback) override {
    XI_UNUSED(blockHashes, blocks, callback);
  }

  void getBlock(const CryptoNote::BlockHeight blockHeight, CryptoNote::BlockDetails& block,
                const Callback& callback) override {
    XI_UNUSED(blockHeight, block, callback);
  }

  void getRawBlocksByRange(CryptoNote::BlockHeight height, uint32_t count, std::vector<CryptoNote::RawBlock>& blocks,
                           const Callback& callback) override {
    XI_UNUSED(height, count, blocks, callback);
  }

  void getTransactions(const std::vector<Crypto::Hash>& transactionHashes,
                       std::vector<CryptoNote::TransactionDetails>& transactions, const Callback& callback) override {
    XI_UNUSED(transactionHashes, transactions, callback);
  }

  void isSynchronized(bool& syncStatus, const Callback& callback) override { XI_UNUSED(syncStatus, callback); }
  std::optional<CryptoNote::FeeAddress> feeAddress() const override { return std::nullopt; }
};

class NodeInitObserver {
 public:
  NodeInitObserver() { initFuture = initPromise.get_future(); }

  void initCompleted(std::error_code result) { initPromise.set_value(result); }

  void waitForInitEnd() {
    std::error_code ec = initFuture.get();
    if (ec) {
      throw std::system_error(ec);
    }
    return;
  }

 private:
  std::promise<std::error_code> initPromise;
  std::future<std::error_code> initFuture;
};

NodeFactory::NodeFactory() {}

NodeFactory::~NodeFactory() {}

CryptoNote::INode* NodeFactory::createNode(const std::string& daemonAddress, uint16_t daemonPort,
                                           Xi::Http::SSLConfiguration sslConfig, const CryptoNote::Currency& currency,
                                           Logging::ILogger& logger) {
  std::unique_ptr<CryptoNote::INode> node(
      new CryptoNote::NodeRpcProxy(daemonAddress, daemonPort, sslConfig, currency, logger));

  NodeInitObserver initObserver;
  node->init(std::bind(&NodeInitObserver::initCompleted, &initObserver, std::placeholders::_1));
  initObserver.waitForInitEnd();

  return node.release();
}

CryptoNote::INode* NodeFactory::createNodeStub() { return new NodeRpcStub(); }

}  // namespace PaymentService
