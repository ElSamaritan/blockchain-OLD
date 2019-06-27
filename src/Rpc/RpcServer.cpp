// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Karai Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "RpcServer.h"

#include <future>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <ctime>

#include <fmt/format.h>

#include <Xi/Version/Version.h>
#include <Xi/Global.hh>
#include <Xi/Concurrent/SystemDispatcher.h>
#include <Xi/Blockchain/Explorer/CoreExplorer.hpp>

// CryptoNote
#include "Common/StringTools.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/Core.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"
#include <Xi/Config.h>
#include "CryptoNoteProtocol/CryptoNoteProtocolHandlerCommon.h"
#include "P2p/NetNode.h"
#include "P2p/P2pProtocolTypes.h"
#include "CoreRpcServerErrorCodes.h"
#include "JsonRpc.h"

#undef ERROR

using namespace Logging;
using namespace Crypto;
using namespace Common;

namespace CryptoNote {

static inline bool serialize(COMMAND_RPC_GET_BLOCKS_FAST::response& response, ISerializer& s) {
  KV_MEMBER_RENAME(response.blocks, blocks)
  KV_MEMBER_RENAME(response.start_height, start_height)
  KV_MEMBER_RENAME(response.current_height, current_height)
  KV_MEMBER_RENAME(response.status, status)
  return true;
}

[[nodiscard]] bool serialize(BlockFullInfo& blockFullInfo, ISerializer& s) {
  KV_MEMBER_RENAME(blockFullInfo.block_id, block_hash);
  KV_MEMBER_RENAME(blockFullInfo.blockTemplate, block);
  KV_MEMBER_RENAME(blockFullInfo.transactions, transactions);
  return true;
}

[[nodiscard]] bool serialize(TransactionPrefixInfo& transactionPrefixInfo, ISerializer& s) {
  KV_MEMBER_RENAME(transactionPrefixInfo.hash, hash);
  KV_MEMBER_RENAME(transactionPrefixInfo.prefix, prefix);
  return true;
}

[[nodiscard]] bool serialize(BlockShortInfo& blockShortInfo, ISerializer& s) {
  KV_MEMBER_RENAME(blockShortInfo.block_hash, block_hash);
  XI_RETURN_EC_IF_NOT(s.binary(blockShortInfo.block, "block_short_info"), false);
  KV_MEMBER_RENAME(blockShortInfo.transaction_prefixes, transaction_prefixes);
  return true;
}

namespace {

template <typename Command>
RpcServer::HandlerFunction jsonMethod(bool (RpcServer::*handler)(typename Command::request const&,
                                                                 typename Command::response&)) {
  return [handler](RpcServer* obj, const Xi::Http::Request& request, Xi::Http::Response& response) {
    boost::value_initialized<typename Command::request> req;
    boost::value_initialized<typename Command::response> res;

    if constexpr (!std::is_same_v<CryptoNote::Null, typename Command::request>) {
      if (!loadFromJson(static_cast<typename Command::request&>(req), request.body())) {
        return false;
      }
    }

    if (!obj->getCorsDomain().empty()) {
      response.headers().set(Xi::Http::HeaderContainer::AccessControlAllowOrigin, obj->getCorsDomain());
    }
    response.headers().setContentType(Xi::Http::ContentType::Json);

    if (request.method() != Xi::Http::Method::Post && request.method() != Xi::Http::Method::Get) {
      response = obj->makeBadRequest("Only OPTIONS, POST and GET requests are allowed.");
      return false;
    }

    auto result = (obj->*handler)(req, res);
    response.headers().setContentType(Xi::Http::ContentType::Json);
    if constexpr (!std::is_same_v<CryptoNote::Null, typename Command::response>) {
      response.setBody(storeToJson(res.data()));
    }
    return result;
  };
}

}  // namespace

std::unordered_map<std::string, RpcServer::RpcHandler<RpcServer::HandlerFunction>> RpcServer::s_handlers = {
    // json handlers
    // Enabled on block explorer
    {"/getinfo", {jsonMethod<COMMAND_RPC_GET_INFO>(&RpcServer::on_get_info), true, true}},
    {"/getheight", {jsonMethod<COMMAND_RPC_GET_HEIGHT>(&RpcServer::on_get_height), true, true}},
    {"/gettransactions", {jsonMethod<COMMAND_RPC_GET_TRANSACTIONS>(&RpcServer::on_get_transactions), false, true}},
    {"/getpeers", {jsonMethod<COMMAND_RPC_GET_PEERS>(&RpcServer::on_get_peers), true, true}},
    {"/getblocks", {jsonMethod<COMMAND_RPC_GET_BLOCKS_FAST>(&RpcServer::on_get_blocks), false, true}},
    {"/queryblocks", {jsonMethod<COMMAND_RPC_QUERY_BLOCKS>(&RpcServer::on_query_blocks), false, true}},
    {"/queryblockslite", {jsonMethod<COMMAND_RPC_QUERY_BLOCKS_LITE>(&RpcServer::on_query_blocks_lite), false, true}},
    {"/queryblocksdetailed",
     {jsonMethod<COMMAND_RPC_QUERY_BLOCKS_DETAILED>(&RpcServer::on_query_blocks_detailed), false, true}},
    {"/get_pool_changes", {jsonMethod<COMMAND_RPC_GET_POOL_CHANGES>(&RpcServer::onGetPoolChanges), false, true}},
    {"/get_pool_changes_lite",
     {jsonMethod<COMMAND_RPC_GET_POOL_CHANGES_LITE>(&RpcServer::onGetPoolChangesLite), false, true}},
    {"/get_block_details_by_height",
     {jsonMethod<COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT>(&RpcServer::onGetBlockDetailsByHeight), false, true}},
    {"/get_blocks_details_by_heights",
     {jsonMethod<COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS>(&RpcServer::onGetBlocksDetailsByHeights), false, true}},
    {"/get_blocks_details_by_hashes",
     {jsonMethod<COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES>(&RpcServer::onGetBlocksDetailsByHashes), false, true}},
    {"/get_blocks_hashes_by_timestamps",
     {jsonMethod<COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS>(&RpcServer::onGetBlocksHashesByTimestamps), false, true}},
    {"/get_transaction_details_by_hashes",
     {jsonMethod<COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES>(&RpcServer::onGetTransactionDetailsByHashes), false,
      true}},
    {"/get_transaction_hashes_by_payment_id",
     {jsonMethod<COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID>(&RpcServer::onGetTransactionHashesByPaymentId),
      false, true}},

    // remove me in 2019
    // Not enabled on block explorer
    {"/sendrawtransaction", {jsonMethod<COMMAND_RPC_SEND_RAW_TX>(&RpcServer::on_send_raw_tx), false, false}},
    {"/feeinfo", {jsonMethod<COMMAND_RPC_GET_FEE_ADDRESS>(&RpcServer::on_get_fee_info), true, false}},
    {"/getNodeFeeInfo", {jsonMethod<COMMAND_RPC_GET_FEE_ADDRESS>(&RpcServer::on_get_fee_info), true, false}},
    {"/get_o_indexes",
     {jsonMethod<COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES>(&RpcServer::on_get_indexes), false, false}},
    {"/getrandom_outs",
     {jsonMethod<COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS>(&RpcServer::on_get_random_outs), false, false}},

    // json rpc
    {"/json_rpc",
     {std::bind(&RpcServer::processJsonRpcRequest, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
      true, true}}};

RpcServer::RpcServer(System::Dispatcher& dispatcher, Logging::ILogger& log, Core& c, NodeServer& p2p,
                     ICryptoNoteProtocolHandler& protocol)
    : Xi::Http::Server(),
      logger(log, "RpcServer"),
      m_core(c),
      m_p2p(p2p),
      m_protocol(protocol),
      m_isBlockexplorer{false},
      m_isBlockexplorerOnly{false},
      m_submissionAccess{} {
  setDispatcher(std::make_shared<Xi::Concurrent::SystemDispatcher>(dispatcher));

  m_explorer = std::make_shared<Xi::Blockchain::Explorer::CoreExplorer>(c);
  m_explorerService = Xi::Blockchain::Services::BlockExplorer::BlockExplorer::create(m_explorer, log);
  m_explorerService->setPrefix("explorer");
  m_explorerEndpoint = std::make_shared<Xi::Rpc::JsonProviderEndpoint>(
      std::static_pointer_cast<Xi::Rpc::ServiceProviderCollection>(m_explorerService));
}

Xi::Http::Response RpcServer::doHandleRequest(const Xi::Http::Request& request) {
  if (request.target() == "/rpc") {
    if (!m_isBlockexplorer) {
      return makeNotFound("endpoint disabled");
    }
    auto reval = (*m_explorerEndpoint)(request);
    if (!m_cors.empty()) {
      reval.headers().set(Xi::Http::HeaderContainer::AccessControlAllowOrigin, m_cors);
    }
    return reval;
  }

  auto it = s_handlers.find(request.target());
  if (it == s_handlers.end())
    return makeNotFound();
  else if (!it->second.allowBusyCore && !isCoreReady())
    return makeInternalServerError("core is busy");
  else if (isBlockexplorerOnly() && !it->second.isBlockexplorerRequest) {
    return makeBadRequest("only block explorer requests are allowed");
  } else {
    Xi::Http::Response response;
    if (!on_options_request(request, response)) {
      if (request.method() != Xi::Http::Method::Post && request.method() != Xi::Http::Method::Get)
        return makeBadRequest("Only OPTIONS, GET and POST methods are allowed.");
      it->second.handler(this, request, response);
    }
    return response;
  }
}

bool RpcServer::processJsonRpcRequest(const HttpRequest& request, HttpResponse& response) {
  using namespace JsonRpc;

  response.headers().setContentType(Xi::Http::ContentType::Json);

  JsonRpcRequest jsonRequest;
  JsonRpcResponse jsonResponse;

  try {
    logger(TRACE) << "JSON-RPC request: " << request.body();
    jsonRequest.parseRequest(request.body());
    jsonResponse.setId(jsonRequest.getId());  // copy id

    static std::unordered_map<std::string, RpcServer::RpcHandler<JsonMemberMethod>> jsonRpcHandlers = {
        // Enabled on block explorer
        {"f_blocks_list_json", {makeMemberMethod(&RpcServer::f_on_blocks_list_json), false, true}},
        {"f_block_json", {makeMemberMethod(&RpcServer::f_on_block_json), false, true}},
        {"f_blocks_list_raw", {makeMemberMethod(&RpcServer::f_on_blocks_list_raw), false, true}},
        {"f_transaction_json", {makeMemberMethod(&RpcServer::f_on_transaction_json), false, true}},
        {"f_on_transactions_pool_json", {makeMemberMethod(&RpcServer::f_on_transactions_pool_json), false, true}},
        {"f_p2p_ban_info", {makeMemberMethod(&RpcServer::f_on_p2p_ban_info), false, true}},
        {"getblockcount", {makeMemberMethod(&RpcServer::on_getblockcount), true, true}},
        {"on_getblockhash", {makeMemberMethod(&RpcServer::on_getblockhash), false, true}},
        {"getcurrencyid", {makeMemberMethod(&RpcServer::on_get_currency_id), true, true}},
        {"getlastblockheader", {makeMemberMethod(&RpcServer::on_get_last_block_header), false, true}},
        {"getblockheaderbyhash", {makeMemberMethod(&RpcServer::on_get_block_header_by_hash), false, true}},
        {"getblockheaderbyheight", {makeMemberMethod(&RpcServer::on_get_block_header_by_height), false, true}},

        // clang-format off
        {RpcCommands::GetBlockTemplate::identifier(), {makeMemberMethod(&RpcServer::on_get_block_template), false, false}},
        {RpcCommands::GetBlockTemplateState::identifier(), {makeMemberMethod(&RpcServer::on_get_block_template_state), false, false}},
        {RpcCommands::SubmitBlock::identifier(), {makeMemberMethod(&RpcServer::on_submit_block), false, false}},
        // clang-format on

        // Not enabled on block explorer
        {"getblocktemplate", {makeMemberMethod(&RpcServer::on_getblocktemplate), false, false}},
        {"submitblock", {makeMemberMethod(&RpcServer::on_submitblock), false, false}}};

    auto it = jsonRpcHandlers.find(jsonRequest.getMethod());
    if (it == jsonRpcHandlers.end()) {
      throw JsonRpcError(JsonRpc::errMethodNotFound);
    }

    if (!it->second.allowBusyCore && !isCoreReady()) {
      throw JsonRpcError(CORE_RPC_ERROR_CODE_CORE_BUSY, "Core is busy");
    }

    if (isBlockexplorerOnly() && !it->second.isBlockexplorerRequest) {
      throw JsonRpcError(CORE_RPC_ERROR_CODE_BLOCK_EXPLORER_ONLY, "only block explorer requests are allowed");
    }

    it->second.handler(this, jsonRequest, jsonResponse);

  } catch (const JsonRpcError& err) {
    jsonResponse.setError(err);
  } catch (const std::exception& e) {
    jsonResponse.setError(JsonRpcError(JsonRpc::errInternalError, e.what()));
  }

  response.setBody(jsonResponse.getBody());
  logger(TRACE) << "JSON-RPC response: " << jsonResponse.getBody();
  return true;
}

bool RpcServer::setFeeAddress(const std::string fee_address) {
  AccountPublicAddress addr{};
  XI_RETURN_EC_IF_NOT(currency().parseAccountAddressString(fee_address, addr), false);
  m_fee_address = addr;
  return true;
}

bool RpcServer::setFeeAmount(const uint64_t fee_amount) {
  m_fee_amount = fee_amount;
  return true;
}

bool RpcServer::enableCors(const std::string& domain) {
  m_cors = domain;
  return true;
}

const std::string& RpcServer::getCorsDomain() { return m_cors; }

bool RpcServer::isBlockexplorer() const { return m_isBlockexplorer; }
void RpcServer::setBlockexplorer(bool enabled) {
  m_isBlockexplorer = enabled;
  logger(INFO) << "Blockexplorer " << (enabled ? "enabled" : "disabled");
}

bool RpcServer::isBlockexplorerOnly() const { return m_isBlockexplorerOnly; }
void RpcServer::setBlockexplorerOnly(bool enabled) {
  m_isBlockexplorerOnly = enabled;
  setBlockexplorer(true);
}

bool RpcServer::isCoreReady() {
  return m_core.getCurrency().isTestNet() || m_p2p.get_payload_object().isSynchronized();
}

bool RpcServer::on_options_request(const RpcServer::HttpRequest& request, RpcServer::HttpResponse& response) {
  if (!getCorsDomain().empty()) {
    response.headers().set(Xi::Http::HeaderContainer::AccessControlAllowOrigin, getCorsDomain());
    response.headers().setAccessControlRequestMethods({Xi::Http::Method::Post, Xi::Http::Method::Options});
  }
  response.headers().setAllow({Xi::Http::Method::Post, Xi::Http::Method::Get, Xi::Http::Method::Options});
  response.headers().setContentType(Xi::Http::ContentType::Json);
  return request.method() == Xi::Http::Method::Options;
}

bool RpcServer::on_get_blocks(const COMMAND_RPC_GET_BLOCKS_FAST::request& req,
                              COMMAND_RPC_GET_BLOCKS_FAST::response& res) {
  // TODO code duplication see InProcessNode::doGetNewBlocks()
  if (req.block_hashes.empty()) {
    res.status = "Failed";
    return false;
  }

  if (req.block_hashes.back() != m_core.getBlockHashByIndex(0)) {
    res.status = "Failed";
    return false;
  }

  uint32_t totalBlockCount;
  uint32_t startBlockIndex;
  auto supplementQuery = m_core.findBlockchainSupplement(
      req.block_hashes, Xi::Config::Limits::maximumRPCBlocksQueryCount(), totalBlockCount, startBlockIndex);

  if (supplementQuery.isError()) {
    logger(Logging::ERROR) << "Failed to query blockchain supplement: " << supplementQuery.error().message();
    res.status = "Failed";
    return false;
  }

  res.current_height = BlockHeight::fromIndex(totalBlockCount);
  res.start_height = BlockHeight::fromIndex(startBlockIndex);

  std::vector<Crypto::Hash> missedHashes;
  m_core.getBlocks(supplementQuery.value(), res.blocks, missedHashes);
  assert(missedHashes.empty());

  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_query_blocks(const COMMAND_RPC_QUERY_BLOCKS::request& req, COMMAND_RPC_QUERY_BLOCKS::response& res) {
  uint32_t startIndex;
  uint32_t currentIndex;
  uint32_t fullOffset;

  if (!m_core.queryBlocks(req.block_ids, req.timestamp, startIndex, currentIndex, fullOffset, res.blocks)) {
    res.status = "Failed to perform query";
    return false;
  }

  res.start_height = BlockHeight::fromIndex(startIndex);
  res.current_height = BlockHeight::fromIndex(currentIndex);
  res.full_offset = fullOffset;
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_query_blocks_lite(const COMMAND_RPC_QUERY_BLOCKS_LITE::request& req,
                                     COMMAND_RPC_QUERY_BLOCKS_LITE::response& res) {
  uint32_t startIndex;
  uint32_t currentIndex;
  uint32_t fullOffset;
  if (!m_core.queryBlocksLite(req.block_hashes, req.timestamp, startIndex, currentIndex, fullOffset, res.blocks)) {
    res.status = "Failed to perform query";
    return false;
  }

  res.start_height = BlockHeight::fromIndex(startIndex);
  res.current_height = BlockHeight::fromIndex(currentIndex);
  res.full_offset = fullOffset;
  res.status = CORE_RPC_STATUS_OK;

  return true;
}

bool RpcServer::on_query_blocks_detailed(const COMMAND_RPC_QUERY_BLOCKS_DETAILED::request& req,
                                         COMMAND_RPC_QUERY_BLOCKS_DETAILED::response& res) {
  uint32_t startIndex;
  uint32_t currentIndex;
  uint32_t fullOffset;
  if (!m_core.queryBlocksDetailed(req.block_hashes, req.timestamp, startIndex, currentIndex, fullOffset, res.blocks)) {
    res.status = "Failed to perform query";
    return false;
  }

  res.start_height = BlockHeight::fromIndex(startIndex);
  res.current_height = BlockHeight::fromIndex(currentIndex);
  res.full_offset = fullOffset;
  res.status = CORE_RPC_STATUS_OK;

  return true;
}

bool RpcServer::on_get_indexes(const COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::request& req,
                               COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES::response& res) {
  std::vector<uint32_t> outputIndexes;
  if (!m_core.getTransactionGlobalIndexes(req.transaction_hash, outputIndexes)) {
    res.status = "Failed";
    return true;
  }

  res.output_indices.assign(outputIndexes.begin(), outputIndexes.end());
  res.status = CORE_RPC_STATUS_OK;
  logger(TRACE) << "COMMAND_RPC_GET_TX_GLOBAL_OUTPUTS_INDEXES: [" << res.output_indices.size() << "]";
  return true;
}

bool RpcServer::on_get_random_outs(const COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::request& req,
                                   COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::response& res) {
  res.status = "Failed";

  for (uint64_t amount : req.amounts) {
    std::vector<uint32_t> globalIndexes;
    std::vector<Crypto::PublicKey> publicKeys;
    if (!m_core.getRandomOutputs(amount, static_cast<uint16_t>(req.outs_count), globalIndexes, publicKeys)) {
      return true;
    }

    assert(globalIndexes.size() == publicKeys.size());
    res.outs.emplace_back(COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS_outs_for_amount{amount, {}});
    for (size_t i = 0; i < globalIndexes.size(); ++i) {
      res.outs.back().outs.push_back({globalIndexes[i], publicKeys[i]});
    }
  }

  res.status = CORE_RPC_STATUS_OK;

  std::stringstream ss;
  typedef COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::outs_for_amount outs_for_amount;
  typedef COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::out_entry out_entry;

  std::for_each(res.outs.begin(), res.outs.end(), [&](outs_for_amount& ofa) {
    ss << "[" << ofa.amount << "]:";

    assert(ofa.outs.size() && "internal error: ofa.outs.size() is empty");

    std::for_each(ofa.outs.begin(), ofa.outs.end(), [&](out_entry& oe) { ss << oe.global_amount_index << " "; });
    ss << ENDL;
  });
  std::string s = ss.str();
  logger(TRACE) << "COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS: " << ENDL << s;
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::onGetPoolChanges(const COMMAND_RPC_GET_POOL_CHANGES::request& req,
                                 COMMAND_RPC_GET_POOL_CHANGES::response& rsp) {
  rsp.status = CORE_RPC_STATUS_OK;
  rsp.is_current_tail_block = m_core.getPoolChanges(req.tail_block_hash, req.known_transaction_hashes,
                                                    rsp.added_transactions, rsp.deleted_transactions_hashes);

  return true;
}

bool RpcServer::onGetPoolChangesLite(const COMMAND_RPC_GET_POOL_CHANGES_LITE::request& req,
                                     COMMAND_RPC_GET_POOL_CHANGES_LITE::response& rsp) {
  rsp.status = CORE_RPC_STATUS_OK;
  rsp.is_current_tail_block = m_core.getPoolChangesLite(req.tail_block_hash, req.known_transaction_hashes,
                                                        rsp.added_transactions, rsp.deleted_transaction_hashes);

  return true;
}

bool RpcServer::onGetBlocksDetailsByHeights(const COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS::request& req,
                                            COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HEIGHTS::response& rsp) {
  try {
    std::vector<BlockDetails> blockDetails;
    for (const BlockHeight& height : req.block_heights) {
      auto search = m_core.getBlockDetails(height.toIndex());
      if (!search) {
        notFound("block", std::to_string(height.native()));
      }
      blockDetails.push_back(std::move(*search));
    }

    rsp.blocks = std::move(blockDetails);
  } catch (std::system_error& e) {
    rsp.status = e.what();
    return false;
  } catch (std::exception& e) {
    rsp.status = "Error: " + std::string(e.what());
    return false;
  }

  rsp.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::onGetBlocksDetailsByHashes(const COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::request& req,
                                           COMMAND_RPC_GET_BLOCKS_DETAILS_BY_HASHES::response& rsp) {
  try {
    std::vector<BlockDetails> blockDetails;
    for (const Crypto::Hash& hash : req.block_hashes) {
      auto search = m_core.getBlockDetails(hash);
      if (!search) {
        notFound("block", hash.toString());
      }
      blockDetails.push_back(std::move(*search));
    }

    rsp.blocks = std::move(blockDetails);
  } catch (std::system_error& e) {
    rsp.status = e.what();
    return false;
  } catch (std::exception& e) {
    rsp.status = "Error: " + std::string(e.what());
    return false;
  }

  rsp.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::onGetBlockDetailsByHeight(const COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::request& req,
                                          COMMAND_RPC_GET_BLOCK_DETAILS_BY_HEIGHT::response& rsp) {
  try {
    XI_RETURN_EC_IF(req.block_height.isNull(), false);
    auto search = m_core.getBlockDetails(req.block_height.toIndex());
    if (!search) {
      notFound("block", std::to_string(req.block_height.native()));
    }
    rsp.block = std::move(*search);
  } catch (std::system_error& e) {
    rsp.status = e.what();
    return false;
  } catch (std::exception& e) {
    rsp.status = "Error: " + std::string(e.what());
    return false;
  }

  rsp.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::onGetBlocksHashesByTimestamps(const COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::request& req,
                                              COMMAND_RPC_GET_BLOCKS_HASHES_BY_TIMESTAMPS::response& rsp) {
  try {
    if (req.timestamp_begin > req.timestamp_end) {
      rsp.status = "negative timespan (begin > end)";
      return false;
    } else if (req.timestamp_end - req.timestamp_begin > 60 * 60 * 24) {
      rsp.status = "timespan exceeds one day threshold";
      return false;
    }
    auto blockHashes = m_core.getBlockHashesByTimestamps(req.timestamp_begin, req.timestamp_end);
    rsp.block_hashes = std::move(blockHashes);
  } catch (std::system_error& e) {
    rsp.status = e.what();
    return false;
  } catch (std::exception& e) {
    rsp.status = "Error: " + std::string(e.what());
    return false;
  }

  rsp.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::onGetTransactionDetailsByHashes(const COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::request& req,
                                                COMMAND_RPC_GET_TRANSACTION_DETAILS_BY_HASHES::response& rsp) {
  try {
    std::vector<TransactionDetails> transactionDetails;
    transactionDetails.reserve(req.transaction_hashes.size());

    for (const auto& hash : req.transaction_hashes) {
      transactionDetails.push_back(m_core.getTransactionDetails(hash));
    }

    rsp.transactions = std::move(transactionDetails);
  } catch (std::system_error& e) {
    rsp.status = e.what();
    return false;
  } catch (std::exception& e) {
    rsp.status = "Error: " + std::string(e.what());
    return false;
  }

  rsp.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::onGetTransactionHashesByPaymentId(const COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::request& req,
                                                  COMMAND_RPC_GET_TRANSACTION_HASHES_BY_PAYMENT_ID::response& rsp) {
  try {
    rsp.transactionHashes = m_core.getTransactionHashesByPaymentId(req.payment_id);
  } catch (std::system_error& e) {
    rsp.status = e.what();
    return false;
  } catch (std::exception& e) {
    rsp.status = "Error: " + std::string(e.what());
    return false;
  }

  rsp.status = CORE_RPC_STATUS_OK;
  return true;
}

//
// JSON handlers
//

bool RpcServer::on_get_info(const COMMAND_RPC_GET_INFO::request& req, COMMAND_RPC_GET_INFO::response& res) {
  XI_UNUSED(req);
  res.height = BlockHeight::fromIndex(m_core.getTopBlockIndex());
  res.difficulty = m_core.getDifficultyForNextBlock();
  res.tx_count = m_core.getBlockchainTransactionCount() - res.height.native();  // without coinbase, but static reward
  res.tx_pool_size = m_core.transactionPool().size();
  res.tx_pool_state = m_core.transactionPool().stateHash();
  res.version = m_core.getTopBlockVersion();
  res.tx_min_fee = m_core.getCurrency().minimumFee(res.version);
  res.alt_blocks_count = m_core.getAlternativeBlockCount();
  uint64_t total_conn = m_p2p.get_connections_count();
  res.outgoing_connections_count = m_p2p.get_outgoing_connections_count();
  res.incoming_connections_count = total_conn - res.outgoing_connections_count;
  res.white_peerlist_size = m_p2p.getPeerlistManager().get_white_peers_count();
  res.grey_peerlist_size = m_p2p.getPeerlistManager().get_gray_peers_count();
  res.last_known_block_height = std::max(BlockHeight::fromIndex(0), m_protocol.getObservedHeight());
  res.network_height = std::max(BlockHeight::fromIndex(0), m_protocol.getBlockchainHeight());
  const auto forks = Xi::Config::BlockVersion::forks();
  std::transform(forks.begin(), forks.end(), std::back_inserter(res.upgrade_heights),
                 [](const auto forkIndex) { return BlockHeight::fromIndex(forkIndex); });
  res.supported_height = res.upgrade_heights.empty() ? BlockHeight::fromIndex(0) : *res.upgrade_heights.rbegin();
  res.hashrate =
      (uint32_t)round(res.difficulty / Xi::Config::Time::blockTimeSeconds());  // TODO Not a good approximation
  res.synced = res.height == res.network_height;
  res.network = Xi::to_string(m_core.getCurrency().network());
  auto topBlockSearch = m_core.getBlockByIndex(m_core.getTopBlockIndex());
  if (!topBlockSearch) {
    return false;
  }
  res.softwareVersion = APP_VERSION;
  res.status = CORE_RPC_STATUS_OK;
  res.start_time = (uint64_t)m_core.getStartTime();
  return true;
}

const Currency& RpcServer::currency() const { return m_core.currency(); }

bool RpcServer::on_get_height(const COMMAND_RPC_GET_HEIGHT::request& req, COMMAND_RPC_GET_HEIGHT::response& res) {
  XI_UNUSED(req);
  res.height = BlockHeight::fromIndex(m_core.getTopBlockIndex());
  res.network_height = std::max(BlockHeight::fromIndex(0), m_protocol.getBlockchainHeight());
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_get_transactions(const COMMAND_RPC_GET_TRANSACTIONS::request& req,
                                    COMMAND_RPC_GET_TRANSACTIONS::response& res) {
  if (req.transaction_hashes.size() > 100) {
    res.status = "too many transactions requested";
    return true;
  }

  std::vector<Hash> missed_txs;
  std::vector<BinaryArray> txs;
  m_core.getTransactions(req.transaction_hashes, txs, missed_txs);

  for (auto& tx : txs) {
    Transaction transaction;
    if (!fromBinaryArray(transaction, tx)) {
      logger(ERROR) << "unable to deserialize transaction returned from core";
    } else {
      res.transactions.push_back(std::move(transaction));
    }
  }

  for (const auto& miss_tx : missed_txs) {
    res.missed_transactions.push_back(miss_tx);
  }

  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_send_raw_tx(const COMMAND_RPC_SEND_RAW_TX::request& req, COMMAND_RPC_SEND_RAW_TX::response& res) {
  auto txPushResult = m_core.transactionPool().pushTransaction(req.transaction);
  if (txPushResult.isError()) {
    logger(DEBUGGING) << "[on_send_raw_tx]: tx verification failed (" << txPushResult.error().message() << ")";
    res.status = "Failed";
    return true;
  }

  m_protocol.relayTransactions({toBinaryArray(req.transaction)});
  // TODO: make sure that tx has reached other nodes here, probably wait to receive reflections from other nodes
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_get_fee_info(const COMMAND_RPC_GET_FEE_ADDRESS::request& req,
                                COMMAND_RPC_GET_FEE_ADDRESS::response& res) {
  XI_UNUSED(req);
  if (m_fee_address) {
    res.fee = FeeAddress{*m_fee_address, m_fee_amount};
    res.status = CORE_RPC_STATUS_OK;
    return true;
  } else {
    res.fee = std::nullopt;
    res.status = CORE_RPC_STATUS_OK;
    return true;
  }
}

bool RpcServer::on_get_peers(const COMMAND_RPC_GET_PEERS::request& req, COMMAND_RPC_GET_PEERS::response& res) {
  XI_UNUSED(req);
  std::list<PeerlistEntry> peers_white;
  std::list<PeerlistEntry> peers_gray;

  m_p2p.getPeerlistManager().get_peerlist_full(peers_gray, peers_white);
  const auto getNetworkAddress = [](const PeerlistEntry& entry) -> NetworkAddress { return entry.address; };
  std::transform(peers_white.begin(), peers_white.end(), std::back_inserter(res.peers_white), getNetworkAddress);
  std::transform(peers_gray.begin(), peers_gray.end(), std::back_inserter(res.peers_gray), getNetworkAddress);
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

//------------------------------------------------------------------------------------------------------------------------------
// JSON RPC methods
//------------------------------------------------------------------------------------------------------------------------------
bool RpcServer::f_on_blocks_list_json(const F_COMMAND_RPC_GET_BLOCKS_LIST::request& req,
                                      F_COMMAND_RPC_GET_BLOCKS_LIST::response& res) {
  // check if blockchain explorer RPC is enabled
  if (!isBlockexplorer()) {
    return false;
  }

  if (req.height < BlockHeight::fromIndex(0)) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_TOO_SMALL_HEIGHT, "Height must be at least 1."};
  }
  if (BlockHeight::fromIndex(m_core.getTopBlockIndex()) < req.height) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_TOO_BIG_HEIGHT,
                                std::string("To big height: ") + std::to_string(req.height.native()) +
                                    ", current blockchain height = " + std::to_string(m_core.getTopBlockIndex())};
  }

  BlockOffset print_blocks_count = BlockOffset::fromNative(30);
  BlockHeight last_height = req.height - print_blocks_count;
  if (print_blocks_count.native() > req.height.native()) {
    last_height = BlockHeight::Genesis;
  }

  for (auto i = req.height; i >= last_height && !i.isNull(); i -= BlockOffset::fromNative(1)) {
    Hash block_hash = m_core.getBlockHashByIndex(i.toIndex());
    if (!m_core.hasBlock(block_hash)) {
      throw JsonRpc::JsonRpcError{
          CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
          "Internal error: can't get block by height. Height = " + std::to_string(i.native()) + '.'};
    }

    auto blockSearch = m_core.getBlockByHash(block_hash);
    if (!blockSearch) {
      throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_NOT_FOUND, "Block not found: " + block_hash.toString()};
    }
    auto search = m_core.getBlockDetails(block_hash);
    if (!search) {
      notFound("blow", block_hash.toString());
    }
    const auto& blkDetails = *search;

    f_block_short_response block_short;
    block_short.cumulative_size = blkDetails.blockSize;
    block_short.timestamp = blkDetails.timestamp;
    block_short.difficulty = blkDetails.difficulty;
    block_short.height = i;
    block_short.hash = block_hash;
    block_short.transactions_count = blkDetails.transactions.size();

    res.blocks.push_back(block_short);
  }

  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::f_on_block_json(const F_COMMAND_RPC_GET_BLOCK_DETAILS::request& req,
                                F_COMMAND_RPC_GET_BLOCK_DETAILS::response& res) {
  if (!isBlockexplorer()) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                "The blockexplorer is not enabled on this endpoint but required by this operation."};
  }

  if (!m_core.hasBlock(req.hash)) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                "Internal error: can't get block by hash. Hash = " + req.hash.toString() + '.'};
  }

  auto blockSearch = m_core.getBlockByHash(req.hash);
  if (!blockSearch) {
    notFound("block", req.hash.toString());
  }
  auto& blk = blockSearch->block.block().getBlock();

  auto search = m_core.getBlockDetails(req.hash);
  if (!search) {
    notFound("blow", req.hash.toString());
  }
  const auto& blkDetails = *search;

  const auto validateCoinbaseTransaction = [](const auto& tx) {
    if (tx.inputs.size() != 1) {
      throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                  "Internal error: coinbase transaction has invalid input size."};
    } else if (!std::holds_alternative<BaseInput>(tx.inputs.front())) {
      throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                  "Internal error: coinbase transaction in the block has the wrong type"};
    }
  };

  validateCoinbaseTransaction(blk.baseTransaction);

  block_header_response block_header;
  fill_block_header_response(blk, false, res.block.height, req.hash, block_header);

  res.block.cumulative_size = blkDetails.blockSize;
  res.block.timestamp = block_header.timestamp;
  res.block.height = std::get<BaseInput>(blk.baseTransaction.inputs.front()).height;
  res.block.hash = req.hash;
  res.block.difficulty = m_core.getBlockDifficulty(res.block.height.toIndex());

  res.block.version = block_header.version;
  res.block.upgrade_vote = block_header.upgrade_vote;
  res.block.previous_hash = block_header.prev_hash;
  res.block.nonce = block_header.nonce;
  res.block.depth = BlockHeight::fromIndex(m_core.getTopBlockIndex()).native() - res.block.height.native();
  res.block.transactions_cumulative_size = blkDetails.transactionsCumulativeSize;
  res.block.circulating_supply = blkDetails.alreadyGeneratedCoins;
  res.block.transactions_generated = blkDetails.alreadyGeneratedTransactions;
  res.block.reward = block_header.reward;
  res.block.static_reward = block_header.static_reward;
  res.block.median_size = blkDetails.sizeMedian;
  res.block.block_size = blkDetails.blockSize;
  res.block.orphan_status = blkDetails.isAlternative;

  size_t blockGrantedFullRewardZone =
      m_core.getCurrency().blockGrantedFullRewardZoneByBlockVersion(block_header.version);
  res.block.median_effective_size = std::max(res.block.median_size, blockGrantedFullRewardZone);

  res.block.base_reward = blkDetails.baseReward;
  res.block.penalty = blkDetails.penalty;

  // Base transaction adding
  f_transaction_short_response transaction_short;
  transaction_short.hash = getObjectHash(blk.baseTransaction);
  transaction_short.fee = 0;
  transaction_short.amount_out = getOutputAmount(blk.baseTransaction);
  transaction_short.size = getObjectBinarySize(blk.baseTransaction);
  res.block.transactions.push_back(transaction_short);

  auto staticReward = m_core.currency().constructStaticRewardTx(blk).takeOrThrow();
  if (staticReward.has_value()) {
    // Static reward adding
    const auto& static_reward_tx = *staticReward;
    f_transaction_short_response short_static_reward_info;
    short_static_reward_info.hash = getObjectHash(static_reward_tx);
    short_static_reward_info.fee = 0;
    short_static_reward_info.amount_out = getOutputAmount(static_reward_tx);
    short_static_reward_info.size = getObjectBinarySize(static_reward_tx);
    res.block.transactions.push_back(short_static_reward_info);
  }

  std::vector<Crypto::Hash> missed_txs;
  std::vector<BinaryArray> txs;
  m_core.getTransactions(blk.transactionHashes, txs, missed_txs);

  res.block.total_fee_amount = 0;

  for (const BinaryArray& ba : txs) {
    Transaction tx;
    if (!fromBinaryArray(tx, ba)) {
      throw std::runtime_error("Couldn't deserialize transaction");
    }
    f_transaction_short_response i_transaction_short;
    uint64_t amount_in = getInputAmount(tx);
    uint64_t amount_out = getOutputAmount(tx);

    i_transaction_short.hash = getObjectHash(tx);
    i_transaction_short.fee = amount_in - amount_out;
    i_transaction_short.amount_out = amount_out;
    i_transaction_short.size = getObjectBinarySize(tx);
    res.block.transactions.push_back(i_transaction_short);

    res.block.total_fee_amount += i_transaction_short.fee;
  }

  res.block.transactions_count = res.block.transactions.size();

  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::f_on_blocks_list_raw(const F_COMMAND_RPC_GET_BLOCKS_RAW_BY_RANGE::request& req,
                                     F_COMMAND_RPC_GET_BLOCKS_RAW_BY_RANGE::response& res) {
  if (!isBlockexplorer()) {
    return false;
  }

  if (req.height.isNull()) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_TOO_SMALL_HEIGHT, "height param must be at least 1."};
  }

  if (req.height > BlockHeight::fromNative(m_core.getTopBlockIndex())) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_TOO_BIG_HEIGHT, "queried height exceeds local blockchain height."};
  }

  if (req.count < 1) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_WRONG_PARAM, "count must be at least 1"};
  }

  if (req.count > 100) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_WRONG_PARAM, "count may not exceed 100."};
  }

  res.blocks = m_core.getBlocks(req.height.toIndex(), req.count);
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::f_on_transaction_json(const F_COMMAND_RPC_GET_TRANSACTION_DETAILS::request& req,
                                      F_COMMAND_RPC_GET_TRANSACTION_DETAILS::response& res) {
  // check if blockchain explorer RPC is enabled
  if (!isBlockexplorer()) {
    return false;
  }

  Hash hash = req.hash;

  std::vector<Crypto::Hash> tx_ids;
  tx_ids.push_back(hash);

  std::vector<Crypto::Hash> missed_txs;
  std::vector<BinaryArray> txs;
  m_core.getTransactions(tx_ids, txs, missed_txs);

  if (1 == txs.size()) {
    Transaction transaction;
    if (!fromBinaryArray(transaction, txs.front())) {
      throw std::runtime_error("Couldn't deserialize transaction");
    }
    res.transaction = transaction;
  } else {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_WRONG_PARAM,
                                "transaction wasn't found. Hash = " + req.hash.toString() + '.'};
  }
  TransactionDetails transactionDetails = m_core.getTransactionDetails(hash);

  Crypto::Hash blockHash;
  if (transactionDetails.inBlockchain) {
    auto blockHeight = transactionDetails.blockHeight;
    blockHash = m_core.getBlockHashByIndex(blockHeight.toIndex());

    auto blockSearch = m_core.getBlockByIndex(blockHeight.toIndex());
    if (!blockSearch) {
      notFound("block", blockHash.toString());
    }

    auto search = m_core.getBlockDetails(blockSearch->block.block().getBlockHash());
    if (!search) {
      notFound("blow", blockSearch->block.block().getBlockHash().toString());
    }
    const auto& blkDetails = *search;
    auto& blk = blockSearch->block.block().getBlock();

    f_block_short_response block_short;

    block_short.cumulative_size = blkDetails.blockSize;
    block_short.timestamp = blk.timestamp;
    block_short.height = blockHeight;
    block_short.hash = blockHash;
    block_short.difficulty = blkDetails.difficulty;
    block_short.transactions_count = blk.transactionHashes.size() + 1;
    res.block = block_short;
  }

  uint64_t amount_in = getInputAmount(res.transaction);
  uint64_t amount_out = getOutputAmount(res.transaction);

  res.transaction_details.hash = getObjectHash(res.transaction);
  res.transaction_details.fee = amount_in - amount_out;
  if (amount_in == 0) res.transaction_details.fee = 0;
  res.transaction_details.amount_out = amount_out;
  res.transaction_details.size = getObjectBinarySize(res.transaction);

  uint64_t mixin;
  if (!f_getMixin(res.transaction, mixin)) {
    return false;
  }
  res.transaction_details.mixin = mixin;

  PaymentId paymentId;
  if (CryptoNote::getPaymentIdFromTxExtra(res.transaction.extra, paymentId)) {
    res.transaction_details.payment_id = paymentId;
  } else {
    res.transaction_details.payment_id.nullify();
  }

  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::f_on_transactions_pool_json(const F_COMMAND_RPC_GET_POOL::request& req,
                                            F_COMMAND_RPC_GET_POOL::response& res) {
  XI_UNUSED(req);
  // check if blockchain explorer RPC is enabled
  if (!isBlockexplorer()) {
    return false;
  }

  res.state = m_core.transactionPool().stateHash();
  auto pool = m_core.getPoolTransactions();
  for (const Transaction& tx : pool) {
    f_transaction_short_response transaction_short;
    uint64_t amount_in = getInputAmount(tx);
    uint64_t amount_out = getOutputAmount(tx);

    transaction_short.hash = getObjectHash(tx);
    transaction_short.fee = amount_in - amount_out;
    transaction_short.amount_out = amount_out;
    transaction_short.size = getObjectBinarySize(tx);
    res.transactions.push_back(transaction_short);
  }

  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::f_on_p2p_ban_info(const F_COMMAND_RPC_GET_P2P_BAN_INFO::request& req,
                                  F_COMMAND_RPC_GET_P2P_BAN_INFO::response& res) {
  XI_UNUSED(req);
  if (!isBlockexplorer()) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_BLOCK_EXPLORER_DISABLED,
                                "Block explorer must be enabled for this request."};
  }

  auto peerBans = m_p2p.blockedPeers();
  auto peerPanalites = m_p2p.peerPenalties();

  time_t currentTime = time(nullptr);
  for (auto iPeerBan = peerBans.begin(); iPeerBan != peerBans.end(); /* */) {
    if (iPeerBan->second < currentTime) {
      peerBans.erase(iPeerBan++);
    } else {
      ++iPeerBan;
    }
  }

  if (peerBans.empty() && peerBans.empty()) {
    return true;
  }

  for (const auto& iPeerBan : peerBans) {
    F_P2P_BAN_INFO iBanInfo{/* */};
    iBanInfo.ip_address = Common::ipAddressToString(iPeerBan.first);
    iBanInfo.ban_timestamp = iPeerBan.second;
    auto penaltySearch = peerPanalites.find(iPeerBan.first);
    if (penaltySearch != peerPanalites.end()) {
      iBanInfo.penalty_score = penaltySearch->second;
      peerPanalites.erase(penaltySearch);
    } else {
      iBanInfo.penalty_score = 0;
    }
    res.peers_ban_info.emplace_back(std::move(iBanInfo));
  }

  for (const auto& iPeerPenalty : peerPanalites) {
    F_P2P_BAN_INFO iBanInfo{/* */};
    iBanInfo.ip_address = Common::ipAddressToString(iPeerPenalty.first);
    iBanInfo.penalty_score = iPeerPenalty.second;
    iBanInfo.ban_timestamp = 0;
    res.peers_ban_info.emplace_back(std::move(iBanInfo));
  }

  return true;
}

bool RpcServer::f_getMixin(const Transaction& transaction, uint64_t& mixin) {
  mixin = 0;
  for (const TransactionInput& txin : transaction.inputs) {
    if (auto keyInput = std::get_if<KeyInput>(&txin)) {
      mixin = std::max(mixin, keyInput->outputIndices.empty() ? 0 : keyInput->outputIndices.size() - 1);
    }
  }
  return true;
}

void RpcServer::checkService(RpcServiceType type) const {
  if (hasFlag(type, RpcServiceType::BlockExplorer) && !isBlockexplorer()) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_BLOCK_EXPLORER_DISABLED, "blockexplorer is disabled."};
  }
}

void RpcServer::notFound(const std::string& resource, const std::string& id) const {
  throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_NOT_FOUND,
                              fmt::format("Resource type '{}' not found for id '{}'.", resource, id)};
}

bool RpcServer::on_getblockcount(const COMMAND_RPC_GETBLOCKCOUNT::request& req,
                                 COMMAND_RPC_GETBLOCKCOUNT::response& res) {
  XI_UNUSED(req);
  res.count = m_core.getTopBlockIndex() + 1;
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_getblockhash(const COMMAND_RPC_GETBLOCKHASH::request& req, COMMAND_RPC_GETBLOCKHASH::response& res) {
  auto h = req.height;
  if (h.isNull()) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_TOO_SMALL_HEIGHT, "Height must be greater zero"};
  }

  Crypto::Hash blockId = m_core.getBlockHashByIndex(h.toIndex());
  if (blockId == Hash::Null) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_TOO_BIG_HEIGHT,
                                std::string("Too big height: ") + std::to_string(h.native()) +
                                    ", current blockchain height = " + std::to_string(m_core.getTopBlockIndex() + 1)};
  }

  res.hash = blockId;
  return true;
}

Hash RpcServer::block_template_state_hash() const {
  const auto topBlock = m_core.getTopBlockHash();
  const auto poolState = m_core.transactionPool().stateHash();
  Hash reval{};
  for (size_t i = 0; i < sizeof(reval); ++i) {
    reval[i] = topBlock[i] ^ poolState[i];
  }
  return reval;
}

bool RpcServer::on_getblocktemplatestate(const COMMAND_RPC_GETBLOCKTEMPLATE_STATE::request& req,
                                         COMMAND_RPC_GETBLOCKTEMPLATE_STATE::response& res) {
  XI_UNUSED(req);
  const auto stateHash = block_template_state_hash();
  res.template_state = stateHash;
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

namespace {
uint64_t slow_memmem(void* start_buff, size_t buflen, void* pat, size_t patlen) {
  void* buf = memchr(start_buff, ((char*)pat)[0], buflen);
  void* end = (char*)buf + buflen - patlen;
  while (buf) {
    if (buf > end) return 0;
    if (memcmp(buf, pat, patlen) == 0) return static_cast<uint64_t>((char*)buf - (char*)start_buff);
    buf = (char*)buf + 1;
    buf = memchr(buf, ((char*)pat)[0], buflen);
  }
  return 0;
}
}  // namespace

bool RpcServer::on_getblocktemplate(const COMMAND_RPC_GETBLOCKTEMPLATE::request& req,
                                    COMMAND_RPC_GETBLOCKTEMPLATE::response& res) {
  // TODO REMOVE ME
  if (req.reserve_size > TX_EXTRA_NONCE_MAX_COUNT) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_TOO_BIG_RESERVE_SIZE, "To big reserved size, maximum 126"};
  }

  AccountPublicAddress acc = boost::value_initialized<AccountPublicAddress>();

  if (!req.wallet_address.isValid()) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_WRONG_WALLET_ADDRESS, "Invalid wallet address"};
  }

  BlockTemplate blockTemplate = boost::value_initialized<BlockTemplate>();
  CryptoNote::BinaryArray blob_reserve;
  blob_reserve.resize(req.reserve_size, 0);

  uint32_t blockTemplateIndex = 0;
  if (!m_core.getBlockTemplate(blockTemplate, acc, blob_reserve, res.difficulty, blockTemplateIndex)) {
    logger(ERROR) << "Failed to create block template";
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR, "Internal error: failed to create block template"};
  }
  res.height = BlockHeight::fromIndex(blockTemplateIndex);

  BinaryArray block_blob = toBinaryArray(blockTemplate);
  PublicKey tx_pub_key = CryptoNote::getTransactionPublicKeyFromExtra(blockTemplate.baseTransaction.extra);
  if (tx_pub_key == PublicKey::Null) {
    logger(ERROR) << "Failed to find tx pub key in coinbase extra";
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                "Internal error: failed to find tx pub key in coinbase extra"};
  }

  if (0 < req.reserve_size) {
    res.reserved_offset = slow_memmem((void*)block_blob.data(), block_blob.size(), &tx_pub_key, sizeof(tx_pub_key));
    if (!res.reserved_offset) {
      logger(ERROR) << "Failed to find tx pub key in blockblob";
      throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                  "Internal error: failed to create block template"};
    }
    res.reserved_offset += sizeof(tx_pub_key) + 3;  // 3 bytes: tag for TX_EXTRA_TAG_PUBKEY(1 byte), tag for
                                                    // TX_EXTRA_NONCE(1 byte), counter in TX_EXTRA_NONCE(1 byte)
    if (res.reserved_offset + req.reserve_size > block_blob.size()) {
      logger(ERROR) << "Failed to calculate offset for reserved bytes";
      throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                  "Internal error: failed to create block template"};
    }
  } else {
    res.reserved_offset = 0;
  }

  res.block_template = fromBinaryArray<BlockTemplate>(block_blob);
  res.template_state = block_template_state_hash();
  res.status = CORE_RPC_STATUS_OK;

  return true;
}

bool RpcServer::on_get_block_template_state(const RpcCommands::GetBlockTemplateState::request& req,
                                            RpcCommands::GetBlockTemplateState::response& res) {
  XI_UNUSED(req);
  const auto stateHash = block_template_state_hash();
  res.template_state = Common::podToHex(stateHash);
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_get_block_template(const RpcCommands::GetBlockTemplate::request& req,
                                      RpcCommands::GetBlockTemplate::response& res) {
  if (req.reserve_size > TX_EXTRA_NONCE_MAX_COUNT) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_TOO_BIG_RESERVE_SIZE, "To big reserved size, maximum 126"};
  }

  AccountPublicAddress acc = boost::value_initialized<AccountPublicAddress>();

  if (!req.wallet_address.size() || !m_core.getCurrency().parseAccountAddressString(req.wallet_address, acc)) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_WRONG_WALLET_ADDRESS, "Failed to parse wallet address"};
  }

  BlockTemplate blockTemplate = boost::value_initialized<BlockTemplate>();
  CryptoNote::BinaryArray blob_reserve;
  uint32_t blockTemplateIndex = 0;
  if (!m_core.getBlockTemplate(blockTemplate, acc, blob_reserve, res.difficulty, blockTemplateIndex)) {
    logger(ERROR) << "Failed to create block template";
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR, "Internal error: failed to create block template"};
  }
  res.height = BlockHeight::fromIndex(blockTemplateIndex);

  BinaryArray block_blob = toBinaryArray(blockTemplate);
  PublicKey tx_pub_key = CryptoNote::getTransactionPublicKeyFromExtra(blockTemplate.baseTransaction.extra);
  if (tx_pub_key == PublicKey::Null) {
    logger(ERROR) << "Failed to find tx pub key in coinbase extra";
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                "Internal error: failed to find tx pub key in coinbase extra"};
  }

  if (0 < req.reserve_size) {
    res.reserved_offset = static_cast<uint32_t>(
        slow_memmem((void*)block_blob.data(), block_blob.size(), &tx_pub_key, sizeof(tx_pub_key)));
    if (!res.reserved_offset) {
      logger(ERROR) << "Failed to find tx pub key in blockblob";
      throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                  "Internal error: failed to create block template"};
    }
    res.reserved_offset += sizeof(tx_pub_key) + 3;  // 3 bytes: tag for TX_EXTRA_TAG_PUBKEY(1 byte), tag for
                                                    // TX_EXTRA_NONCE(1 byte), counter in TX_EXTRA_NONCE(1 byte)
    if (res.reserved_offset + req.reserve_size > block_blob.size()) {
      logger(ERROR) << "Failed to calculate offset for reserved bytes";
      throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                  "Internal error: failed to create block template"};
    }
  } else {
    res.reserved_offset = 0;
  }

  res.blocktemplate_blob = toHex(block_blob);
  const auto btstate = block_template_state_hash();
  res.template_state = Common::podToHex(btstate);
  res.status = CORE_RPC_STATUS_OK;

  return true;
}

bool RpcServer::on_submit_block(const RpcCommands::SubmitBlock::request& req, RpcCommands::SubmitBlock::response& res) {
  XI_CONCURRENT_RLOCK(m_submissionAccess);

  BinaryArray blockblob;
  if (!fromHex(req.hex_binary_template, blockblob)) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_WRONG_BLOCKBLOB, "Wrong block blob"};
  }

  auto submitResult = m_core.submitBlock(BinaryArray{blockblob});
  if (submitResult != error::AddBlockErrorCondition::BLOCK_ADDED) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_BLOCK_NOT_ACCEPTED, "Block not accepted"};
  }

  res.result = submitResult.message();
  if (submitResult == error::AddBlockErrorCode::ADDED_TO_MAIN ||
      submitResult == error::AddBlockErrorCode::ADDED_TO_ALTERNATIVE_AND_SWITCHED) {
    auto blockTemplate = fromBinaryArray<BlockTemplate>(blockblob);  // safe as long as core checked it for us.
    LiteBlock blockToRelay;
    blockToRelay.blockTemplate = std::move(blockblob);
    m_protocol.relayBlock(std::move(blockToRelay));
  }

  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_get_currency_id(const COMMAND_RPC_GET_CURRENCY_ID::request& /*req*/,
                                   COMMAND_RPC_GET_CURRENCY_ID::response& res) {
  Hash genesisBlockHash = m_core.getCurrency().genesisBlockHash();
  res.currency_id_blob = Common::podToHex(genesisBlockHash);
  return true;
}

bool RpcServer::on_submitblock(const COMMAND_RPC_SUBMITBLOCK::request& req, COMMAND_RPC_SUBMITBLOCK::response& res) {
  XI_CONCURRENT_RLOCK(m_submissionAccess);
  if (req.size() != 1) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_WRONG_PARAM, "Wrong param"};
  }

  BinaryArray blockblob;
  if (!fromHex(req[0], blockblob)) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_WRONG_BLOCKBLOB, "Wrong block blob"};
  }

  auto submitResult = m_core.submitBlock(BinaryArray{blockblob});
  if (submitResult != error::AddBlockErrorCondition::BLOCK_ADDED) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_BLOCK_NOT_ACCEPTED, "Block not accepted"};
  }

  if (submitResult == error::AddBlockErrorCode::ADDED_TO_MAIN ||
      submitResult == error::AddBlockErrorCode::ADDED_TO_ALTERNATIVE_AND_SWITCHED) {
    auto blockTemplate = fromBinaryArray<BlockTemplate>(blockblob);  // safe as long as core checked it for us.
    LiteBlock blockToRelay;
    blockToRelay.blockTemplate = std::move(blockblob);
    m_protocol.relayBlock(std::move(blockToRelay));
  }

  res.status = CORE_RPC_STATUS_OK;
  return true;
}

RawBlock RpcServer::prepareRawBlock(BinaryArray&& blockBlob) {
  BlockTemplate blockTemplate;
  bool result = fromBinaryArray(blockTemplate, blockBlob);
  if (result) {
  }
  assert(result);

  RawBlock rawBlock;
  rawBlock.blockTemplate = std::move(blockBlob);

  if (blockTemplate.transactionHashes.empty()) {
    return rawBlock;
  }

  rawBlock.transactions.reserve(blockTemplate.transactionHashes.size());
  std::vector<Crypto::Hash> missedTransactions;
  m_core.getTransactions(blockTemplate.transactionHashes, rawBlock.transactions, missedTransactions);
  assert(missedTransactions.empty());

  return rawBlock;
}

namespace {

uint64_t get_block_reward(const BlockTemplate& blk) {
  uint64_t reward = 0;
  for (const TransactionOutput& out : blk.baseTransaction.outputs) {
    reward += out.amount;
  }
  return reward;
}

}  // namespace

void RpcServer::fill_block_header_response(const BlockTemplate& blk, bool orphan_status, BlockHeight height,
                                           const Hash& hash, block_header_response& response) {
  response.version = blk.version;
  response.upgrade_vote = blk.upgradeVote;
  response.timestamp = blk.timestamp;
  response.prev_hash = blk.previousBlockHash;
  response.nonce = blk.nonce;
  response.orphan_status = orphan_status;
  response.height = height;
  response.depth = BlockHeight::fromIndex(m_core.getTopBlockIndex()).native() - height.native();
  response.hash = hash;
  response.difficulty = m_core.getBlockDifficulty(height.toIndex());
  response.reward = get_block_reward(blk);
  response.static_reward = m_core.currency().staticRewardAmountForBlockVersion(blk.version);
  response.transactions_count = blk.transactionHashes.size() + 1;
  if (blk.staticRewardHash) {
    response.transactions_count += 1;
  }
  response.block_size = 0;
}

bool RpcServer::on_get_last_block_header(const COMMAND_RPC_GET_LAST_BLOCK_HEADER::request& req,
                                         COMMAND_RPC_GET_LAST_BLOCK_HEADER::response& res) {
  XI_UNUSED(req);
  const auto topHash = m_core.getTopBlockHash();
  auto topBlock = m_core.getBlockByHash(topHash);
  if (!topBlock) {
    notFound("block", topHash.toString());
  }

  fill_block_header_response(topBlock->block.block().getBlock(), false,
                             BlockHeight::fromIndex(m_core.getTopBlockIndex()), m_core.getTopBlockHash(),
                             res.block_header);
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_get_block_header_by_hash(const COMMAND_RPC_GET_BLOCK_HEADER_BY_HASH::request& req,
                                            COMMAND_RPC_GET_BLOCK_HEADER_BY_HASH::response& res) {
  Hash blockHash = req.hash;

  if (!m_core.hasBlock(blockHash)) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_INTERNAL_ERROR,
                                "Internal error: can't get block by hash. Hash = " + req.hash.toString() + '.'};
  }

  auto block = m_core.getBlockByHash(blockHash);
  if (!block) {
    notFound("block", blockHash.toString());
  }
  const CachedBlock& cachedBlock = block->block.block();
  fill_block_header_response(cachedBlock.getBlock(), false, BlockHeight::fromIndex(cachedBlock.getBlockIndex()),
                             cachedBlock.getBlockHash(), res.block_header);
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_get_block_header_by_height(const COMMAND_RPC_GET_BLOCK_HEADER_BY_HEIGHT::request& req,
                                              COMMAND_RPC_GET_BLOCK_HEADER_BY_HEIGHT::response& res) {
  if (m_core.getTopBlockIndex() < req.height.toIndex()) {
    throw JsonRpc::JsonRpcError{CORE_RPC_ERROR_CODE_TOO_BIG_HEIGHT,
                                std::string("To big height: ") + std::to_string(req.height.native()) +
                                    ", current blockchain height = " +
                                    std::to_string(BlockHeight::fromIndex(m_core.getTopBlockIndex()).native())};
  }
  auto block = m_core.getBlockByIndex(req.height.toIndex());
  if (!block) {
    notFound("block", std::to_string(req.height.native()));
  }

  const CachedBlock& cachedBlock = block->block.block();
  fill_block_header_response(cachedBlock.getBlock(), false, req.height, cachedBlock.getBlockHash(), res.block_header);
  res.status = CORE_RPC_STATUS_OK;
  return true;
}

bool RpcServer::on_get_block_headers_range(const COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::request& req,
                                           COMMAND_RPC_GET_BLOCK_HEADERS_RANGE::response& res,
                                           JsonRpc::JsonRpcError& error_resp) {
  // TODO: change usage to jsonRpcHandlers?
  const auto bc_height = m_core.get_current_blockchain_height();
  if (req.start_height > bc_height || req.end_height >= bc_height || req.start_height > req.end_height) {
    error_resp.code = CORE_RPC_ERROR_CODE_TOO_BIG_HEIGHT;
    error_resp.message = "Invalid start/end heights.";
    return false;
  }

  for (auto h = req.start_height; h <= req.end_height && !h.isNull(); h += BlockOffset::fromNative(1)) {
    auto search = m_core.getBlockByIndex(h.toIndex());
    if (!search) {
      notFound("block", std::to_string(h.native()));
    }

    res.headers.push_back(block_header_response{});
    fill_block_header_response(search->block.block().getBlock(), false, h, search->block.block().getBlockHash(),
                               res.headers.back());

    // TODO: Error handling like in monero?
    /*block blk;
    bool have_block = m_core.get_block_by_hash(block_hash, blk);
    if (!have_block)
    {
            error_resp.code = CORE_RPC_ERROR_CODE_INTERNAL_ERROR;
            error_resp.message = "Internal error: can't get block by height. Height = " +
    boost::lexical_cast<std::string>(h) + ". Hash = " + epee::string_tools::pod_to_hex(block_hash) + '.'; return
    false;
    }
    if (blk.miner_tx.vin.size() != 1 || blk.miner_tx.vin.front().type() != typeid(txin_gen))
    {
            error_resp.code = CORE_RPC_ERROR_CODE_INTERNAL_ERROR;
            error_resp.message = "Internal error: coinbase transaction in the block has the wrong type";
            return false;
    }
    uint64_t block_height = boost::get<txin_gen>(blk.miner_tx.vin.front()).height;
    if (block_height != h)
    {
            error_resp.code = CORE_RPC_ERROR_CODE_INTERNAL_ERROR;
            error_resp.message = "Internal error: coinbase transaction in the block has the wrong height";
            return false;
    }
    res.headers.push_back(block_header_response());
    bool response_filled = fill_block_header_response(blk, false, block_height, block_hash, res.headers.back());
    if (!response_filled)
    {
            error_resp.code = CORE_RPC_ERROR_CODE_INTERNAL_ERROR;
            error_resp.message = "Internal error: can't produce valid response.";
            return false;
    }*/
  }

  res.status = CORE_RPC_STATUS_OK;
  return true;
}

}  // namespace CryptoNote
