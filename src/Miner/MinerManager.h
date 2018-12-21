﻿// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#pragma once

#include <queue>
#include <string>

#include <System/ContextGroup.h>
#include <System/Event.h>

#include <Xi/Http/Client.h>

#include "BlockchainMonitor.h"
#include "Logging/LoggerRef.h"
#include "Miner.h"
#include "MinerEvent.h"
#include "MiningConfig.h"

namespace System {
class Dispatcher;
}

namespace Miner {

class MinerManager {
 public:
  MinerManager(System::Dispatcher& dispatcher, const CryptoNote::MiningConfig& config, Logging::ILogger& logger);
  ~MinerManager();

  void start();

 private:
  System::Dispatcher& m_dispatcher;
  ::Xi::Http::Client m_httpClient;
  Logging::LoggerRef m_logger;
  System::ContextGroup m_contextGroup;
  CryptoNote::MiningConfig m_config;
  CryptoNote::Miner m_miner;
  int m_blockCounter;
  BlockchainMonitor m_blockchainMonitor;

  System::Event m_eventOccurred;
  System::Event m_httpEvent;
  std::queue<MinerEvent> m_events;
  bool isRunning;

  CryptoNote::BlockTemplate m_minedBlock;

  uint64_t m_lastBlockTimestamp;

  void eventLoop();
  MinerEvent waitEvent();
  void pushEvent(MinerEvent&& event);
  void printHashRate();

  void startMining(const CryptoNote::BlockMiningParameters& params);
  void stopMining();

  void startBlockchainMonitoring();
  void stopBlockchainMonitoring();

  bool submitBlock(const CryptoNote::BlockTemplate& minedBlock);
  CryptoNote::BlockMiningParameters requestMiningParameters(System::Dispatcher& dispatcher,
                                                            const std::string& miningAddress);

  void adjustBlockTemplate(CryptoNote::BlockTemplate& blockTemplate) const;
};

}  // namespace Miner
