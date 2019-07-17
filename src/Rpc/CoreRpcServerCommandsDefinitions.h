// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Karai Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <vector>
#include <string>
#include <cinttypes>

#include <Xi/Global.hh>

#include "CryptoNoteCore/CryptoNoteBasic.h"
#include "CryptoNoteCore/Difficulty.h"

#include "BlockchainExplorer/BlockchainExplorerData.h"
#include "BlockchainExplorer/BlockchainExplorerDataSerialization.h"

#include "Serialization/SerializationOverloads.h"
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>
#include <Serialization/OptionalSerialization.hpp>
#include <CryptoNoteCore/ICoreDefinitions.h>
#include <CryptoNoteCore/CryptoNoteSerialization.h>
#include <P2p/P2pProtocolTypes.h>

namespace CryptoNote {
//-----------------------------------------------
#define CORE_RPC_STATUS_OK "OK"
#define CORE_RPC_STATUS_BUSY "BUSY"

struct STATUS_STRUCT {
  std::string status;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(status)
  KV_END_SERIALIZATION
};

struct COMMAND_RPC_GET_HEIGHT {
  typedef Null request;

  struct response {
    BlockHeight height;
    BlockHeight network_height;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(height)
    KV_MEMBER(network_height)
    KV_MEMBER(status)
    KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_BLOCKS_FAST {
  struct request {
    std::vector<Crypto::Hash>
        block_hashes;  //*first 10 blocks id goes sequential, next goes in pow(2,n) offset, like 2,
                       // 4, 8, 16, 32, 64 and so on, and the last one is always genesis block */

    KV_BEGIN_SERIALIZATION KV_MEMBER(block_hashes);
    KV_END_SERIALIZATION
  };

  struct response {
    std::vector<RawBlock> blocks;
    BlockHeight start_height;
    BlockHeight current_height;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(blocks)
    KV_MEMBER(start_height)
    KV_MEMBER(current_height)
    KV_MEMBER(status)
    KV_END_SERIALIZATION
  };
};
//-----------------------------------------------
struct COMMAND_RPC_GET_TRANSACTIONS {
  struct request {
    std::vector<Crypto::Hash> transaction_hashes;

    KV_BEGIN_SERIALIZATION KV_MEMBER(transaction_hashes) KV_END_SERIALIZATION
  };

  struct response {
    std::vector<CryptoNote::Transaction> transactions;  // transactions blobs as hex
    std::vector<Crypto::Hash> missed_transactions;      // not found transactions
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(transactions)
    KV_MEMBER(missed_transactions)
    KV_MEMBER(status)
    KV_END_SERIALIZATION
  };
};
//-----------------------------------------------
struct COMMAND_RPC_GET_POOL_CHANGES {
  struct request {
    Crypto::Hash tail_block_hash;
    std::vector<Crypto::Hash> known_transaction_hashes;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(tail_block_hash)
    KV_MEMBER(known_transaction_hashes);
    KV_END_SERIALIZATION
  };

  struct response {
    bool is_current_tail_block;
    std::vector<Transaction> added_transactions;            // Added transactions blobs
    std::vector<Crypto::Hash> deleted_transactions_hashes;  // IDs of not found transactions
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(is_current_tail_block)
    KV_MEMBER(added_transactions)
    KV_MEMBER(deleted_transactions_hashes);
    KV_MEMBER(status)
    KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_POOL_CHANGES_LITE {
  struct request {
    Crypto::Hash tail_block_hash;
    std::vector<Crypto::Hash> known_transaction_hashes;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(tail_block_hash)
    KV_MEMBER(known_transaction_hashes);
    KV_END_SERIALIZATION
  };

  struct response {
    bool is_current_tail_block;
    std::vector<TransactionPrefixInfo> added_transactions;  // Added transactions blobs
    std::vector<Crypto::Hash> deleted_transaction_hashes;   // IDs of not found transactions
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(is_current_tail_block)
    KV_MEMBER(added_transactions)
    KV_MEMBER(deleted_transaction_hashes);
    KV_MEMBER(status)
    KV_END_SERIALIZATION
  };
};

//-----------------------------------------------
struct COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES {
  struct request {
    Crypto::Hash transaction_hash;

    KV_BEGIN_SERIALIZATION KV_MEMBER(transaction_hash) KV_END_SERIALIZATION
  };

  struct response {
    std::vector<uint64_t> output_indices;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(output_indices)
    KV_MEMBER(status)
    KV_END_SERIALIZATION
  };
};
//-----------------------------------------------
struct COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_request_entry {
  uint64_t amount;
  uint64_t count;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(amount)
  KV_MEMBER(count)
  KV_END_SERIALIZATION
};

struct COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_request {
  std::vector<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_request_entry> amounts;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(amounts)
  KV_END_SERIALIZATION
};

#pragma pack(push, 1)
struct COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_out_entry {
  uint32_t global_amount_index;
  Crypto::PublicKey out_key;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(global_amount_index)
  KV_MEMBER(out_key);
  KV_END_SERIALIZATION
};
#pragma pack(pop)

struct COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount {
  uint64_t amount;
  std::vector<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_out_entry> outs;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(amount)
  KV_MEMBER(outs);
  KV_END_SERIALIZATION
};

struct COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response {
  std::vector<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount> outs;
  std::string status;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(outs);
  KV_MEMBER(status)
  KV_END_SERIALIZATION
};

struct COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS {
  typedef COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_request request;
  typedef COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_response response;

  typedef COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_out_entry out_entry;
  typedef COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount outs_for_amount;
};

//-----------------------------------------------
struct COMMAND_RPC_SEND_RAW_TX {
  struct request {
    Transaction transaction;

    request() {
    }
    explicit request(const Transaction &);

    KV_BEGIN_SERIALIZATION KV_MEMBER(transaction) KV_END_SERIALIZATION
  };

  struct response {
    std::string status;

    KV_BEGIN_SERIALIZATION KV_MEMBER(status) KV_END_SERIALIZATION
  };
};
//-----------------------------------------------
struct COMMAND_RPC_START_MINING {
  struct request {
    AccountPublicAddress miner_address;
    uint64_t threads_count;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(miner_address)
    KV_MEMBER(threads_count)
    KV_END_SERIALIZATION
  };

  struct response {
    std::string status;

    KV_BEGIN_SERIALIZATION KV_MEMBER(status) KV_END_SERIALIZATION
  };
};
//-----------------------------------------------
struct COMMAND_RPC_GET_INFO {
  typedef Null request;

  struct response {
    std::string status;
    BlockHeight height;
    uint64_t difficulty;
    uint64_t tx_count;
    uint64_t tx_pool_size;
    Crypto::Hash tx_pool_state;
    uint64_t tx_min_fee;
    uint64_t alt_blocks_count;
    uint64_t outgoing_connections_count;
    uint64_t incoming_connections_count;
    uint64_t white_peerlist_size;
    uint64_t grey_peerlist_size;
    BlockHeight last_known_block_height;
    BlockHeight network_height;
    std::vector<BlockHeight> upgrade_heights;
    BlockHeight supported_height;
    uint32_t hashrate;
    BlockVersion version;
    std::string softwareVersion;
    uint64_t start_time;
    bool synced;
    std::string network;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(height)
    KV_MEMBER(difficulty)
    KV_MEMBER(tx_count)
    KV_MEMBER(tx_pool_size)
    KV_MEMBER(tx_pool_state)
    KV_MEMBER(tx_min_fee)
    KV_MEMBER(alt_blocks_count)
    KV_MEMBER(outgoing_connections_count)
    KV_MEMBER(incoming_connections_count)
    KV_MEMBER(white_peerlist_size)
    KV_MEMBER(grey_peerlist_size)
    KV_MEMBER(last_known_block_height)
    KV_MEMBER(network_height)
    KV_MEMBER(upgrade_heights)
    KV_MEMBER(supported_height)
    KV_MEMBER(hashrate)
    KV_MEMBER(version)
    KV_MEMBER(start_time)
    KV_MEMBER(synced)
    KV_MEMBER(network)
    KV_MEMBER(softwareVersion)
    KV_END_SERIALIZATION
  };
};

//-----------------------------------------------
struct COMMAND_RPC_STOP_MINING {
  typedef Null request;
  typedef STATUS_STRUCT response;
};

//-----------------------------------------------
struct COMMAND_RPC_STOP_DAEMON {
  typedef Null request;
  typedef STATUS_STRUCT response;
};

//
struct COMMAND_RPC_GETBLOCKCOUNT {
  typedef Null request;

  struct response {
    uint64_t count;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(count)
    KV_MEMBER(status)
    KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GETBLOCKHASH {
  struct request {
    BlockHeight height;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(height)
    KV_END_SERIALIZATION
  };

  struct response {
    Crypto::Hash hash;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(hash)
    KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GETBLOCKTEMPLATE_STATE {
  using request = Null;
  struct response {
    Crypto::Hash
        template_state;  ///< Hash unique to a previous call, iff a new requested block template would not change.
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(template_state)
    KV_MEMBER(status)
    KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GETBLOCKTEMPLATE {
  struct request {
    uint8_t reserve_size;  // max 255 bytes
    AccountPublicAddress wallet_address;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(reserve_size)
    KV_MEMBER(wallet_address)
    KV_END_SERIALIZATION
  };

  struct response {
    uint64_t difficulty;
    BlockHeight height;
    uint64_t reserved_offset;
    BlockTemplate block_template;
    std::string status;
    Crypto::Hash
        template_state;  ///< Hash unique to a previous call, iff a new requested block template would not change.

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(difficulty)
    KV_MEMBER(height)
    KV_MEMBER(reserved_offset)
    KV_MEMBER(block_template)
    KV_MEMBER(status)
    KV_MEMBER(template_state)
    KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_CURRENCY_ID {
  typedef Null request;

  struct response {
    std::string currency_id_blob;

    KV_BEGIN_SERIALIZATION KV_MEMBER(currency_id_blob) KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_SUBMITBLOCK {
  typedef std::vector<std::string> request;
  typedef STATUS_STRUCT response;
};

struct block_header_response {
  BlockVersion version;
  BlockVersion upgrade_vote;
  uint64_t timestamp;
  Crypto::Hash prev_hash;
  BlockNonce nonce;
  bool orphan_status;
  BlockHeight height;
  uint32_t depth;
  Crypto::Hash hash;
  uint64_t difficulty;
  uint64_t reward;
  uint64_t static_reward;
  uint64_t transactions_count;
  uint64_t block_size;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(version)
  KV_MEMBER(upgrade_vote)
  KV_MEMBER(timestamp)
  KV_MEMBER(prev_hash)
  KV_MEMBER(nonce)
  KV_MEMBER(orphan_status)
  KV_MEMBER(height)
  KV_MEMBER(depth)
  KV_MEMBER(hash)
  KV_MEMBER(difficulty)
  KV_MEMBER(reward)
  KV_MEMBER(static_reward)
  KV_MEMBER(transactions_count)
  KV_MEMBER(block_size)
  KV_END_SERIALIZATION
};

struct BLOCK_HEADER_RESPONSE {
  std::string status;
  block_header_response block_header;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(block_header)
  KV_MEMBER(status)
  KV_END_SERIALIZATION
};

struct COMMAND_RPC_GET_BLOCK_HEADERS_RANGE {
  struct request {
    BlockHeight start_height;
    BlockHeight end_height;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(start_height)
    KV_MEMBER(end_height)
    KV_END_SERIALIZATION
  };

  struct response {
    std::string status;
    std::vector<block_header_response> headers;
    bool untrusted;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(headers)
    KV_MEMBER(untrusted)
    KV_END_SERIALIZATION
  };
};  // namespace CryptoNote

struct f_transaction_short_response {
  Crypto::Hash hash;
  uint64_t fee;
  uint64_t amount_out;
  uint64_t size;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(hash)
  KV_MEMBER(fee)
  KV_MEMBER(amount_out)
  KV_MEMBER(size)
  KV_END_SERIALIZATION
};

struct f_transaction_details_response {
  Crypto::Hash hash;
  size_t size;
  std::optional<PaymentId> payment_id;
  uint64_t mixin;
  uint64_t fee;
  uint64_t amount_out;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(hash)
  KV_MEMBER(size)
  KV_MEMBER(payment_id)
  KV_MEMBER(mixin)
  KV_MEMBER(fee)
  KV_MEMBER(amount_out)
  KV_END_SERIALIZATION
};

struct f_block_short_response {
  uint64_t difficulty;
  uint64_t timestamp;
  BlockHeight height;
  Crypto::Hash hash;
  uint64_t cumulative_size;
  uint64_t transactions_count;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(difficulty)
  KV_MEMBER(timestamp)
  KV_MEMBER(height)
  KV_MEMBER(hash)
  KV_MEMBER(cumulative_size)
  KV_MEMBER(transactions_count)
  KV_END_SERIALIZATION
};

struct f_block_details_response : f_block_short_response {
  BlockVersion version;
  BlockVersion upgrade_vote;
  Crypto::Hash previous_hash;
  BlockNonce nonce;
  bool orphan_status;
  uint64_t depth;
  uint64_t reward;
  uint64_t static_reward;
  uint64_t block_size;
  uint64_t median_size;
  uint64_t median_effective_size;
  uint64_t transactions_cumulative_size;
  uint64_t circulating_supply;
  uint64_t transactions_generated;
  uint64_t base_reward;
  double penalty;
  uint64_t total_fee_amount;
  std::vector<f_transaction_short_response> transactions;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(version)
  KV_MEMBER(upgrade_vote)
  KV_MEMBER(timestamp)
  KV_MEMBER(previous_hash)
  KV_MEMBER(nonce)
  KV_MEMBER(orphan_status)
  KV_MEMBER(height)
  KV_MEMBER(depth)
  KV_MEMBER(hash)
  KV_MEMBER(difficulty)
  KV_MEMBER(reward)
  KV_MEMBER(static_reward)
  KV_MEMBER(block_size)
  KV_MEMBER(median_size)
  KV_MEMBER(median_effective_size)
  KV_MEMBER(transactions_cumulative_size)
  KV_MEMBER(circulating_supply)
  KV_MEMBER(transactions_generated)
  KV_MEMBER(base_reward)
  KV_MEMBER(penalty)
  KV_MEMBER(transactions)
  KV_MEMBER(total_fee_amount)
  KV_END_SERIALIZATION
};
struct COMMAND_RPC_GET_LAST_BLOCK_HEADER {
  typedef Null request;
  typedef BLOCK_HEADER_RESPONSE response;
};

struct COMMAND_RPC_GET_BLOCK_HEADER_BY_HASH {
  struct request {
    Crypto::Hash hash;

    KV_BEGIN_SERIALIZATION KV_MEMBER(hash) KV_END_SERIALIZATION
  };

  typedef BLOCK_HEADER_RESPONSE response;
};

struct COMMAND_RPC_GET_BLOCK_HEADER_BY_HEIGHT {
  struct request {
    BlockHeight height;

    KV_BEGIN_SERIALIZATION KV_MEMBER(height) KV_END_SERIALIZATION
  };

  typedef BLOCK_HEADER_RESPONSE response;
};

struct F_COMMAND_RPC_GET_BLOCKS_LIST {
  struct request {
    BlockHeight height;

    KV_BEGIN_SERIALIZATION KV_MEMBER(height) KV_END_SERIALIZATION
  };

  struct response {
    std::vector<f_block_short_response> blocks;  // transactions blobs as hex
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(blocks)
    KV_MEMBER(status) KV_END_SERIALIZATION
  };
};

struct F_COMMAND_RPC_GET_BLOCK_DETAILS {
  struct request {
    Crypto::Hash hash;

    KV_BEGIN_SERIALIZATION KV_MEMBER(hash) KV_END_SERIALIZATION
  };

  struct response {
    f_block_details_response block;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(block)
    KV_MEMBER(status) KV_END_SERIALIZATION
  };
};

struct F_COMMAND_RPC_GET_BLOCKS_RAW_BY_RANGE {
  struct request {
    BlockHeight height;  ///< Index + 1
    uint32_t count;      ///< Number of blocks to query, may not exceed 100.

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(height)
    KV_MEMBER(count)
    KV_END_SERIALIZATION
  };

  struct response {
    std::string status;
    std::vector<std::string> blocks;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(blocks)
    KV_END_SERIALIZATION
  };
};

struct F_COMMAND_RPC_GET_TRANSACTION_DETAILS {
  struct request {
    Crypto::Hash hash;

    KV_BEGIN_SERIALIZATION KV_MEMBER(hash) KV_END_SERIALIZATION
  };

  struct response {
    Transaction transaction;
    f_transaction_details_response transaction_details;
    std::optional<f_block_short_response> block;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(transaction)
    KV_MEMBER(transaction_details)
    KV_MEMBER(block)
    KV_MEMBER(status) KV_END_SERIALIZATION
  };
};

struct F_COMMAND_RPC_GET_POOL {
  typedef Null request;

  struct response {
    Crypto::Hash state;  ///< Tree hash of all transactions of the pool. Only query the pool if this has changed.
    std::vector<f_transaction_short_response> transactions;  // transactions blobs as hex
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(state);
    KV_MEMBER(transactions)
    KV_MEMBER(status) KV_END_SERIALIZATION
  };
};

struct F_P2P_BAN_INFO {
  std::string ip_address;
  uint64_t penalty_score;
  int64_t ban_timestamp;

  KV_BEGIN_SERIALIZATION
  KV_MEMBER(ip_address);
  KV_MEMBER(penalty_score)
  KV_MEMBER(ban_timestamp) KV_END_SERIALIZATION
};

struct F_COMMAND_RPC_GET_P2P_BAN_INFO {
  typedef Null request;

  struct response {
    std::vector<F_P2P_BAN_INFO> peers_ban_info;

    KV_BEGIN_SERIALIZATION KV_MEMBER(peers_ban_info);
    KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_QUERY_BLOCKS {
  struct request {
    std::vector<Crypto::Hash> block_ids;  //*first 10 blocks id goes sequential, next goes in pow(2,n) offset, like 2,
                                          // 4, 8, 16, 32, 64 and so on, and the last one is always genesis block */
    uint64_t timestamp;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(block_ids);
    KV_MEMBER(timestamp) KV_END_SERIALIZATION
  };

  struct response {
    std::string status;
    BlockHeight start_height;
    BlockHeight current_height;
    uint64_t full_offset;
    std::vector<BlockFullInfo> blocks;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(start_height)
    KV_MEMBER(current_height)
    KV_MEMBER(full_offset)
    KV_MEMBER(blocks) KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_QUERY_BLOCKS_LITE {
  struct request {
    std::vector<Crypto::Hash> block_hashes;
    uint64_t timestamp;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(block_hashes);
    KV_MEMBER(timestamp) KV_END_SERIALIZATION
  };

  struct response {
    std::string status;
    BlockHeight start_height;
    BlockHeight current_height;
    uint64_t full_offset;
    std::vector<BlockShortInfo> blocks;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(start_height)
    KV_MEMBER(current_height)
    KV_MEMBER(full_offset)
    KV_MEMBER(blocks)
    KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS {
  struct request {
    std::vector<BlockHeight> block_heights;

    KV_BEGIN_SERIALIZATION KV_MEMBER(block_heights);
    KV_END_SERIALIZATION
  };

  struct response {
    std::vector<BlockDetails> blocks;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(blocks) KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES {
  struct request {
    std::vector<Crypto::Hash> block_hashes;

    KV_BEGIN_SERIALIZATION KV_MEMBER(block_hashes);
    KV_END_SERIALIZATION
  };

  struct response {
    std::vector<BlockDetails> blocks;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(blocks) KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_QUERY_BLOCKS_DETAILED {
  struct request {
    std::vector<Crypto::Hash> block_hashes;
    uint64_t timestamp;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(block_hashes);
    KV_MEMBER(timestamp) KV_END_SERIALIZATION
  };

  struct response {
    std::string status;
    BlockHeight start_height;
    BlockHeight current_height;
    uint64_t full_offset;
    std::vector<BlockDetails> blocks;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(start_height)
    KV_MEMBER(current_height)
    KV_MEMBER(full_offset)
    KV_MEMBER(blocks) KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT {
  struct request {
    BlockHeight block_height;

    KV_BEGIN_SERIALIZATION KV_MEMBER(block_height) KV_END_SERIALIZATION
  };

  struct response {
    BlockDetails block;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(block) KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS {
  struct request {
    uint64_t timestamp_begin;
    uint64_t timestamp_end;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(timestamp_begin)
    KV_MEMBER(timestamp_end) KV_END_SERIALIZATION
  };

  struct response {
    std::vector<Crypto::Hash> block_hashes;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(block_hashes) KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID {
  struct request {
    PaymentId payment_id;

    KV_BEGIN_SERIALIZATION KV_MEMBER(payment_id) KV_END_SERIALIZATION
  };

  struct response {
    std::vector<Crypto::Hash> transactionHashes;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(transactionHashes) KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES {
  struct request {
    std::vector<Crypto::Hash> transaction_hashes;

    KV_BEGIN_SERIALIZATION KV_MEMBER(transaction_hashes);
    KV_END_SERIALIZATION
  };

  struct response {
    std::vector<TransactionDetails> transactions;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(transactions) KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_PEERS {
  // TODO useful to add option to get gray peers ?
  typedef Null request;

  struct response {
    std::string status;
    std::vector<NetworkAddress> peers_white;
    std::vector<NetworkAddress> peers_gray;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(status)
    KV_MEMBER(peers_white)
    KV_MEMBER(peers_gray)
    KV_END_SERIALIZATION
  };
};

struct COMMAND_RPC_GET_FEE_ADDRESS {
  typedef Null request;

  struct response {
    std::optional<FeeAddress> fee;
    std::string status;

    KV_BEGIN_SERIALIZATION
    KV_MEMBER(fee)
    KV_MEMBER(status)
    KV_END_SERIALIZATION
  };
};

}  // namespace CryptoNote
