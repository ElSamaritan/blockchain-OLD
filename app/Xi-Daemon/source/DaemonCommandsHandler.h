// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Common/ConsoleHandler.h"

#include <Logging/LoggerRef.h>
#include <Logging/LoggerManager.h>
#include <Xi/Blockchain/Explorer/IExplorer.hpp>
#include <Xi/Blockchain/Services/BlockExplorer/BlockExplorer.hpp>
#include <CryptoNoteProtocol/CryptoNoteProtocolHandler.h>
#include "Rpc/RpcServer.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"
#include "Rpc/JsonRpc.h"

namespace CryptoNote {
class Core;
class NodeServer;
}  // namespace CryptoNote

class DaemonCommandsHandler : public Common::ConsoleHandler {
 public:
  DaemonCommandsHandler(CryptoNote::Core& core, CryptoNote::NodeServer& srv,
                        CryptoNote::CryptoNoteProtocolHandler& protocol, Logging::LoggerManager& log);

 private:
  CryptoNote::Core& m_core;
  CryptoNote::NodeServer& m_srv;
  CryptoNote::CryptoNoteProtocolHandler& m_protocol;
  Logging::LoggerRef logger;
  Logging::LoggerManager& m_logManager;

  std::shared_ptr<Xi::Blockchain::Explorer::IExplorer> m_explorer;
  std::shared_ptr<Xi::Blockchain::Services::BlockExplorer::BlockExplorer> m_explorerService;

  std::string get_commands_str();

  bool help(const std::vector<std::string>& args);
  bool version(const std::vector<std::string>& args);
  bool show_hr(const std::vector<std::string>& args);
  bool hide_hr(const std::vector<std::string>& args);
  bool log_set(const std::vector<std::string>& args);
  bool start_mining(const std::vector<std::string>& args);
  bool stop_mining(const std::vector<std::string>& args);
  bool status(const std::vector<std::string>& args);

  /* ----------------------------------------------- Explorer Commands --------------------------------------------- */
  bool info(const std::vector<std::string>& args);
  bool search(const std::vector<std::string>& args);
  bool search_detailed(const std::vector<std::string>& args);
  bool top(const std::vector<std::string>& args);
  bool top_short(const std::vector<std::string>& args);
  bool top_detailed(const std::vector<std::string>& args);
  bool block(const std::vector<std::string>& args);
  bool block_short(const std::vector<std::string>& args);
  bool block_detailed(const std::vector<std::string>& args);
  bool transaction(const std::vector<std::string>& args);
  bool transaction_short(const std::vector<std::string>& args);
  bool transaction_detailed(const std::vector<std::string>& args);
  /* ----------------------------------------------- Explorer Commands --------------------------------------------- */

  /* ------------------------------------------------- Pool Commands ----------------------------------------------- */
  bool pool_detailed(const std::vector<std::string>& args);
  bool pool(const std::vector<std::string>& args);
  bool pool_check(const std::vector<std::string>& args);
  bool pool_flush(const std::vector<std::string>& args);
  bool pool_remove(const std::vector<std::string>& args);
  /* ------------------------------------------------- Pool Commands ----------------------------------------------- */

  /* -------------------------------------------------- P2P Commands ----------------------------------------------- */
  bool p2p_peers(const std::vector<std::string>& args);
  bool p2p_connections(const std::vector<std::string>& args);
  bool p2p_ban_list(const std::vector<std::string>& args);
  bool p2p_penalty_list(const std::vector<std::string>& args);
  bool p2p_penalty_reset(const std::vector<std::string>& args);
  bool p2p_ban_ip(const std::vector<std::string>& args);
  bool p2p_unban_ip(const std::vector<std::string>& args);
  bool p2p_unban_all(const std::vector<std::string>& args);
  /* -------------------------------------------------- P2P Commands ----------------------------------------------- */

  std::vector<uint32_t> parseIps(const std::vector<std::string>& args);
};
