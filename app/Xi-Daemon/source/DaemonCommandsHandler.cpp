// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "DaemonCommandsHandler.h"

#include <ctime>
#include <chrono>
#include <memory>

#include <Xi/ExternalIncludePush.h>
#include <rang.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Global.hh>
#include <Xi/Time.h>
#include <Xi/Algorithm/String.h>
#include <Xi/Version/Version.h>
#include <CommonCLI/CommonCLI.h>
#include <Xi/Blockchain/Explorer/CoreExplorer.hpp>

#include "P2p/NetNode.h"
#include "CryptoNoteCore/Core.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "Serialization/SerializationTools.h"
#include "Serialization/SerializationOverloads.h"
#include "Serialization/ConsoleOutputSerializer.hpp"

#include "Rpc/JsonRpc.h"
#include "CryptoNoteCore/Currency.h"
#include <boost/format.hpp>
#include "CryptoNoteCore/FormatTools.h"
#include "Common/StringTools.h"

#define DAEMON_COMMAND_DEFINE(NAME, HELP) setHandler(#NAME, boost::bind(&DaemonCommandsHandler::NAME, this, _1), HELP)

DaemonCommandsHandler::DaemonCommandsHandler(CryptoNote::Core& core, CryptoNote::NodeServer& srv,
                                             CryptoNote::CryptoNoteProtocolHandler& protocol,
                                             Logging::LoggerManager& log, Logging::CommonLogger& clogger)
    : m_core(core), m_srv(srv), m_protocol{protocol}, m_logManager(log), m_consoleLogger{clogger} {
  m_explorer = std::make_shared<Xi::Blockchain::Explorer::CoreExplorer>(core);
  m_explorerService = Xi::Blockchain::Services::BlockExplorer::BlockExplorer::create(m_explorer, log);

  DAEMON_COMMAND_DEFINE(help, "Show this help");
  DAEMON_COMMAND_DEFINE(version, "Shows version information about the running daemon");
  DAEMON_COMMAND_DEFINE(log_set,
                        "Sets the application log level, [<number>|none|fatal|error|warning|info|debugging|trace]");
  DAEMON_COMMAND_DEFINE(status, "Show daemon status");

  // clang-format off
  /* ----------------------------------------------- Explorer Commands --------------------------------------------- */
  DAEMON_COMMAND_DEFINE(info, "Prints general informations about the curring blockchain");
  DAEMON_COMMAND_DEFINE(search, "Searches for a block height/hash or transaction hash");
  DAEMON_COMMAND_DEFINE(search_detailed, "Searches for a block height/hash or transaction hash");
  DAEMON_COMMAND_DEFINE(top, "Displays top block info");
  DAEMON_COMMAND_DEFINE(top_short, "Displays top block short info");
  DAEMON_COMMAND_DEFINE(top_detailed, "Displays top block detailed info");
  DAEMON_COMMAND_DEFINE(block, "Displays block info [<hash>|<height>]+");
  DAEMON_COMMAND_DEFINE(block_short, "Displays block short info [<hash>|<height>]+");
  DAEMON_COMMAND_DEFINE(block_detailed, "Displays block detailed info [<hash>|<height>]+");
  DAEMON_COMMAND_DEFINE(transaction, "Displays transaction info [<hash>]+");
  DAEMON_COMMAND_DEFINE(transaction_short, "Displays transaction short info [<hash>]+");
  DAEMON_COMMAND_DEFINE(transaction_detailed, "Displays transaction detailed info [<hash>]+");

  /* ------------------------------------------------- Pool Commands ----------------------------------------------- */
  DAEMON_COMMAND_DEFINE(pool, "Print transaction pool (short format)");
  DAEMON_COMMAND_DEFINE(pool_detailed, "Print transaction pool (long format)");
  DAEMON_COMMAND_DEFINE(pool_flush, "Removes all pool transactions");
  DAEMON_COMMAND_DEFINE(pool_check, "Performs a sanity check on the current pool state, deletes all failing transactions");
  DAEMON_COMMAND_DEFINE(pool_remove, "Removes a transaction with given hash from the pool <transaction_hash>");

  /* -------------------------------------------------- P2P Commands ----------------------------------------------- */
  DAEMON_COMMAND_DEFINE(p2p_peers, "Print peer list");
  DAEMON_COMMAND_DEFINE(p2p_connections, "Print connections");
  DAEMON_COMMAND_DEFINE(p2p_ban_list, "Lists all currently banned peers.");
  DAEMON_COMMAND_DEFINE(p2p_penalty_list, "Lists all peers with penalities.");
  DAEMON_COMMAND_DEFINE(p2p_ban_ip, "Adds given ips to the ban list.  '<ip> [<ip> ]*'");
  DAEMON_COMMAND_DEFINE(p2p_unban_ip, "Removes given ips from the ban list. '<ip> [<ip> ]*'");
  DAEMON_COMMAND_DEFINE(p2p_unban_all, "Removes all banned peers from the ban list.");
  // clang-format on
}

#undef DAEMON_COMMAND_DEFINE

#define DAEMON_COMMAND_ARGS_COND(COND, HELP)                \
  do {                                                      \
    if (!(COND)) {                                          \
      std::cout << "Command failed: " << HELP << std::endl; \
      return false;                                         \
    }                                                       \
  } while (false)

#define DAEMON_COMMAND_EXPECTED_ARGS(NUM, HELP) DAEMON_COMMAND_ARGS_COND(args.size() == NUM, HELP)

//--------------------------------------------------------------------------------
std::string DaemonCommandsHandler::get_commands_str() {
  std::stringstream ss;
  ss << m_core.currency().coin().name() << " v" << APP_VERSION << ENDL << ENDL;
  ss << "Commands: " << ENDL;
  std::string usage = getUsage();
  boost::replace_all(usage, "\n", "\n  ");
  usage.insert(0, "  ");
  ss << usage << ENDL;
  return ss.str();
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::help(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  std::cout << get_commands_str() << ENDL;
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::version(const std::vector<std::string>& args) {
  DAEMON_COMMAND_EXPECTED_ARGS(0, "No arguments expected.");
  printObject(CommonCLI::versionInformation(), "Version");
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::log_set(const std::vector<std::string>& args) {
  DAEMON_COMMAND_EXPECTED_ARGS(1, "One argument expected");

  Logging::LevelTranslator translator{};
  auto level = translator.get_value(args.front());
  if (!level.has_value()) {
    return false;
  } else {
    m_consoleLogger.setMaxLevel(*level);
    return true;
  }
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::pool_detailed(const std::vector<std::string>& args) {
  XI_UNUSED(args);

  if (const auto poolInfo = m_explorer->queryPoolInfo(); poolInfo.isError()) {
    printError(poolInfo.error().message());
    return false;
  } else {
    return printObject(*poolInfo, "Pool");
  }
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::pool(const std::vector<std::string>& args) {
  XI_UNUSED(args);

  if (const auto poolInfo = m_explorer->queryShortPoolInfo(); poolInfo.isError()) {
    printError(poolInfo.error().message());
    return false;
  } else {
    return printObject(*poolInfo, "Pool");
  }
}

bool DaemonCommandsHandler::pool_check(const std::vector<std::string>& args) {
  DAEMON_COMMAND_EXPECTED_ARGS(0, "No argument expected.");
  auto departed = m_core.transactionPool().sanityCheck(std::numeric_limits<uint64_t>::max());
  return printObject(departed, "Cleared Transactions");
}

bool DaemonCommandsHandler::pool_flush(const std::vector<std::string>& args) {
  DAEMON_COMMAND_EXPECTED_ARGS(0, "No argument expected.");

  const auto count = m_core.transactionPool().forceFlush();
  std::cout << count << " transactions deleted" << std::endl;
  return true;
}

bool DaemonCommandsHandler::pool_remove(const std::vector<std::string>& args) {
  DAEMON_COMMAND_EXPECTED_ARGS(1, "No argument expected.");

  Crypto::Hash txHash;
  if (!parse_hash256(args[0], txHash)) {
    std::cout << "Failed to parse transaction hash: " << args[0] << std::endl;
    return false;
  }

  auto isRemoved = m_core.transactionPool().forceErasure(txHash);
  std::cout << "Transaction removal " << (isRemoved ? "succeeded" : "failed") << "." << std::endl;

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::status(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  Common::StatusInfo info{};
  info.start_time = static_cast<uint64_t>(m_core.getStartTime());
  info.version = m_core.getTopBlockVersion();
  info.height = CryptoNote::BlockHeight::fromIndex(m_core.getTopBlockIndex());
  info.network_height = m_protocol.getBlockchainHeight();
  const auto forks = getForks(m_core.upgradeManager());
  std::transform(forks.begin(), forks.end(), std::back_inserter(info.upgrade_heights),
                 [](const auto forkIndex) { return CryptoNote::BlockHeight::fromIndex(forkIndex); });
  info.supported_height =
      info.upgrade_heights.empty() ? CryptoNote::BlockHeight::fromIndex(0) : *info.upgrade_heights.rbegin();
  info.network = m_core.currency().network().type();
  info.synced = m_protocol.isSynchronized();
  info.hashrate = static_cast<uint32_t>(round(m_core.getDifficultyForNextBlock() /
                                              m_core.currency().coin().blockTime()));  // TODO Not a good approximation
  const uint32_t totalConnections = static_cast<uint32_t>(m_srv.get_connections_count());
  info.outgoing_connections_count = static_cast<uint32_t>(m_srv.get_outgoing_connections_count());
  info.incoming_connections_count = totalConnections - info.outgoing_connections_count;
  std::cout << "\n";
  Common::printStatus(info, m_core.currency(), std::cout);
  std::cout << "\n" << std::endl;
  return true;
}

bool DaemonCommandsHandler::info(const std::vector<std::string>& args) {
  XI_RETURN_EC_IF_NOT(args.empty(), false);
  XI_RETURN_EC_IF_NOT(printObject(m_core.currency().general(), "general"), false);
  XI_RETURN_EC_IF_NOT(printObject(m_core.currency().coin(), "coin"), false);
  XI_RETURN_SC(true);
}

bool DaemonCommandsHandler::search(const std::vector<std::string>& args) {
  try {
    XI_RETURN_EC_IF(args.empty(), false);

    for (const auto& iArg : args) {
      std::optional<Xi::Blockchain::Services::BlockExplorer::SearchResponse> res;
      const auto ec = m_explorerService->process("", iArg, res);
      if (ec != Xi::Rpc::ServiceError::Success) {
        printError(toString(ec));
        return false;
      }
      printObject(res, iArg);
    }
    return true;
  } catch (std::exception& e) {
    printError(e.what());
    return false;
  }
}

bool DaemonCommandsHandler::search_detailed(const std::vector<std::string>& args) {
  using namespace Xi::Blockchain::Explorer;
  try {
    XI_RETURN_EC_IF(args.empty(), false);

    for (const auto& iArg : args) {
      std::optional<Xi::Blockchain::Services::BlockExplorer::SearchResponse> res;
      const auto ec = m_explorerService->process("", iArg, res);
      if (ec != Xi::Rpc::ServiceError::Success) {
        printError(toString(ec));
        return false;
      } else if (!res) {
        printObject(res, iArg);
      } else {
        if (const auto blk = std::get_if<ShortBlockInfo>(std::addressof(*res))) {
          XI_RETURN_EC_IF_NOT(block({blk->hash.toString()}), false);
        } else if (const auto tx = std::get_if<ShortTransactionInfo>(std::addressof(*res))) {
          XI_RETURN_EC_IF_NOT(transaction({tx->hash.toString()}), false);
        }
      }
    }
    return true;
  } catch (std::exception& e) {
    printError(e.what());
    return false;
  }
}

bool DaemonCommandsHandler::top(const std::vector<std::string>&) {
  try {
    std::optional<Xi::Blockchain::Explorer::BlockInfo> result{std::nullopt};
    const auto ec = m_explorerService->process("", std::nullopt, result);
    if (ec != Xi::Rpc::ServiceError::Success) {
      std::cout << toString(ec) << std::endl;
      return false;
    }
    return printObject(result, "Top Block");
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

bool DaemonCommandsHandler::top_short(const std::vector<std::string>&) {
  try {
    std::optional<Xi::Blockchain::Explorer::ShortBlockInfo> result{std::nullopt};
    const auto ec = m_explorerService->process("", std::nullopt, result);
    if (ec != Xi::Rpc::ServiceError::Success) {
      std::cout << toString(ec) << std::endl;
      return false;
    }
    return printObject(result, "Top Block");
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

bool DaemonCommandsHandler::top_detailed(const std::vector<std::string>&) {
  try {
    std::optional<Xi::Blockchain::Explorer::DetailedBlockInfo> result{std::nullopt};
    const auto ec = m_explorerService->process("", std::nullopt, result);
    if (ec != Xi::Rpc::ServiceError::Success) {
      std::cout << toString(ec) << std::endl;
      return false;
    }
    return printObject(result, "Top Block");
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return false;
  }
}

namespace {
std::variant<CryptoNote::BlockHash, CryptoNote::BlockHeight, CryptoNote::Null> parseBlockSearchArgument(
    const std::string& arg) {
  if (const auto height = CryptoNote::BlockHeight::fromString(arg); !height.isNull()) {
    return height;
  } else if (auto hash = CryptoNote::BlockHash::fromString(arg); !hash.isError()) {
    return hash.take();
  } else {
    return CryptoNote::Null{};
  }
}
}  // namespace

bool DaemonCommandsHandler::block(const std::vector<std::string>& args) {
  using namespace Xi::Blockchain::Explorer;

  DAEMON_COMMAND_ARGS_COND(args.size() > 0, "At least one argument expected.");

  for (const auto& arg : args) {
    auto blockArg = parseBlockSearchArgument(Xi::trim(arg));
    if (std::holds_alternative<CryptoNote::Null>(blockArg)) {
      std::string _{"invalid argument"};
      XI_RETURN_EC_IF_NOT(printObject(_, arg), false);
    } else if (const auto height = std::get_if<CryptoNote::BlockHeight>(std::addressof(blockArg))) {
      auto block = m_explorer->queryBlockInfo(*height);
      if (block.isError()) {
        printError(block.error().message());
      } else {
        XI_RETURN_EC_IF_NOT(printObject(*block, arg), false);
      }
    } else if (const auto hash = std::get_if<CryptoNote::BlockHash>(std::addressof(blockArg))) {
      auto block = m_explorer->queryBlockInfo(*hash);
      if (block.isError()) {
        printError(block.error().message());
      } else {
        XI_RETURN_EC_IF_NOT(printObject(*block, arg), false);
      }
    }
  }
  return true;
}

bool DaemonCommandsHandler::block_short(const std::vector<std::string>& args) {
  using namespace Xi::Blockchain::Explorer;

  DAEMON_COMMAND_ARGS_COND(args.size() > 0, "At least one argument expected.");

  for (const auto& arg : args) {
    auto blockArg = parseBlockSearchArgument(Xi::trim(arg));
    if (std::holds_alternative<CryptoNote::Null>(blockArg)) {
      std::string _{"invalid argument"};
      XI_RETURN_EC_IF_NOT(printObject(_, arg), false);
    } else if (const auto height = std::get_if<CryptoNote::BlockHeight>(std::addressof(blockArg))) {
      auto block = m_explorer->queryShortBlockInfo(*height);
      if (block.isError()) {
        printError(block.error().message());
      } else {
        XI_RETURN_EC_IF_NOT(printObject(*block, arg), false);
      }
    } else if (const auto hash = std::get_if<CryptoNote::BlockHash>(std::addressof(blockArg))) {
      auto block = m_explorer->queryShortBlockInfo(*hash);
      if (block.isError()) {
        printError(block.error().message());
      } else {
        XI_RETURN_EC_IF_NOT(printObject(*block, arg), false);
      }
    }
  }
  return true;
}

bool DaemonCommandsHandler::block_detailed(const std::vector<std::string>& args) {
  using namespace Xi::Blockchain::Explorer;

  DAEMON_COMMAND_ARGS_COND(args.size() > 0, "At least one argument expected.");

  for (const auto& arg : args) {
    auto blockArg = parseBlockSearchArgument(Xi::trim(arg));
    if (std::holds_alternative<CryptoNote::Null>(blockArg)) {
      std::string _{"invalid argument"};
      XI_RETURN_EC_IF_NOT(printObject(_, arg), false);
    } else if (const auto height = std::get_if<CryptoNote::BlockHeight>(std::addressof(blockArg))) {
      auto block = m_explorer->queryDetailedBlockInfo(*height);
      if (block.isError()) {
        printError(block.error().message());
      } else {
        XI_RETURN_EC_IF_NOT(printObject(*block, arg), false);
      }
    } else if (const auto hash = std::get_if<CryptoNote::BlockHash>(std::addressof(blockArg))) {
      auto block = m_explorer->queryDetailedBlockInfo(*hash);
      if (block.isError()) {
        printError(block.error().message());
      } else {
        XI_RETURN_EC_IF_NOT(printObject(*block, arg), false);
      }
    }
  }
  return true;
}

bool DaemonCommandsHandler::transaction(const std::vector<std::string>& args) {
  using namespace Xi::Blockchain::Explorer;

  DAEMON_COMMAND_ARGS_COND(args.size() > 0, "At least one argument expected.");

  for (const auto& arg : args) {
    if (const auto hash = CryptoNote::TransactionHash::fromString(arg); !hash.isError()) {
      auto transaction = m_explorer->queryTransactionInfo(*hash);
      if (transaction.isError()) {
        printError(transaction.error().message());
      } else {
        XI_RETURN_EC_IF_NOT(printObject(*transaction, arg), false);
      }
    } else {
      std::string _{"invalid argument: "};
      _ += hash.error().message();
      XI_RETURN_EC_IF_NOT(printObject(_, arg), false);
    }
  }
  return true;
}

bool DaemonCommandsHandler::transaction_short(const std::vector<std::string>& args) {
  using namespace Xi::Blockchain::Explorer;

  DAEMON_COMMAND_ARGS_COND(args.size() > 0, "At least one argument expected.");

  for (const auto& arg : args) {
    if (const auto hash = CryptoNote::TransactionHash::fromString(arg); !hash.isError()) {
      auto transaction = m_explorer->queryShortTransactionInfo(*hash);
      if (transaction.isError()) {
        printError(transaction.error().message());
      } else {
        XI_RETURN_EC_IF_NOT(printObject(*transaction, arg), false);
      }
    } else {
      std::string _{"invalid argument: "};
      _ += hash.error().message();
      XI_RETURN_EC_IF_NOT(printObject(_, arg), false);
    }
  }
  return true;
}

bool DaemonCommandsHandler::transaction_detailed(const std::vector<std::string>& args) {
  using namespace Xi::Blockchain::Explorer;

  DAEMON_COMMAND_ARGS_COND(args.size() > 0, "At least one argument expected.");

  for (const auto& arg : args) {
    if (const auto hash = CryptoNote::TransactionHash::fromString(arg); !hash.isError()) {
      auto transaction = m_explorer->queryDetailedTransactionInfo(*hash);
      if (transaction.isError()) {
        printError(transaction.error().message());
      } else {
        XI_RETURN_EC_IF_NOT(printObject(*transaction, arg), false);
      }
    } else {
      std::string _{"invalid argument: "};
      _ += hash.error().message();
      XI_RETURN_EC_IF_NOT(printObject(_, arg), false);
    }
  }
  return true;
}

bool DaemonCommandsHandler::p2p_peers(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  const auto peers = m_srv.peerlist();

  // clang-format off
  std::cout << rang::style::underline
    << std::left << std::setw(6)  << "Type"
    << std::left<< std::setw(2 * 8 + 2) << "Identifier"
    << std::left<< std::setw(15 + 2) << "Address"
    << std::left<< std::setw(5 + 2) << "Port"
    << std::left<< std::setw(24) << "Last Seen"
    << rang::style::reset
    << "\n"
  ;
  // clang-format on
  const auto printPeer = [](const CryptoNote::PeerlistEntry& peer) {
    // clang-format off
    std::cout
      << std::left << std::setw(2 * 8 + 2) << Common::toHex(&peer.id, sizeof(peer.id))
      << std::left << std::setw(15 + 2) << Common::ipAddressToString(peer.address.ip)
      << std::left << std::setw(5 + 2) << peer.address.port
      << std::left << std::setw(24) << Xi::Time::unixToLocalShortString(peer.last_seen)
      << "\n"
    ;
    // clang-format on
  };

  for (const auto& peer : peers.white) {
    std::cout << std::left << std::setw(6) << "WHITE";
    printPeer(peer);
  };
  std::cout << rang::fg::gray;
  for (const auto& peer : peers.gray) {
    std::cout << std::left << std::setw(6) << "GRAY";
    printPeer(peer);
  };
  std::cout << rang::fg::reset;
  std::cout << std::endl;

  return true;
}

bool DaemonCommandsHandler::p2p_connections(const std::vector<std::string>& args) {
  using CryptoNote::P2pConnectionInfo;

  XI_UNUSED(args);
  const auto connections = m_srv.connections();

  // clang-format off
  std::cout << rang::style::underline
    << std::left << std::setw(10)  << "Source"
    << std::left<< std::setw(2 * 8 + 2) << "Identifier"
    << std::left<< std::setw(2 * 8 + 2) << "Connection"
    << std::left<< std::setw(15 + 2) << "Address"
    << std::left<< std::setw(5 + 2) << "Port"
    << std::left<< std::setw(10 + 2) << "Height"
    << std::left<< std::setw(8 + 2) << "Type"
    << rang::style::reset
    << "\n"
  ;

  for (const auto& connection : connections) {
    // clang-format off
    std::cout
      << std::left << std::setw(10) << (connection.source == P2pConnectionInfo::Incoming ? "INCOMING" : "OUTGOING")
      << std::left << std::setw(2 * 8 + 2) << Common::toHex(&connection.id, sizeof(connection.id))
      << std::left << std::setw(2 * 8 + 2) << Common::toHex(connection.connectionId.data, 8)
      << std::left << std::setw(15 + 2) << Common::ipAddressToString(connection.address.ip)
      << std::left << std::setw(5 + 2) << connection.address.port
      << std::left << std::setw(10 + 2) << toString(connection.height)
      << std::left << std::setw(8 + 2) << (connection.type == P2pConnectionInfo::LightNode ? "LIGHT" : "FULL")
      << "\n"
    ;
    // clang-format on
  };
  std::cout << rang::fg::reset;
  std::cout << std::endl;

  return true;
}

bool DaemonCommandsHandler::p2p_ban_list(const std::vector<std::string>& args) {
  DAEMON_COMMAND_EXPECTED_ARGS(0, "No argument expected.");
  const auto banList = m_srv.blockedPeers();
  if (banList.empty()) {
    std::cout << "No banned peers." << std::endl;
  } else {
    std::cout << "Banned Peers";
    for (const auto& bannedPeer : banList) {
      time_t banTimestamp = bannedPeer.second;
      std::cout << "\n\t" << Common::ipAddressToString(bannedPeer.first) << "\t" << Xi::to_string(banTimestamp);
    }
    std::cout << std::endl;
  }
  return true;
}

bool DaemonCommandsHandler::p2p_penalty_list(const std::vector<std::string>& args) {
  DAEMON_COMMAND_EXPECTED_ARGS(0, "No argument expected.");
  const auto penaltyList = m_srv.peerPenalties();
  if (penaltyList.empty()) {
    std::cout << "No penalties on peers." << std::endl;
  } else {
    std::cout << "Peer Penalties";
    for (const auto& peerPanilty : penaltyList) {
      std::cout << "\n\t" << Common::ipAddressToString(peerPanilty.first) << "\t" << std::to_string(peerPanilty.second);
    }
    std::cout << std::endl;
  }
  return true;
}

bool DaemonCommandsHandler::p2p_penalty_reset(const std::vector<std::string>& args) {
  DAEMON_COMMAND_EXPECTED_ARGS(0, "No argument expected.");

  const size_t removedPenalties = m_srv.resetPenalties();
  std::cout << removedPenalties << " penalties removed." << std::endl;

  return true;
}

bool DaemonCommandsHandler::p2p_ban_ip(const std::vector<std::string>& args) {
  if (args.empty()) {
    std::cout << "Expected at least one argument." << std::endl;
    return false;
  }

  const auto ips = parseIps(args);
  if (ips.size() < args.size()) {
    std::cout << "Command aborted." << std::endl;
    return false;
  }

  const auto newBansCount = m_srv.banIps(ips);
  std::cout << newBansCount << " IP(s) added to ban list and " << (ips.size() - newBansCount) << " is/are renewed."
            << std::endl;

  return true;
}

bool DaemonCommandsHandler::p2p_unban_ip(const std::vector<std::string>& args) {
  if (args.empty()) {
    std::cout << "Expected at least one argument." << std::endl;
    return false;
  }

  const auto ips = parseIps(args);
  if (ips.size() < args.size()) {
    std::cout << "Command aborted." << std::endl;
    return false;
  }

  const auto unbannedCount = m_srv.unbanIps(ips);
  std::cout << unbannedCount << " IP(s) successfully unbanned." << std::endl;

  return true;
}

bool DaemonCommandsHandler::p2p_unban_all(const std::vector<std::string>& args) {
  DAEMON_COMMAND_EXPECTED_ARGS(0, "No argument expected.");

  const auto unbannedCount = m_srv.unbanAllIps();
  std::cout << unbannedCount << " IP(s) successfully unbanned." << std::endl;

  return true;
}

std::vector<uint32_t> DaemonCommandsHandler::parseIps(const std::vector<std::string>& args) {
  std::vector<uint32_t> ips;
  ips.reserve(args.size());
  for (const auto& ipArg : args) {
    uint32_t ip;
    if (!Common::parseIpAddress(ip, ipArg)) {
      std::cout << "Invalid IP: " << ipArg << std::endl;
    } else {
      ips.push_back(ip);
    }
  }
  return ips;
}
