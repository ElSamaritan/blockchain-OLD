// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <initializer_list>
#include <chrono>

#include <Xi/Utils/ConstExprMath.h>
#include <Xi/Utils/Conversion.h>

#include <boost/uuid/uuid.hpp>

#include "config/Coin.h"
#include "config/Network.h"
#include "config/P2P.h"
#include "config/Mixin.h"

namespace CryptoNote {
namespace Config {

const uint64_t DIFFICULTY_TARGET = 60;

const uint32_t CRYPTONOTE_MAX_BLOCK_NUMBER = 500000000;
const size_t CRYPTONOTE_MAX_BLOCK_BLOB_SIZE = 500000000;
const size_t CRYPTONOTE_MAX_TX_SIZE = 1000000000;
const uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX = 22583;
const uint32_t CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW = 1440;
const uint64_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT = 60 * 60 * 2;
const uint64_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V3 = 3 * DIFFICULTY_TARGET;
const uint64_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V4 = CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT_V3;

const uint32_t BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW = 60;
const uint32_t BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW_V3 = 11;

const uint32_t ZAWY_DIFFICULTY_BLOCK_INDEX = 0;
const size_t ZAWY_DIFFICULTY_V2 = 0;
const uint8_t ZAWY_DIFFICULTY_DIFFICULTY_BLOCK_VERSION = 3;

const uint32_t LWMA_2_DIFFICULTY_BLOCK_INDEX = 5;
const uint32_t LWMA_2_DIFFICULTY_BLOCK_INDEX_V2 = 6;
const uint32_t LWMA_2_DIFFICULTY_BLOCK_INDEX_V3 = 7;
const uint32_t LWMA_3_DIFFICULTY_BLOCK_INDEX = 8;

const uint32_t DIFFICULTY_WINDOW_V3 = 64;
const uint32_t DIFFICULTY_BLOCKS_COUNT_V3 = DIFFICULTY_WINDOW_V3 + 1;

const unsigned EMISSION_SPEED_FACTOR = 50;
static_assert(EMISSION_SPEED_FACTOR <= 8 * sizeof(uint64_t), "Bad EMISSION_SPEED_FACTOR");

/* Premine amount */
const uint64_t GENESIS_BLOCK_REWARD = UINT64_C(1200000 * 1000000);

/* How to generate a premine:

* Compile your code

* Run xi-wallet, ignore that it can't connect to the daemon, and generate an
  address. Save this and the keys somewhere safe.

* Launch the daemon with these arguments:
--print-genesis-tx --genesis-block-reward-address <premine wallet address>

For example:
xi-daemon --print-genesis-tx --genesis-block-reward-address
XIv2Fyavy8CXG8BPEbNeCHFZ1fuDCYCZ3vW5H5LXN4K2M2MHUpTENip9bbavpHvvPwb4NDkBWrNgURAd5DB38FHXWZyoBh4wW

* Take the hash printed, and replace it with the hash below in GENESIS_COINBASE_TX_HEX

* Recompile, setup your seed nodes, and start mining

* You should see your premine appear in the previously generated wallet.

*/
const char GENESIS_COINBASE_TX_HEX[] =
    "01a00b01ff000180c0cbacf622029b2e4c0281c0b02e7c53291a94d1d0cbff8883f8024f5142ee494ffbbd088071210116b7eaec13f43967ca"
    "dd52d6ed6bb0d0901cb7db66e7b221595d448593a303a2";

/* This is the unix timestamp of the first "mined" block (technically block 2, not the genesis block)
   You can get this value by doing "print_block 2" in xi-daemon. It is used to know what timestamp.\xi-
   to import from when the block height cannot be found in the node or the node is offline. */
const uint64_t GENESIS_BLOCK_TIMESTAMP = 1544396293;  // WARNING(mainnet) Needs to be adjusted

const size_t CRYPTONOTE_REWARD_BLOCKS_WINDOW = 50;
const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE =
    100000;  // size of block (bytes) after which reward for block calculated using block size
const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V2 = 20000;
const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1 = 10000;
const size_t CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_CURRENT = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE;
const size_t CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE = 600;
const uint64_t MINIMUM_FEE = UINT64_C(1000);

const uint64_t DEFAULT_DUST_THRESHOLD = UINT64_C(10);
const uint64_t DEFAULT_DUST_THRESHOLD_V2 = DEFAULT_DUST_THRESHOLD;

const uint32_t DUST_THRESHOLD_V2_HEIGHT = 3000;
const uint32_t FUSION_DUST_THRESHOLD_HEIGHT_V2 = 2;

const uint64_t EXPECTED_NUMBER_OF_BLOCKS_PER_DAY = 24 * 60 * 60 / DIFFICULTY_TARGET;
const size_t DIFFICULTY_WINDOW = 17;
const size_t DIFFICULTY_WINDOW_V1 = 2880;
const size_t DIFFICULTY_WINDOW_V2 = 2880;
const size_t DIFFICULTY_CUT = 0;  // timestamps to cut after sorting
const size_t DIFFICULTY_CUT_V1 = 60;
const size_t DIFFICULTY_CUT_V2 = 60;
const size_t DIFFICULTY_LAG = 0;  // !!!
const size_t DIFFICULTY_LAG_V1 = 15;
const size_t DIFFICULTY_LAG_V2 = 15;
static_assert(2 * DIFFICULTY_CUT <= DIFFICULTY_WINDOW - 2, "Bad DIFFICULTY_WINDOW or DIFFICULTY_CUT");

const size_t MAX_BLOCK_SIZE_INITIAL = 100000;
const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_NUMERATOR = 100 * 1024;
const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_DENOMINATOR = 365 * 24 * 60 * 60 / DIFFICULTY_TARGET;
const uint64_t MAX_EXTRA_SIZE = 140000;

const uint64_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS = 1;
const uint64_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS =
    DIFFICULTY_TARGET * CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS;

const uint64_t CRYPTONOTE_MEMPOOL_TX_LIVETIME = 60 * 60 * 24;                     // seconds, one day
const uint64_t CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME = 60 * 60 * 24 * 7;  // seconds, one week
const uint64_t CRYPTONOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL =
    7;  // CRYPTONOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL * CRYPTONOTE_MEMPOOL_TX_LIVETIME = time to forget
        // tx

const size_t FUSION_TX_MAX_SIZE = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_CURRENT * 30 / 100;
const size_t FUSION_TX_MIN_INPUT_COUNT = 12;
const size_t FUSION_TX_MIN_IN_OUT_COUNT_RATIO = 4;

const uint32_t KEY_IMAGE_CHECKING_BLOCK_INDEX = 0;
const uint32_t UPGRADE_HEIGHT_V2 = 1;
const uint32_t UPGRADE_HEIGHT_V3 = 2;
const uint32_t UPGRADE_HEIGHT_V4 = 3;  // Upgrade height for CN-X
const uint32_t UPGRADE_HEIGHT_CURRENT = UPGRADE_HEIGHT_V4;
const unsigned UPGRADE_VOTING_THRESHOLD = 90;                              // percent
const uint32_t UPGRADE_VOTING_WINDOW = EXPECTED_NUMBER_OF_BLOCKS_PER_DAY;  // blocks
const uint32_t UPGRADE_WINDOW = EXPECTED_NUMBER_OF_BLOCKS_PER_DAY;         // blocks
static_assert(0 < UPGRADE_VOTING_THRESHOLD && UPGRADE_VOTING_THRESHOLD <= 100, "Bad UPGRADE_VOTING_THRESHOLD");
static_assert(UPGRADE_VOTING_WINDOW > 1, "Bad UPGRADE_VOTING_WINDOW");

/* Block heights we are going to have hard forks at */
const uint64_t FORK_HEIGHTS[] = {
    1,  // 0
    2,  // 1
    3,  // 2
    4   // 3
};

/* MAKE SURE TO UPDATE THIS VALUE WITH EVERY MAJOR RELEASE BEFORE A FORK */
const uint64_t SOFTWARE_SUPPORTED_FORK_INDEX = 3;

const uint64_t FORK_HEIGHTS_SIZE = sizeof(FORK_HEIGHTS) / sizeof(*FORK_HEIGHTS);

/* The index in the FORK_HEIGHTS array that this version of the software will
   support. For example, if CURRENT_FORK_INDEX is 3, this version of the
   software will support the fork at 600,000 blocks.

   This will default to zero if the FORK_HEIGHTS array is empty, so you don't
   need to change it manually. */
const uint8_t CURRENT_FORK_INDEX = FORK_HEIGHTS_SIZE == 0 ? 0 : SOFTWARE_SUPPORTED_FORK_INDEX;

static_assert(CURRENT_FORK_INDEX >= 0, "CURRENT FORK INDEX must be >= 0");
/* Make sure CURRENT_FORK_INDEX is a valid index, unless FORK_HEIGHTS is empty */
static_assert(FORK_HEIGHTS_SIZE == 0 || CURRENT_FORK_INDEX < FORK_HEIGHTS_SIZE,
              "CURRENT_FORK_INDEX out of range of FORK_HEIGHTS!");

const char CRYPTONOTE_BLOCKS_FILENAME[] = "blocks.bin";
const char CRYPTONOTE_BLOCKINDEXES_FILENAME[] = "blockindexes.bin";
const char CRYPTONOTE_POOLDATA_FILENAME[] = "poolstate.bin";
const char P2P_NET_DATA_FILENAME[] = "p2pstate.bin";
const char MINER_CONFIG_FILE_NAME[] = "miner_conf.json";
}  // namespace Config

const char CRYPTONOTE_NAME[] = "XI";

const uint8_t TRANSACTION_VERSION_1 = 1;
const uint8_t TRANSACTION_VERSION_2 = 2;
const uint8_t CURRENT_TRANSACTION_VERSION = TRANSACTION_VERSION_1;
const uint8_t BLOCK_MAJOR_VERSION_1 = 1;
const uint8_t BLOCK_MAJOR_VERSION_2 = 2;
const uint8_t BLOCK_MAJOR_VERSION_3 = 3;
const uint8_t BLOCK_MAJOR_VERSION_4 = 4;
const uint8_t BLOCK_MINOR_VERSION_0 = 0;
const uint8_t BLOCK_MINOR_VERSION_1 = 1;

const size_t BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT = 10000;  // by default, blocks ids count in synchronizing
const size_t BLOCKS_SYNCHRONIZING_DEFAULT_COUNT = 100;        // by default, blocks count in blocks downloading
const size_t COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT = 1000;

const uint16_t RPC_DEFAULT_PORT = 22869;
const uint16_t SERVICE_DEFAULT_PORT = 38070;

const uint64_t DATABASE_WRITE_BUFFER_DEFAULT_SIZE = 256_MB;
const uint64_t DATABASE_READ_BUFFER_DEFAULT_SIZE = 10_MB;
const uint32_t DATABASE_DEFAULT_MAX_OPEN_FILES = 100;
const uint16_t DATABASE_DEFAULT_BACKGROUND_THREADS_COUNT = 2;

const char LICENSE_URL[] = "https://gitlab.com/galaxia-project/blockchain/xi/blob/develop/LICENSE";
const char LATEST_VERSION_URL[] = "http://release.xi.galaxiaproject.org";

const char* const SEED_NODES[] = {"207.180.240.151:22868", "207.180.240.152:22868"};
}  // namespace CryptoNote
