/* ============================================================================================== *
 *                                                                                                *
 *                                     Galaxia Blockchain                                         *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Xi framework.                                                         *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               *
 *                                                                                                *
 * This program is free software: you can redistribute it and/or modify it under the terms of the *
 * GNU General Public License as published by the Free Software Foundation, either version 3 of   *
 * the License, or (at your option) any later version.                                            *
 *                                                                                                *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 * See the GNU General Public License for more details.                                           *
 *                                                                                                *
 * You should have received a copy of the GNU General Public License along with this program.     *
 * If not, see <https://www.gnu.org/licenses/>.                                                   *
 *                                                                                                *
 * ============================================================================================== */

#pragma once

#include <string>

#include <Xi/Algorithm/Math.h>

#include "Xi/Config/NetworkType.h"

namespace Xi {
namespace Config {
namespace Coin {
inline std::string name() { return "Galaxia"; }
inline std::string ticker() { return "GLX"; }
inline std::string addressPrefix() { return "gxi"; }
inline constexpr uint32_t addressBas58Prefix() { return 0x1cf46e; }
constexpr uint8_t numberOfDecimalPoints() { return 6; }
constexpr uint64_t toAtomicUnits(uint64_t coins) { return coins * Xi::pow(10, numberOfDecimalPoints()); }
constexpr uint64_t totalSupply() { return toAtomicUnits(55_M); }
constexpr uint64_t amountOfPremine() { return toAtomicUnits(500_k); }
constexpr uint64_t minimumFee() { return 1000; }
constexpr uint32_t emissionSpeed() { return 21; }

inline std::string homepage() { return "https://galaixia-project.com"; }
inline std::string description() { return "An evolving framework for blockchain-based applications."; }
inline std::string copyright() { return "(c) 2018-2019 Xi Project Developers"; }
inline std::string contactUrl() { return "https://chat.galaxia-project.com"; }
inline std::string licenseUrl() { return "https://gitlab.com/galaxia-project/blockchain/blob/develop/LICENSE"; }
inline std::string downloadUrl() { return "https://releases.galaxia-project.com"; }

uint64_t genesisTimestamp(Xi::Config::Network::Type network);

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
std::string genesisTransactionBlob(Network::Type network);

static_assert(emissionSpeed() <= 8 * sizeof(uint64_t), "Bad emission speed.");
}  // namespace Coin
}  // namespace Config
}  // namespace Xi
