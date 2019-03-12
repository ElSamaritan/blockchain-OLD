// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "NodeFactory.h"

#include <memory>
#include <future>

#include <Xi/Global.h>

#include "NodeRpcProxy/NodeRpcProxy.h"

namespace PaymentService {

class NodeRpcStub : public CryptoNote::INode {
 public:
  virtual ~NodeRpcStub() override {}
  virtual bool addObserver(CryptoNote::INodeObserver* observer) override {
    XI_UNUSED(observer);
    return true;
  }
  virtual bool removeObserver(CryptoNote::INodeObserver* observer) override {
    XI_UNUSED(observer);
    return true;
  }

  virtual void init(const Callback& callback) override { XI_UNUSED(callback); }
  virtual bool shutdown() override { return true; }

  virtual size_t getPeerCount() const override { return 0; }
  virtual uint32_t getLastLocalBlockHeight() const override { return 0; }
  virtual uint32_t getLastKnownBlockHeight() const override { return 0; }
  virtual uint32_t getLocalBlockCount() const override { return 0; }
  virtual uint32_t getKnownBlockCount() const override { return 0; }
  virtual uint64_t getLastLocalBlockTimestamp() const override { return 0; }
  virtual uint64_t getNodeHeight() const override { return 0; }

  virtual std::string getInfo() override { return std::string(); }
  virtual void getFeeInfo() override {}
  virtual bool ping() override { return false; }

  virtual void getLastBlockHeaderInfo(CryptoNote::BlockHeaderInfo& info, const Callback& callback) override {
    XI_UNUSED(info);
    callback(std::error_code());
  }

  virtual void getBlockHashesByTimestamps(uint64_t timestampBegin, size_t secondsCount,
                                          std::vector<Crypto::Hash>& blockHashes, const Callback& callback) override {
    XI_UNUSED(timestampBegin, secondsCount, blockHashes);
    callback(std::error_code());
  }

  virtual void getTransactionHashesByPaymentId(const Crypto::Hash& paymentId,
                                               std::vector<Crypto::Hash>& transactionHashes,
                                               const Callback& callback) override {
    XI_UNUSED(paymentId, transactionHashes);
    callback(std::error_code());
  }

  virtual CryptoNote::BlockHeaderInfo getLastLocalBlockHeaderInfo() const override {
    return CryptoNote::BlockHeaderInfo();
  }

  virtual void relayTransaction(const CryptoNote::Transaction& transaction, const Callback& callback) override {
    XI_UNUSED(transaction);
    callback(std::error_code());
  }
  virtual void getRandomOutsByAmounts(
      std::vector<uint64_t>&& amounts, uint16_t outsCount,
      std::vector<CryptoNote::COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount>& result,
      const Callback& callback) override {
    XI_UNUSED(amounts, outsCount, result, callback);
  }
  virtual void getNewBlocks(std::vector<Crypto::Hash>&& knownBlockIds, std::vector<CryptoNote::RawBlock>& newBlocks,
                            uint32_t& startHeight, const Callback& callback) override {
    XI_UNUSED(knownBlockIds);
    XI_UNUSED(newBlocks);
    startHeight = 0;
    callback(std::error_code());
  }
  virtual void getTransactionOutsGlobalIndices(const Crypto::Hash& transactionHash,
                                               std::vector<uint32_t>& outsGlobalIndices,
                                               const Callback& callback) override {
    XI_UNUSED(transactionHash, outsGlobalIndices, callback);
  }

  virtual void queryBlocks(std::vector<Crypto::Hash>&& knownBlockIds, uint64_t timestamp,
                           std::vector<CryptoNote::BlockShortEntry>& newBlocks, uint32_t& startHeight,
                           const Callback& callback) override {
    XI_UNUSED(knownBlockIds, timestamp, newBlocks);
    startHeight = 0;
    callback(std::error_code());
  }

  virtual void getPoolSymmetricDifference(std::vector<Crypto::Hash>&& knownPoolTxIds, Crypto::Hash knownBlockId,
                                          bool& isBcActual,
                                          std::vector<std::unique_ptr<CryptoNote::ITransactionReader>>& newTxs,
                                          std::vector<Crypto::Hash>& deletedTxIds, const Callback& callback) override {
    XI_UNUSED(knownPoolTxIds, knownBlockId, newTxs, deletedTxIds);
    isBcActual = true;
    callback(std::error_code());
  }

  virtual void getBlocks(const std::vector<uint32_t>& blockHeights,
                         std::vector<std::vector<CryptoNote::BlockDetails>>& blocks,
                         const Callback& callback) override {
    XI_UNUSED(blockHeights, blocks, callback);
  }

  virtual void getBlocks(const std::vector<Crypto::Hash>& blockHashes, std::vector<CryptoNote::BlockDetails>& blocks,
                         const Callback& callback) override {
    XI_UNUSED(blockHashes, blocks, callback);
  }

  virtual void getBlock(const uint32_t blockHeight, CryptoNote::BlockDetails& block,
                        const Callback& callback) override {
    XI_UNUSED(blockHeight, block, callback);
  }

  virtual void getRawBlocksByRange(uint32_t height, uint32_t count, std::vector<CryptoNote::RawBlock>& blocks,
                                   const Callback& callback) override {
    XI_UNUSED(height, count, blocks, callback);
  }

  virtual void getTransactions(const std::vector<Crypto::Hash>& transactionHashes,
                               std::vector<CryptoNote::TransactionDetails>& transactions,
                               const Callback& callback) override {
    XI_UNUSED(transactionHashes, transactions, callback);
  }

  virtual void isSynchronized(bool& syncStatus, const Callback& callback) override { XI_UNUSED(syncStatus, callback); }
  virtual std::string feeAddress() override { return std::string(); }
  virtual uint32_t feeAmount() override { return 0; }
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
                                           Xi::Http::SSLConfiguration sslConfig, Logging::ILogger& logger) {
  std::unique_ptr<CryptoNote::INode> node(new CryptoNote::NodeRpcProxy(daemonAddress, daemonPort, sslConfig, logger));

  NodeInitObserver initObserver;
  node->init(std::bind(&NodeInitObserver::initCompleted, &initObserver, std::placeholders::_1));
  initObserver.waitForInitEnd();

  return node.release();
}

CryptoNote::INode* NodeFactory::createNodeStub() { return new NodeRpcStub(); }

}  // namespace PaymentService
