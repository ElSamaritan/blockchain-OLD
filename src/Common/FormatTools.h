// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <string>
#include <ostream>
#include "Rpc/CoreRpcServerCommandsDefinitions.h"
#include "CryptoNoteCore/Currency.h"

namespace Common {
struct StatusInfo {
  uint64_t start_time;
  CryptoNote::BlockVersion version;
  CryptoNote::BlockHeight height;
  CryptoNote::BlockHeight network_height;
  CryptoNote::BlockHeight supported_height;
  std::vector<CryptoNote::BlockHeight> upgrade_heights;
  Xi::Config::Network::Type network;
  bool synced;
  uint32_t hashrate;
  uint32_t outgoing_connections_count;
  uint32_t incoming_connections_count;
};

std::string get_mining_speed(uint32_t hr);
std::string get_sync_percentage(CryptoNote::BlockHeight height, CryptoNote::BlockHeight target_height);
std::string get_upgrade_time(CryptoNote::BlockHeight height, CryptoNote::BlockHeight upgrade_height);
std::string get_status_string(const StatusInfo& iresp, const CryptoNote::Currency& currency);

void printStatus(const StatusInfo& iresp, const CryptoNote::Currency& currency, std::ostream& stream);
}  // namespace Common
