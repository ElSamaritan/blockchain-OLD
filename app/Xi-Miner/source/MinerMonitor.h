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

#include <cinttypes>
#include <atomic>
#include <thread>
#include <chrono>
#include <string>
#include <deque>
#include <functional>
#include <mutex>

#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>
#include <Common/ObserverManager.h>

#include "HashrateSummary.h"
#include "MinerManager.h"
#include "MinerStatus.h"
#include "BlockSubmissionResult.h"

namespace XiMiner {
class MinerMonitor : public MinerManager::Observer {
 public:
  class Observer {
   public:
    virtual ~Observer() = default;

    virtual void onBlockSubmission(BlockSubmissionResult result) = 0;
    virtual void onStatusReport(MinerStatus status) = 0;
  };

 public:
  MinerMonitor(MinerManager& miner, Logging::ILogger& logger);

  void onSuccessfulBlockSubmission(CryptoNote::BlockTemplate block) override;
  void onBlockTemplateChanged(MinerBlockTemplate block) override;

  void run();
  void shutdown();

  void addObserver(Observer* observer);
  void removeObserver(Observer* observer);

  void setReportInterval(std::chrono::seconds interval);
  std::chrono::seconds reportInterval() const;

  MinerMonitor& statusReport(bool enabled);
  bool statusReport() const;

  void setPanicExitEnabled(bool enabled);
  bool isPanicExitEnabled() const;

  void setTelemetryIdentifier(const std::string& id);
  const std::string& telemetryIdentifier() const;

  void setBlocksLimit(uint32_t limit);
  uint32_t blockLimit() const;

  uint32_t blocksMined() const;
  uint64_t cumulativeReward() const;

  MinerStatus status() const;

  void reset();

 private:
  void monitorLoop();
  void reportHashrate();
  void pushHashrateCheckpoint();
  void checkForStall();

 private:
  MinerManager& m_miner;
  Logging::LoggerRef m_logger;
  std::thread m_monitor;
  std::atomic<uint64_t> m_reportSeconds{1};
  std::atomic_bool m_shouldRun{false};
  std::atomic_bool m_panicExit{false};

  std::atomic_bool m_statusReport{false};
  Tools::ObserverManager<Observer> m_observer;

  std::chrono::system_clock::time_point m_lastBlockUpdate;
  std::chrono::system_clock::time_point m_lastStallNotification;

  std::deque<CollectiveHashrateSummary> m_hrTimeline;
  std::string m_minerId;  ///< Used to identify the instance on telemetry services.
  std::atomic<uint32_t> m_blocksLimit{0};
  std::atomic<uint32_t> m_blocksMined{0};
  std::atomic<uint64_t> m_cumulativeReward{0};

  mutable std::mutex m_statusAccess;
  MinerStatus m_status;
};
}  // namespace XiMiner
