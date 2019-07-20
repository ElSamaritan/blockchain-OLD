// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Karai Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

#include <Xi/Concurrent/RecursiveLock.h>
#include <Xi/TypeSafe/Flag.hpp>
#include <Xi/Crypto/PasswordContainer.h>
#include <Xi/Concurrent/ReadersWriterLock.h>
#include <Xi/Http/RequestHandler.h>
#include <Xi/Http/Server.h>
#include <Xi/Http/Router.h>
#include <Xi/Rpc/ServiceProviderCollection.hpp>
#include <Xi/Rpc/ServiceRouter.hpp>
#include <Xi/Rpc/JsonProviderEndpoint.hpp>
#include <Xi/Blockchain/Explorer/IExplorer.hpp>
#include <Xi/Blockchain/Services/BlockExplorer/BlockExplorer.hpp>

#include <Logging/LoggerRef.h>
#include <CryptoNoteCore/Currency.h>
#include "Common/Math.h"
#include "CoreRpcServerCommandsDefinitions.h"
#include "JsonRpc.h"

#include "Rpc/Commands/Commands.h"

namespace CryptoNote {

class Core;
class NodeServer;
struct ICryptoNoteProtocolHandler;

/// Encodes a specific service being responsible to process a request
enum struct RpcServiceType {
  BlockExplorer = 1 << 0,
};
XI_TYPESAFE_FLAG_MAKE_OPERATIONS(RpcServiceType)

class RpcServer : public Xi::Http::Server, public Xi::Http::RequestHandler {
  using HttpRequest = Xi::Http::Request;
  using HttpResponse = Xi::Http::Response;

 public:
  RpcServer(System::Dispatcher& dispatcher, Logging::ILogger& log, Core& c, NodeServer& p2p,
            ICryptoNoteProtocolHandler& protocol);
  virtual ~RpcServer() override = default;

  typedef std::function<bool(RpcServer*, const HttpRequest& request, HttpResponse& response)> HandlerFunction;
  bool enableCors(const std::string& domain);

  bool setFeeAddress(const std::string fee_address);
  bool setFeeViewKey(const std::string viewKey);
  bool setFeeAmount(const uint64_t fee_amount);

  void setAccessToken(const std::string& access_token);

  const std::string& getCorsDomain();

  bool isBlockexplorer() const;
  void setBlockexplorer(bool enabled);

  bool isBlockexplorerOnly() const;
  void setBlockexplorerOnly(bool enabled);

  bool on_get_block_headers_range(const COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::request& req,
                                  COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::response& res,
                                  JsonRpc::JsonRpcError& error_resp);
  bool on_get_info(const COMMAND_RPC_GET_INFO::request& req, COMMAND_RPC_GET_INFO::response& res);

 private:
  const Currency& currency() const;

  template <class Handler>
  struct RpcHandler {
    const Handler handler;
    const bool allowBusyCore;
    const bool isBlockexplorerRequest;
  };

  typedef void (RpcServer::*HandlerPtr)(const HttpRequest& request, HttpResponse& response);
  static std::unordered_map<std::string, RpcHandler<HandlerFunction>> s_handlers;

  Xi::Http::Response doHandleRequest(const Xi::Http::Request& request) override;
  bool processJsonRpcRequest(const HttpRequest& request, HttpResponse& response);
  bool isCoreReady();

  /*!
   * \brief on_options_request sets standard headers for incoming requests
   * \param request The request coming in
   * \param response The response to built and send back to the client
   * \return true if the request was a purly OPTION request and shouldnt be handled further
   */
  bool on_options_request(const HttpRequest& request, HttpResponse& response);

  // json handlers
  bool on_get_blocks(const COMMAND_RPC_GET_BLOCKS_FAST::request& req, COMMAND_RPC_GET_BLOCKS_FAST::response& res);
  bool on_query_blocks(const COMMAND_RPC_QUERY_BLOCKS::request& req, COMMAND_RPC_QUERY_BLOCKS::response& res);
  bool on_query_blocks_lite(const COMMAND_RPC_QUERY_BLOCKS_LITE::request& req,
                            COMMAND_RPC_QUERY_BLOCKS_LITE::response& res);
  bool on_query_blocks_detailed(const COMMAND_RPC_QUERY_BLOCKS_DETAILED::request& req,
                                COMMAND_RPC_QUERY_BLOCKS_DETAILED::response& res);
  bool on_get_indexes(const COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::request& req,
                      COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::response& res);
  bool on_get_random_outs(const COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::request& req,
                          COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::response& res);
  bool on_get_mixins_required(const COMMAND_RPC_GET_REQUIRED_MIXIN_FOR_AMOUNTS::request& req,
                              COMMAND_RPC_GET_REQUIRED_MIXIN_FOR_AMOUNTS::response& res);
  bool onGetPoolChanges(const COMMAND_RPC_GET_POOL_CHANGES::request& req, COMMAND_RPC_GET_POOL_CHANGES::response& rsp);
  bool onGetPoolChangesLite(const COMMAND_RPC_GET_POOL_CHANGES_LITE::request& req,
                            COMMAND_RPC_GET_POOL_CHANGES_LITE::response& rsp);
  bool onGetBlocksDetailsByHeights(const COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS::request& req,
                                   COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS::response& rsp);
  bool onGetBlocksDetailsByHashes(const COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::request& req,
                                  COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::response& rsp);
  bool onGetBlockDetailsByHeight(const COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::request& req,
                                 COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::response& rsp);
  bool onGetBlocksHashesByTimestamps(const COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::request& req,
                                     COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::response& rsp);
  bool onGetTransactionDetailsByHashes(const COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::request& req,
                                       COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::response& rsp);
  bool onGetTransactionHashesByPaymentId(const COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::request& req,
                                         COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::response& rsp);
  bool on_get_height(const COMMAND_RPC_GET_HEIGHT::request& req, COMMAND_RPC_GET_HEIGHT::response& res);
  bool on_get_transactions(const COMMAND_RPC_GET_TRANSACTIONS::request& req,
                           COMMAND_RPC_GET_TRANSACTIONS::response& res);
  bool on_send_raw_tx(const COMMAND_RPC_SEND_RAW_TX::request& req, COMMAND_RPC_SEND_RAW_TX::response& res);
  bool on_get_fee_info(const COMMAND_RPC_GET_FEE_ADDRESS::request& req, COMMAND_RPC_GET_FEE_ADDRESS::response& res);
  bool on_get_peers(const COMMAND_RPC_GET_PEERS::request& req, COMMAND_RPC_GET_PEERS::response& res);

  // json rpc
  bool on_getblockcount(const COMMAND_RPC_GETBLOCKCOUNT::request& req, COMMAND_RPC_GETBLOCKCOUNT::response& res);
  bool on_getblockhash(const COMMAND_RPC_GETBLOCKHASH::request& req, COMMAND_RPC_GETBLOCKHASH::response& res);

  Crypto::Hash block_template_state_hash() const;
  bool on_getblocktemplatestate(const COMMAND_RPC_GETBLOCKTEMPLATE_STATE::request& req,
                                COMMAND_RPC_GETBLOCKTEMPLATE_STATE::response& res);
  bool on_getblocktemplate(const COMMAND_RPC_GETBLOCKTEMPLATE::request& req,
                           COMMAND_RPC_GETBLOCKTEMPLATE::response& res);

  bool on_get_block_template_state(const RpcCommands::GetBlockTemplateState::request& req,
                                   RpcCommands::GetBlockTemplateState::response& res);
  bool on_get_block_template(const RpcCommands::GetBlockTemplate::request& req,
                             RpcCommands::GetBlockTemplate::response& res);

  bool on_submit_block(const RpcCommands::SubmitBlock::request& req, RpcCommands::SubmitBlock::response& res);

  bool on_get_currency_id(const COMMAND_RPC_GET_CURRENCY_ID::request& req, COMMAND_RPC_GET_CURRENCY_ID::response& res);
  bool on_submitblock(const COMMAND_RPC_SUBMITBLOCK::request& req, COMMAND_RPC_SUBMITBLOCK::response& res);
  bool on_get_last_block_header(const COMMAND_RPC_GET_LAST_BLOCK_HEADER::request& req,
                                COMMAND_RPC_GET_LAST_BLOCK_HEADER::response& res);
  bool on_get_block_header_by_hash(const COMMAND_RPC_GET_BLOCK_HEADER_BY_HASH::request& req,
                                   COMMAND_RPC_GET_BLOCK_HEADER_BY_HASH::response& res);
  bool on_get_block_header_by_height(const COMMAND_RPC_GET_BLOCK_HEADER_BY_HEIGHT::request& req,
                                     COMMAND_RPC_GET_BLOCK_HEADER_BY_HEIGHT::response& res);

  void fill_block_header_response(const BlockTemplate& blk, bool orphan_status, BlockHeight height,
                                  const Crypto::Hash& hash, block_header_response& responce);
  RawBlock prepareRawBlock(BinaryArray&& blockBlob);

  bool f_on_blocks_list_json(const F_COMMAND_RPC_GET_BLOCKS_LIST::request& req,
                             F_COMMAND_RPC_GET_BLOCKS_LIST::response& res);
  bool f_on_block_json(const F_COMMAND_RPC_GET_BLOCK_DETAILS::request& req,
                       F_COMMAND_RPC_GET_BLOCK_DETAILS::response& res);
  bool f_on_blocks_list_raw(const F_COMMAND_RPC_GET_BLOCKS_RAW_BY_RANGE::request& req,
                            F_COMMAND_RPC_GET_BLOCKS_RAW_BY_RANGE::response& res);
  bool f_on_transaction_json(const F_COMMAND_RPC_GET_TRANSACTION_DETAILS::request& req,
                             F_COMMAND_RPC_GET_TRANSACTION_DETAILS::response& res);
  bool f_on_transactions_pool_json(const F_COMMAND_RPC_GET_POOL::request& req, F_COMMAND_RPC_GET_POOL::response& res);
  bool f_on_p2p_ban_info(const F_COMMAND_RPC_GET_P2P_BAN_INFO::request& req,
                         F_COMMAND_RPC_GET_P2P_BAN_INFO::response& res);
  bool f_getMixin(const Transaction& transaction, uint64_t& mixin);

 private:
  void checkService(RpcServiceType type) const;
  void notFound(const std::string& resource, const std::string& id) const;

  Logging::LoggerRef logger;
  Core& m_core;
  NodeServer& m_p2p;
  ICryptoNoteProtocolHandler& m_protocol;
  std::string m_cors;

  std::optional<AccountPublicAddress> m_fee_address = std::nullopt;
  std::optional<Crypto::SecretKey> m_fee_view_key = std::nullopt;
  uint64_t m_fee_amount = 0;

  std::unique_ptr<Xi::Crypto::PasswordContainer> m_access_token{};
  Xi::Concurrent::ReadersWriterLock m_access_token_guard{};

  bool m_isBlockexplorer;
  bool m_isBlockexplorerOnly;
  Xi::Concurrent::RecursiveLock m_submissionAccess;

  std::shared_ptr<Xi::Blockchain::Explorer::IExplorer> m_explorer;
  std::shared_ptr<Xi::Blockchain::Services::BlockExplorer::BlockExplorer> m_explorerService;
  std::shared_ptr<Xi::Rpc::JsonProviderEndpoint> m_explorerEndpoint;
};

}  // namespace CryptoNote
