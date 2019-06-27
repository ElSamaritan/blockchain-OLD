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

#include <atomic>
#include <vector>
#include <random>
#include <thread>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/Http/Client.h>
#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>
#include <crypto/CryptoTypes.h>
#include <Common/ObserverManager.h>
#include <Rpc/RpcRemoteConfiguration.h>

#include "UpdateMonitor.h"
#include "MinerWorker.h"
#include "HashrateSummary.h"

namespace XiMiner {
class MinerManager : public UpdateMonitor::Observer, MinerWorker::Observer {
 public:
  class Observer {
   public:
    virtual ~Observer() = default;

    virtual void onSuccessfulBlockSubmission(Crypto::Hash hash) = 0;
    virtual void onBlockTemplateChanged(Crypto::Hash hash) = 0;
  };

 public:
  MinerManager(const CryptoNote::RpcRemoteConfiguration remote, Logging::ILogger& logger);
  XI_DELETE_COPY(MinerManager);
  XI_DELETE_MOVE(MinerManager);
  ~MinerManager() override = default;

  void onTemplateChanged(MinerBlockTemplate newTemplate) override;
  void onBlockFound(CryptoNote::BlockTemplate block) override;

  void run();
  void shutdown();

  void addObserver(Observer* observer);
  void removeObserver(Observer* observer);

  void setThreads(uint32_t threadCount);
  uint32_t threads() const;

  CollectiveHashrateSummary resetHashrateSummary();

 private:
  Xi::Http::Client m_http;
  Logging::LoggerRef m_logger;

  Tools::ObserverManager<Observer> m_observer;
  uint32_t m_threads = static_cast<uint32_t>(std::thread::hardware_concurrency());
  std::atomic_bool m_running{false};
  std::atomic_bool m_shutdownRequest{false};
  std::vector<std::shared_ptr<MinerWorker>> m_worker;
  std::default_random_engine m_randomEngine;
  std::uniform_int_distribution<uint32_t> m_nonceDist;
};
}  // namespace XiMiner
