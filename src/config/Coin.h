#pragma once

#include <string>

#include <Xi/Utils/ConstExprMath.h>
#include <Xi/Utils/Conversion.h>

namespace CryptoNote {
namespace Config {
namespace Coin {
inline std::string name() { return "XI"; }
inline constexpr uint32_t addressBas58Prefix() { return 22583; }
constexpr uint8_t numberOfDecimalPoints() { return 6; }
constexpr uint64_t toAtomicUnits(uint64_t coins) { return coins * Xi::pow(10, numberOfDecimalPoints()); }
constexpr uint64_t totalSupply() { return toAtomicUnits(55_M); }
constexpr uint64_t amountOfPremine() { return toAtomicUnits(3300_k); }
constexpr uint64_t minimumFee() { return 100; }
constexpr uint32_t emissionSpeed() { return 21; }

inline std::string licenseUrl() { return "https://gitlab.com/galaxia-project/blockchain/xi/blob/develop/LICENSE"; }
inline std::string downloadUrl() { return "http://release.xi.galaxiaproject.org"; }

/*
 * This is the unix timestamp of the first "mined" block (technically block 2, not the genesis block)
 * You can get this value by doing "print_block 2" in xi-daemon. It is used to know what timestamp.\xi-
 * to import from when the block height cannot be found in the node or the node is offline.
 */
inline uint64_t genesisTimestamp() { return 1544396293; }

/*
 * How to generate a premine:
 * - Compile your code
 * - Run xi-wallet, ignore that it can't connect to the daemon, and generate an address. Save this and the keys
 * somewhere safe.
 * - Launch the daemon with these arguments:
 *        xi-daemon --print-genesis-tx --genesis-block-reward-address <premine wallet address>
 * - Take the hash printed, and replace it with the hash below in GENESIS_COINBASE_TX_HEX
 * - Recompile, setup your seed nodes, and start mining
 * - You should see your premine appear in the previously generated wallet.
 */
inline std::string genesisTransactionHash() {
  return "01a00b01ff000180d0cfba8560024dd3e9e2c7c25fb8a9ec38c57bf39b8913c2c7858a86429beb64abad700b5dc62101106668f11d65d"
         "ae28eab51dbbf98ebd30d914161b66ec642022b9a55b25dc6cd";
}

static_assert(emissionSpeed() <= 8 * sizeof(uint64_t), "Bad emission speed.");
}  // namespace Coin
}  // namespace Config
}  // namespace CryptoNote
