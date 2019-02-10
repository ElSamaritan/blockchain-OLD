// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
// Copyright (c) 2018, The Calex Developers
//
// Please see the included LICENSE file for more information.

#include "MinerManager.h"

#include <System/EventLock.h>
#include <System/InterruptedException.h>
#include <System/Timer.h>
#include <thread>
#include <chrono>

#include "Common/StringTools.h"
#include <Xi/Config.h>
#include "CryptoNoteCore/CachedBlock.h"
#include "CryptoNoteCore/CryptoNoteTools.h"
#include "CryptoNoteCore/CryptoNoteFormatUtils.h"
#include "CryptoNoteCore/Transactions/TransactionExtra.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"
#include "Rpc/JsonRpc.h"
#include <Logging/LoggerManager.h>

using namespace CryptoNote;
using namespace Logging;

namespace Miner {

namespace {

MinerEvent BlockMinedEvent() {
  MinerEvent event;
  event.type = MinerEventType::BLOCK_MINED;
  return event;
}

MinerEvent BlockchainUpdatedEvent() {
  MinerEvent event;
  event.type = MinerEventType::BLOCKCHAIN_UPDATED;
  return event;
}

void adjustMergeMiningTag(BlockTemplate& blockTemplate) {
  CachedBlock cachedBlock(blockTemplate);
  if (blockTemplate.majorVersion >= ::Xi::Config::BlockVersion::BlockVersionCheckpoint<1>::version()) {
    CryptoNote::TransactionExtraMergeMiningTag mmTag;
    mmTag.depth = 0;
    mmTag.merkleRoot = cachedBlock.getAuxiliaryBlockHeaderHash();

    blockTemplate.parentBlock.baseTransaction.extra.clear();
    if (!CryptoNote::appendMergeMiningTagToExtra(blockTemplate.parentBlock.baseTransaction.extra, mmTag)) {
      throw std::runtime_error("Couldn't append merge mining tag");
    }
  }
}

}  // namespace

MinerManager::MinerManager(System::Dispatcher& dispatcher, const CryptoNote::MiningConfig& config,
                           Logging::ILogger& logger)
    : m_dispatcher(dispatcher),
      m_httpClient(config.daemonHost, config.daemonPort, config.ssl),
      m_logger(logger, "MinerManager"),
      m_contextGroup(dispatcher),
      m_config(config),
      m_miner(dispatcher, logger),
      m_blockchainMonitor(dispatcher, m_httpClient, m_config.scanPeriod, logger),
      m_blockCounter(0),
      m_eventOccurred(dispatcher),
      m_httpEvent(dispatcher),
      m_lastBlockTimestamp(0) {
  m_httpEvent.set();
}

MinerManager::~MinerManager() {}

void MinerManager::start() {
  m_logger(Logging::DEBUGGING) << "starting";

  BlockMiningParameters params;
  for (;;) {
    m_logger(Logging::INFO) << "requesting mining parameters";

    try {
      params = requestMiningParameters(m_dispatcher, m_config.miningAddress);
    } catch (JsonRpc::JsonRpcError& ex) {
      m_logger(Logging::WARNING) << "Daemon returned non-success state: " << ex.what();
      System::Timer timer(m_dispatcher);
      timer.sleep(std::chrono::seconds(m_config.scanPeriod));
      continue;
    } catch (std::exception& e) {
      m_logger(Logging::WARNING) << "Couldn't connect to daemon: " << e.what();
      System::Timer timer(m_dispatcher);
      timer.sleep(std::chrono::seconds(m_config.scanPeriod));
      continue;
    }

    adjustBlockTemplate(params.blockTemplate);
    break;
  }

  isRunning = true;

  startBlockchainMonitoring();
  std::thread reporter(std::bind(&MinerManager::printHashRate, this));
  startMining(params);

  eventLoop();
  isRunning = false;
}

void MinerManager::printHashRate() {
  uint64_t last_hash_count = m_miner.getHashCount();

  while (isRunning) {
    std::this_thread::sleep_for(std::chrono::seconds(60));
    uint64_t current_hash_count = m_miner.getHashCount();
    double hashes = static_cast<double>(current_hash_count - last_hash_count) / 60.0;
    last_hash_count = current_hash_count;
    m_logger(Logging::INFO, Logging::CYAN) << "Mining at " << hashes << " H/s";
  }
}

void MinerManager::eventLoop() {
  size_t blocksMined = 0;

  for (;;) {
    m_logger(Logging::DEBUGGING) << "waiting for event";
    MinerEvent event = waitEvent();

    switch (event.type) {
      case MinerEventType::BLOCK_MINED: {
        m_logger(Logging::DEBUGGING) << "got BLOCK_MINED event";
        stopBlockchainMonitoring();

        if (submitBlock(m_minedBlock)) {
          m_lastBlockTimestamp = m_minedBlock.timestamp;

          if (m_config.blocksLimit != 0 && ++blocksMined == m_config.blocksLimit) {
            m_logger(Logging::INFO) << "Miner mined requested " << m_config.blocksLimit << " blocks. Quitting";
            return;
          }
        }

        BlockMiningParameters params = requestMiningParameters(m_dispatcher, m_config.miningAddress);
        adjustBlockTemplate(params.blockTemplate);

        startBlockchainMonitoring();
        startMining(params);
        break;
      }

      case MinerEventType::BLOCKCHAIN_UPDATED: {
        m_logger(Logging::DEBUGGING) << "got BLOCKCHAIN_UPDATED event";
        stopMining();
        stopBlockchainMonitoring();
        BlockMiningParameters params = requestMiningParameters(m_dispatcher, m_config.miningAddress);
        adjustBlockTemplate(params.blockTemplate);

        startBlockchainMonitoring();
        startMining(params);
        break;
      }

      default:
        assert(false);
        return;
    }
  }
}

MinerEvent MinerManager::waitEvent() {
  while (m_events.empty()) {
    m_eventOccurred.wait();
    m_eventOccurred.clear();
  }

  MinerEvent event = std::move(m_events.front());
  m_events.pop();

  return event;
}

void MinerManager::pushEvent(MinerEvent&& event) {
  m_events.push(std::move(event));
  m_eventOccurred.set();
}

void MinerManager::startMining(const CryptoNote::BlockMiningParameters& params) {
  m_contextGroup.spawn([this, params]() {
    try {
      m_minedBlock = m_miner.mine(params, m_config.threadCount);
      pushEvent(BlockMinedEvent());
    } catch (System::InterruptedException&) {
    } catch (std::exception& e) {
      m_logger(Logging::ERROR) << "Miner context unexpectedly finished: " << e.what();
    }
  });
}

void MinerManager::stopMining() { m_miner.stop(); }

void MinerManager::startBlockchainMonitoring() {
  m_contextGroup.spawn([this]() {
    try {
      m_blockchainMonitor.waitBlockchainUpdate();
      pushEvent(BlockchainUpdatedEvent());
    } catch (System::InterruptedException&) {
    } catch (std::exception& e) {
      m_logger(Logging::ERROR) << "BlockchainMonitor context unexpectedly finished: " << e.what();
      std::this_thread::sleep_for(std::chrono::seconds{10});
      startBlockchainMonitoring();
    }
  });
}

void MinerManager::stopBlockchainMonitoring() { m_blockchainMonitor.stop(); }

bool MinerManager::submitBlock(const BlockTemplate& minedBlock) {
  CachedBlock cachedBlock(minedBlock);

  try {
    COMMAND_RPC_SUBMITBLOCK::request request;
    request.emplace_back(Common::toHex(toBinaryArray(minedBlock)));

    COMMAND_RPC_SUBMITBLOCK::response response;

    System::EventLock lk(m_httpEvent);
    JsonRpc::invokeJsonRpcCommand(m_httpClient, "submitblock", request, response);

    m_logger(Logging::INFO) << "Block has been successfully submitted. Block hash: "
                            << Common::podToHex(cachedBlock.getBlockHash());
    return true;
  } catch (std::exception& e) {
    m_logger(Logging::WARNING) << "Couldn't submit block: " << Common::podToHex(cachedBlock.getBlockHash())
                               << ", reason: " << e.what();
    return false;
  }
}

BlockMiningParameters MinerManager::requestMiningParameters(System::Dispatcher& dispatcher,
                                                            const std::string& miningAddress) {
  XI_UNUSED(dispatcher);
  try {
    COMMAND_RPC_GETBLOCKTEMPLATE::request request;
    request.reserve_size = 0;
    request.wallet_address = miningAddress;

    COMMAND_RPC_GETBLOCKTEMPLATE::response response;

    System::EventLock lk(m_httpEvent);
    JsonRpc::invokeJsonRpcCommand(m_httpClient, "getblocktemplate", request, response);

    if (response.status != CORE_RPC_STATUS_OK) {
      throw std::runtime_error("Core responded with wrong status: " + response.status);
    }

    BlockMiningParameters params;
    params.difficulty = response.difficulty;

    if (!fromBinaryArray(params.blockTemplate, Common::fromHex(response.blocktemplate_blob))) {
      throw std::runtime_error("Couldn't deserialize block template");
    }

    m_logger(Logging::DEBUGGING) << "Requested block template with previous block hash: "
                                 << Common::podToHex(params.blockTemplate.previousBlockHash);
    m_blockCounter++;
    return params;
  } catch (std::exception& e) {
    m_logger(Logging::WARNING) << "Couldn't get block template: " << e.what();
    throw;
  }
}

void MinerManager::adjustBlockTemplate(CryptoNote::BlockTemplate& blockTemplate) const {
  adjustMergeMiningTag(blockTemplate);

  if (m_config.firstBlockTimestamp == 0) {
    // no need to fix timestamp
    return;
  }

  if (m_lastBlockTimestamp == 0) {
    blockTemplate.timestamp = m_config.firstBlockTimestamp;
  } else if (m_lastBlockTimestamp != 0 && m_config.blockTimestampInterval != 0) {
    blockTemplate.timestamp = m_lastBlockTimestamp + m_config.blockTimestampInterval;
  }
}

}  // namespace Miner
