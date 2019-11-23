// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.
#include "PaymentServiceJsonRpcMessages.h"

#include <Xi/Global.hh>

#include "Serialization/SerializationOverloads.h"
#include "WalletService.h"

namespace PaymentService {

bool Save::Request::serialize(CryptoNote::ISerializer& /*serializer*/) { return true; }

bool Save::Response::serialize(CryptoNote::ISerializer& /*serializer*/) { return true; }

bool Export::Request::serialize(CryptoNote::ISerializer& serializer) { return serializer(fileName, "file_name"); }

bool Export::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool Reset::Request::serialize(CryptoNote::ISerializer& serializer) {
  bool hasKey = serializer(viewSecretKey, "view_secret_key");

  bool hasScanHeight = serializer(scanHeight, "scan_height");
  bool hasNewAddress = serializer(newAddress, "new_address");

  /* Can't specify both that it is a new address, and a height to begin
     scanning from */
  if (hasNewAddress && hasScanHeight) {
    return false;
  }

  /* It's not a reset if you're not resetting :thinking: */
  if (!hasKey && hasNewAddress) {
    return false;
  }

  return true;
}

bool Reset::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool GetViewKey::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool GetViewKey::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(viewSecretKey, "view_secret_key"), false);
  return true;
}

bool GetMnemonicSeed::Request::serialize(CryptoNote::ISerializer& serializer) { return serializer(address, "address"); }

bool GetMnemonicSeed::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(mnemonicSeed, "mnemonic_seed");
}

bool GetStatus::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool GetStatus::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(blockCount, "block_count"), false);
  XI_RETURN_EC_IF_NOT(serializer(knownBlockCount, "known_block_count"), false);
  XI_RETURN_EC_IF_NOT(serializer(localDaemonBlockCount, "local_daemon_block_count"), false);
  XI_RETURN_EC_IF_NOT(serializer(lastBlockHash, "last_block_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(peerCount, "peer_count"), false);
  return true;
}

bool GetAddresses::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool GetAddresses::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(addresses, "addresses");
}

bool CreateAddress::Request::serialize(CryptoNote::ISerializer& serializer) {
  bool hasSecretKey = serializer(spendSecretKey, "spend_secret_key");
  bool hasPublicKey = serializer(spendPublicKey, "spend_public_key");

  bool hasNewAddress = serializer(newAddress, "new_address");
  bool hasScanHeight = serializer(scanHeight, "scan_height");

  if (hasSecretKey && hasPublicKey) {
    // TODO: replace it with error codes
    return false;
  }

  /* Can't specify both that it is a new address, and a height to begin
     scanning from */
  if (hasNewAddress && hasScanHeight) {
    return false;
  }
  return true;
}

bool CreateAddress::Response::serialize(CryptoNote::ISerializer& serializer) { return serializer(address, "address"); }

bool CreateAddressList::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(spendSecretKeys, "spend_secret_keys"), false);
  XI_RETURN_EC_IF_NOT(serializer(newAddress, "new_address"), false);
  XI_RETURN_EC_IF_NOT(serializer(scanHeight, "scan_height"), false);
  return true;
}

bool CreateAddressList::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(addresses, "addresses");
}

bool DeleteAddress::Request::serialize(CryptoNote::ISerializer& serializer) { return serializer(address, "address"); }

bool DeleteAddress::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool GetSpendKeys::Request::serialize(CryptoNote::ISerializer& serializer) { return serializer(address, "address"); }

bool GetSpendKeys::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(spendSecretKey, "spend_secret_key"), false);
  XI_RETURN_EC_IF_NOT(serializer(spendPublicKey, "spend_public_keys"), false);
  return true;
}

bool GetBalance::Request::serialize(CryptoNote::ISerializer& serializer) { return serializer(address, "address"); }

bool GetBalance::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(availableBalance, "available_balance"), false);
  XI_RETURN_EC_IF_NOT(serializer(lockedAmount, "locked_amount"), false);
  return true;
}

bool GetBlockHashes::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(firstBlockHeight, "first_block_height"), false);
  XI_RETURN_EC_IF_NOT(serializer(blockCount, "block_count"), false);
  return true;
}

bool GetBlockHashes::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(blockHashes, "block_hashes");
}

bool TransactionHashesInBlockRpcInfo::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(blockHash, "block_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(transactionHashes, "transaction_hashes"), false);
  return true;
}

bool GetTransactionHashes::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(addresses, "addresses"), false);
  XI_RETURN_EC_IF_NOT(serializer(firstBlockHeight, "first_block_height"), false);
  XI_RETURN_EC_IF_NOT(serializer(blockHash, "block_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(blockCount, "block_count"), false);
  XI_RETURN_EC_IF_NOT(serializer(paymentId, "payment_id"), false);
  return true;
}

bool GetTransactionHashes::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(items, "items");
}

bool TransferRpcInfo::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(type, "type"), false);
  XI_RETURN_EC_IF_NOT(serializer(address, "address"), false);
  XI_RETURN_EC_IF_NOT(serializer(amount, "amount"), false);
  return true;
}

bool TransactionRpcInfo::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(state, "state"), false);
  XI_RETURN_EC_IF_NOT(serializer(transactionHash, "transaction_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(blockHeight, "block_height"), false);
  XI_RETURN_EC_IF_NOT(serializer(timestamp, "timestamp"), false);
  XI_RETURN_EC_IF_NOT(serializer(isBase, "is_base"), false);
  XI_RETURN_EC_IF_NOT(serializer(unlockTime, "unlock_time"), false);
  XI_RETURN_EC_IF_NOT(serializer(amount, "amount"), false);
  XI_RETURN_EC_IF_NOT(serializer(fee, "fee"), false);
  XI_RETURN_EC_IF_NOT(serializer(transfers, "transfers"), false);
  XI_RETURN_EC_IF_NOT(serializer(extra, "extra"), false);
  return true;
}

bool GetTransaction::Request::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(transactionHash, "transaction_hash");
}

bool GetTransaction::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(transaction, "transaction");
}

bool GetTransactionsCount::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool GetTransactionsCount::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(count, "count"), false);
  return true;
}

bool GetTransactionHashesByRange::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(offset, "offset"), false);
  XI_RETURN_EC_IF_NOT(serializer(count, "count"), false);
  return true;
}

bool GetTransactionHashesByRange::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(hashes, "hashes"), false);
  return true;
}

bool GetTransactionsByHash::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(hashes, "hashes"), false);
  return true;
}

bool GetTransactionsByHash::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(transactions, "transactions"), false);
  return true;
}

bool TransactionsInBlockRpcInfo::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(blockHash, "block_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(transactions, "transactions"), false);
  return true;
}

bool GetTransactions::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(addresses, "addresses"), false);
  XI_RETURN_EC_IF_NOT(serializer(blockHash, "block_hash"), false);
  XI_RETURN_EC_IF_NOT(serializer(firstBlockHeight, "first_block_height"), false);
  XI_RETURN_EC_IF_NOT(serializer(blockCount, "block_count"), false);
  XI_RETURN_EC_IF_NOT(serializer(paymentId, "payment_id"), false);
  return true;
}

bool GetTransactions::Response::serialize(CryptoNote::ISerializer& serializer) { return serializer(items, "items"); }

bool GetUnconfirmedTransactionHashes::Request::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(addresses, "addresses");
}

bool GetUnconfirmedTransactionHashes::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(transactionHashes, "transaction_hashes");
}

bool WalletRpcOrder::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(address, "address"), false);
  XI_RETURN_EC_IF_NOT(serializer(amount, "amount"), false);
  return true;
}

bool SendTransaction::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(sourceAddresses, "addresses"), false);
  XI_RETURN_EC_IF_NOT(serializer(transfers, "transfers"), false);
  XI_RETURN_EC_IF_NOT(serializer(changeAddress, "change_address"), false);
  XI_RETURN_EC_IF_NOT(serializer(fee, "fee"), false);
  XI_RETURN_EC_IF_NOT(serializer(paymentId, "payment_id"), false);
  XI_RETURN_EC_IF_NOT(serializer(unlockTime, "unlock_time"), false);
  return true;
}

bool SendTransaction::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(transactionHash, "transaction_hash");
}

bool CreateDelayedTransaction::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(addresses, "addresses"), false);
  XI_RETURN_EC_IF_NOT(serializer(transfers, "transfers"), false);
  XI_RETURN_EC_IF_NOT(serializer(changeAddress, "change_address"), false);
  XI_RETURN_EC_IF_NOT(serializer(fee, "fee"), false);
  XI_RETURN_EC_IF_NOT(serializer(paymentId, "payment_id"), false);
  XI_RETURN_EC_IF_NOT(serializer(unlockTime, "unlock_time"), false);
  return true;
}

bool CreateDelayedTransaction::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(transactionHash, "transaction_hash");
}

bool GetDelayedTransactionHashes::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool GetDelayedTransactionHashes::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(transactionHashes, "transaction_hashes");
}

bool DeleteDelayedTransaction::Request::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(transactionHash, "transaction_hash");
}

bool DeleteDelayedTransaction::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool SendDelayedTransaction::Request::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(transactionHash, "transaction_hash");
}

bool SendDelayedTransaction::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool SendFusionTransaction::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(threshold, "threshold"), false);
  XI_RETURN_EC_IF_NOT(serializer(addresses, "addresses"), false);
  XI_RETURN_EC_IF_NOT(serializer(destinationAddress, "destination_address"), false);
  return true;
}

bool SendFusionTransaction::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(transactionHash, "transaction_hash");
}

bool EstimateFusion::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(threshold, "threshold"), false);
  XI_RETURN_EC_IF_NOT(serializer(addresses, "addresses"), false);
  return true;
}

bool EstimateFusion::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(fusionReadyCount, "fusion_ready_count"), false);
  XI_RETURN_EC_IF_NOT(serializer(totalOutputCount, "total_output_count"), false);
  return true;
}

bool CreateIntegratedAddress::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(address, "address"), false);
  XI_RETURN_EC_IF_NOT(serializer(paymentId, "payment_id"), false);
  return true;
}

bool CreateIntegratedAddress::Response::serialize(CryptoNote::ISerializer& serializer) {
  return serializer(integratedAddress, "integrated_address");
}

bool NodeFeeInfo::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool NodeFeeInfo::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(address, "address"), false);
  XI_RETURN_EC_IF_NOT(serializer(amount, "amount"), false);
  return true;
}

bool Shutdown::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool Shutdown::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_UNUSED(serializer);
  return true;
}

bool GeneratePaymentId::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(seed, "seed"), false);
  return true;
}

bool GeneratePaymentId::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(paymentId, "payment_id"), false);
  XI_RETURN_EC_IF_NOT(serializer(secretKey, "secretKey"), false);
  return true;
}

bool ValidatePaymentId::Request::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(value, "value"), false);
  return true;
}

bool ValidatePaymentId::Response::serialize(CryptoNote::ISerializer& serializer) {
  XI_RETURN_EC_IF_NOT(serializer(isValid, "is_valid"), false);
  XI_RETURN_EC_IF_NOT(serializer(errorMessage, "error_message"), false);
  return true;
}

}  // namespace PaymentService
