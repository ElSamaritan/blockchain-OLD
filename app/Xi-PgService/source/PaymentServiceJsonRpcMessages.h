// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <exception>
#include <limits>
#include <vector>
#include <string>

#include "Serialization/ISerializer.h"
#include <CryptoNoteCore/Blockchain/BlockHeight.hpp>

namespace PaymentService {

/* Forward declaration to avoid circular dependency from including "WalletService.h" */
class WalletService;

class RequestSerializationError : public std::exception {
 public:
  virtual const char* what() const noexcept override { return "Request error"; }
};

struct Save {
  struct Request {
    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct Export {
  struct Request {
    std::string fileName;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct Reset {
  struct Request {
    std::string viewSecretKey;

    CryptoNote::BlockHeight scanHeight = CryptoNote::BlockHeight::Null;

    bool newAddress = false;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct GetViewKey {
  struct Request {
    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::string viewSecretKey;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct GetMnemonicSeed {
  struct Request {
    std::string address;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::string mnemonicSeed;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct GetStatus {
  struct Request {
    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    CryptoNote::BlockHeight blockCount;
    CryptoNote::BlockHeight knownBlockCount;
    CryptoNote::BlockHeight localDaemonBlockCount;
    std::string lastBlockHash;
    uint32_t peerCount;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct GetAddresses {
  struct Request {
    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> addresses;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct CreateAddress {
  struct Request {
    std::string spendSecretKey;
    std::string spendPublicKey;

    CryptoNote::BlockHeight scanHeight = CryptoNote::BlockHeight::Null;

    bool newAddress = false;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::string address;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct CreateAddressList {
  struct Request {
    std::vector<std::string> spendSecretKeys;

    CryptoNote::BlockHeight scanHeight = CryptoNote::BlockHeight::Null;

    bool newAddress = false;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> addresses;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct DeleteAddress {
  struct Request {
    std::string address;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct GetSpendKeys {
  struct Request {
    std::string address;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::string spendSecretKey;
    std::string spendPublicKey;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct GetBalance {
  struct Request {
    std::string address;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    uint64_t availableBalance;
    uint64_t lockedAmount;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct GetBlockHashes {
  struct Request {
    CryptoNote::BlockHeight firstBlockHeight;
    uint32_t blockCount;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> blockHashes;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct TransactionHashesInBlockRpcInfo {
  std::string blockHash;
  std::vector<std::string> transactionHashes;

  bool serialize(CryptoNote::ISerializer& serializer);
};

struct GetTransactionHashes {
  struct Request {
    std::vector<std::string> addresses;
    CryptoNote::BlockHeight firstBlockHeight = CryptoNote::BlockHeight::Null;
    std::string blockHash;
    uint32_t blockCount;
    std::string paymentId;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<TransactionHashesInBlockRpcInfo> items;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct TransferRpcInfo {
  uint8_t type;
  std::string address;
  int64_t amount;

  bool serialize(CryptoNote::ISerializer& serializer);
};

struct TransactionRpcInfo {
  uint8_t state;
  std::string transactionHash;
  CryptoNote::BlockHeight blockHeight;
  uint64_t timestamp;
  bool isBase;
  uint64_t unlockTime;
  int64_t amount;
  uint64_t fee;
  std::vector<TransferRpcInfo> transfers;
  std::string extra;
  std::string paymentId;

  bool serialize(CryptoNote::ISerializer& serializer);
};

struct GetTransaction {
  struct Request {
    std::string transactionHash;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    TransactionRpcInfo transaction;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct TransactionsInBlockRpcInfo {
  std::string blockHash;
  std::vector<TransactionRpcInfo> transactions;

  bool serialize(CryptoNote::ISerializer& serializer);
};

struct GetTransactions {
  struct Request {
    std::vector<std::string> addresses;
    std::string blockHash;
    CryptoNote::BlockHeight firstBlockHeight = CryptoNote::BlockHeight::Null;
    uint32_t blockCount;
    std::string paymentId;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<TransactionsInBlockRpcInfo> items;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct GetUnconfirmedTransactionHashes {
  struct Request {
    std::vector<std::string> addresses;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> transactionHashes;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct WalletRpcOrder {
  std::string address;
  uint64_t amount;

  bool serialize(CryptoNote::ISerializer& serializer);
};

struct SendTransaction {
  struct Request {
    std::vector<std::string> sourceAddresses;
    std::vector<WalletRpcOrder> transfers;
    std::string changeAddress;
    uint64_t fee = 0;
    uint16_t anonymity;
    std::string extra;
    std::string paymentId;
    uint64_t unlockTime = 0;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::string transactionHash;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct CreateDelayedTransaction {
  struct Request {
    std::vector<std::string> addresses;
    std::vector<WalletRpcOrder> transfers;
    std::string changeAddress;
    uint64_t fee = 0;
    uint16_t anonymity;
    std::string extra;
    std::string paymentId;
    uint64_t unlockTime = 0;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::string transactionHash;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct GetDelayedTransactionHashes {
  struct Request {
    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::vector<std::string> transactionHashes;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct DeleteDelayedTransaction {
  struct Request {
    std::string transactionHash;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct SendDelayedTransaction {
  struct Request {
    std::string transactionHash;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct SendFusionTransaction {
  struct Request {
    uint64_t threshold;
    uint16_t anonymity;
    std::vector<std::string> addresses;
    std::string destinationAddress;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::string transactionHash;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct EstimateFusion {
  struct Request {
    uint64_t threshold;
    std::vector<std::string> addresses;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    uint32_t fusionReadyCount;
    uint32_t totalOutputCount;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct CreateIntegratedAddress {
  struct Request {
    std::string address;
    std::string paymentId;

    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::string integratedAddress;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

struct NodeFeeInfo {
  struct Request {
    bool serialize(CryptoNote::ISerializer& serializer);
  };

  struct Response {
    std::string address;
    uint64_t amount;

    bool serialize(CryptoNote::ISerializer& serializer);
  };
};

}  // namespace PaymentService
