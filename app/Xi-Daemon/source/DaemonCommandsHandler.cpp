// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "DaemonCommandsHandler.h"

#include <ctime>
#include <chrono>

#include <Xi/Global.hh>
#include <Xi/Algorithm/String.h>
#include <Xi/Version/Version.h>
#include <CommonCLI/CommonCLI.h>

#include "P2p/NetNode.h"
#include "CryptoNoteCore/Core.h"
#include "CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "Serialization/SerializationTools.h"

#include "Rpc/JsonRpc.h"
#include "CryptoNoteCore/Currency.h"
#include <boost/format.hpp>
#include "Common/FormatTools.h"
#include "Common/StringTools.h"

namespace {
template <typename T>
static bool print_as_json(const T& obj) {
  std::cout << CryptoNote::storeToJson(obj) << ENDL;
  return true;
}

std::string printTransactionShortInfo(const CryptoNote::CachedTransaction& transaction) {
  std::stringstream ss;

  ss << "id: " << transaction.getTransactionHash() << std::endl;
  ss << "fee: " << transaction.getTransactionFee() << std::endl;
  ss << "blobSize: " << transaction.getTransactionBinaryArray().size() << std::endl;

  return ss.str();
}

std::string printTransactionFullInfo(const CryptoNote::CachedTransaction& transaction) {
  std::stringstream ss;
  ss << printTransactionShortInfo(transaction);
  ss << "JSON: \n" << CryptoNote::storeToJson(transaction.getTransaction()) << std::endl;

  return ss.str();
}

}  // namespace

#define DAEMON_COMMAND_DEFINE(NAME, HELP) \
  m_consoleHandler.setHandler(#NAME, boost::bind(&DaemonCommandsHandler::NAME, this, _1), HELP)

DaemonCommandsHandler::DaemonCommandsHandler(CryptoNote::Core& core, CryptoNote::NodeServer& srv,
                                             Logging::LoggerManager& log, CryptoNote::RpcServer* prpc_server)
    : m_core(core), m_srv(srv), logger(log, "daemon"), m_logManager(log), m_prpc_server(prpc_server) {
  DAEMON_COMMAND_DEFINE(exit, "Shutdown the daemon");
  DAEMON_COMMAND_DEFINE(help, "Show this help");
  DAEMON_COMMAND_DEFINE(version, "Shows version information about the running daemon");
  DAEMON_COMMAND_DEFINE(print_pl, "Print peer list");
  DAEMON_COMMAND_DEFINE(print_cn, "Print connections");
  DAEMON_COMMAND_DEFINE(print_bc,
                        "Print blockchain info in a given blocks range, print_bc <begin_height> [<end_height>]");
  DAEMON_COMMAND_DEFINE(print_block, "Print block, print_block <block_hash> | <block_height>");
  DAEMON_COMMAND_DEFINE(print_tx, "Print transaction, print_tx <transaction_hash>");
  DAEMON_COMMAND_DEFINE(set_log, "set_log <level> - Change current log level, <level> is a number 0-5");
  DAEMON_COMMAND_DEFINE(status, "Show daemon status");

  /* ------------------------------------------------- Pool Commands ----------------------------------------------- */
  DAEMON_COMMAND_DEFINE(print_pool, "Print transaction pool (long format)");
  DAEMON_COMMAND_DEFINE(print_pool_sh, "Print transaction pool (short format)");
  DAEMON_COMMAND_DEFINE(pool_flush, "Removes all pool transactions");
  DAEMON_COMMAND_DEFINE(pool_remove, "Removes a transaction with given hash from the pool <transaction_hash>");

  /* -------------------------------------------------- P2P Commands ----------------------------------------------- */
  DAEMON_COMMAND_DEFINE(p2p_ban_list, "Lists all currently banned peers.");
  DAEMON_COMMAND_DEFINE(p2p_penalty_list, "Lists all peers with penalities.");
  DAEMON_COMMAND_DEFINE(p2p_ban_ip, "Adds given ips to the ban list.  '<ip> [<ip> ]*'");
  DAEMON_COMMAND_DEFINE(p2p_unban_ip, "Removes given ips from the ban list. '<ip> [<ip> ]*'");
  DAEMON_COMMAND_DEFINE(p2p_unban_all, "Removes all banned peers from the ban list.");
}

#undef DAEMON_COMMAND_DEFINE

#define DAEMON_COMMAND_EXPECTED_ARGS(NUM, HELP)             \
  do {                                                      \
    if (args.size() != NUM) {                               \
      std::cout << "Command failed: " << HELP << std::endl; \
      return false;                                         \
    }                                                       \
  } while (false)

//--------------------------------------------------------------------------------
std::string DaemonCommandsHandler::get_commands_str() {
  std::stringstream ss;
  ss << Xi::Config::Coin::name() << " v" << APP_VERSION << ENDL;
  ss << "Commands: " << ENDL;
  std::string usage = m_consoleHandler.getUsage();
  boost::replace_all(usage, "\n", "\n  ");
  usage.insert(0, "  ");
  ss << usage << ENDL;
  return ss.str();
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::exit(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  m_consoleHandler.requestStop();
  m_srv.sendStopSignal();
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::help(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  std::cout << get_commands_str() << ENDL;
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::version(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  std::cout << CommonCLI::verboseVersionInformation() << ENDL;
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pl(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  m_srv.log_peerlist();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_cn(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  m_srv.get_payload_object().log_connections();
  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_bc(const std::vector<std::string>& args) {
  if (!args.size()) {
    std::cout << "need block index parameter" << ENDL;
    return false;
  }

  uint32_t start_index = 0;
  uint32_t end_index = 0;
  uint32_t end_block_parametr = m_core.getTopBlockIndex();

  if (!Common::fromString(args[0], start_index)) {
    std::cout << "wrong starter block index parameter" << ENDL;
    return false;
  }

  if (args.size() > 1 && !Common::fromString(args[1], end_index)) {
    std::cout << "wrong end block index parameter" << ENDL;
    return false;
  }

  if (end_index == 0) end_index = start_index;

  if (end_index > end_block_parametr) {
    std::cout << "end block index parameter shouldn't be greater than " << end_block_parametr << ENDL;
    return false;
  }

  if (end_index < start_index) {
    std::cout << "end block index should be greater than or equal to starter block index" << ENDL;
    return false;
  }

  CryptoNote::COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::request req;
  CryptoNote::COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::response res;
  CryptoNote::JsonRpc::JsonRpcError error_resp;

  req.start_height = start_index + 1;
  req.end_height = end_index + 1;

  if (!m_prpc_server->on_get_block_headers_range(req, res, error_resp) || res.status != CORE_RPC_STATUS_OK) {
    std::cout << "Response status was expected to be '" << CORE_RPC_STATUS_OK "' actually is '" << res.status << "'."
              << ENDL;
    return false;
  }

  const CryptoNote::Currency& currency = m_core.getCurrency();

  bool first = true;
  for (CryptoNote::block_header_response& header : res.headers) {
    if (!first) {
      std::cout << ENDL;
      first = false;
    }

    std::cout << "height: " << header.height << ", timestamp: " << header.timestamp
              << ", difficulty: " << header.difficulty << ", size: " << header.block_size
              << ", transactions: " << header.num_txes << ENDL << "major version: " << unsigned(header.major_version)
              << ", minor version: " << unsigned(header.minor_version) << ENDL << "block id: " << header.hash
              << ", previous block id: " << header.prev_hash << ENDL << "difficulty: " << header.difficulty
              << ", nonce: " << header.nonce << ", reward: " << currency.formatAmount(header.reward) << ENDL;
  }

  return true;
}

bool DaemonCommandsHandler::set_log(const std::vector<std::string>& args) {
  if (args.size() != 1) {
    std::cout << "use: set_log <log_level_number_0-4>" << ENDL;
    return true;
  }

  uint16_t l = 0;
  if (!Common::fromString(args[0], l)) {
    std::cout << "wrong number format, use: set_log <log_level_number_0-4>" << ENDL;
    return true;
  }

  ++l;

  if (l > Logging::TRACE) {
    std::cout << "wrong number range, use: set_log <log_level_number_0-4>" << ENDL;
    return true;
  }

  m_logManager.setMaxLevel(static_cast<Logging::Level>(l));
  return true;
}

//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_block_by_height(uint32_t height) {
  if (height - 1 > m_core.getTopBlockIndex()) {
    std::cout << "block wasn't found. Current block chain height: " << m_core.getTopBlockIndex() + 1
              << ", requested: " << height << std::endl;
    return false;
  }

  auto hash = m_core.getBlockHashByIndex(height - 1);
  std::cout << "block_id: " << hash << ENDL;
  print_as_json(m_core.getBlockByIndex(height - 1));

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_block_by_hash(const std::string& arg) {
  Crypto::Hash block_hash;
  if (!parse_hash256(arg, block_hash)) {
    return false;
  }

  if (m_core.hasBlock(block_hash)) {
    print_as_json(m_core.getBlockByHash(block_hash));
  } else {
    std::cout << "block wasn't found: " << arg << std::endl;
    return false;
  }

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_block(const std::vector<std::string>& args) {
  if (args.empty()) {
    std::cout << "expected: print_block (<block_hash> | <block_height>)" << std::endl;
    return true;
  }

  const std::string& arg = args.front();
  try {
    uint32_t height = boost::lexical_cast<uint32_t>(arg);
    print_block_by_height(height);
  } catch (boost::bad_lexical_cast&) {
    print_block_by_hash(arg);
  }

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_tx(const std::vector<std::string>& args) {
  if (args.empty()) {
    std::cout << "expected: print_tx <transaction hash>" << std::endl;
    return true;
  }

  const std::string& str_hash = args.front();
  Crypto::Hash tx_hash;
  if (!parse_hash256(str_hash, tx_hash)) {
    return true;
  }

  std::vector<Crypto::Hash> tx_ids;
  tx_ids.push_back(tx_hash);
  std::vector<CryptoNote::BinaryArray> txs;
  std::vector<Crypto::Hash> missed_ids;
  m_core.getTransactions(tx_ids, txs, missed_ids);

  if (1 == txs.size()) {
    CryptoNote::CachedTransaction tx(txs.front());
    print_as_json(tx.getTransaction());
  } else {
    std::cout << "transaction wasn't found: <" << str_hash << '>' << std::endl;
  }

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pool(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  auto txpoolHash = m_core.transactionPool().stateHash();
  std::cout << "Pool state (" << txpoolHash.toString() << "): \n";
  auto pool = m_core.getPoolTransactions();

  for (const auto& tx : pool) {
    CryptoNote::CachedTransaction ctx(tx);
    std::cout << printTransactionFullInfo(ctx) << "\n";
  }

  std::cout << std::endl;

  return true;
}
//--------------------------------------------------------------------------------
bool DaemonCommandsHandler::print_pool_sh(const std::vector<std::string>& args) {
  XI_UNUSED(args);
  std::cout << "Pool short state: \n";
  auto pool = m_core.getPoolTransactions();

  for (const auto& tx : pool) {
    CryptoNote::CachedTransaction ctx(tx);
    std::cout << printTransactionShortInfo(ctx) << "\n";
  }

  std::cout << std::endl;

  return true;
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
  CryptoNote::COMMAND_RPC_GET_INFO::request ireq;
  CryptoNote::COMMAND_RPC_GET_INFO::response iresp;

  if (!m_prpc_server->on_get_info(ireq, iresp) || iresp.status != CORE_RPC_STATUS_OK) {
    std::cout << "Problem retrieving information from RPC server." << std::endl;
    return false;
  }

  std::cout << Common::get_status_string(iresp) << std::endl;

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
