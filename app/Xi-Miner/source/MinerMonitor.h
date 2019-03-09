/* ============================================================================================== *
 *                                                                                                *
 *                                       Xi Blockchain                                            *
 *                                                                                                *
 * ---------------------------------------------------------------------------------------------- *
 * This file is part of the Galaxia Project - Xi Blockchain                                       *
 * ---------------------------------------------------------------------------------------------- *
 *                                                                                                *
 * Copyright 2018-2019 Galaxia Project Developers                                                 *
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

#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>

#include "HashrateSummary.h"
#include "MinerManager.h"

namespace XiMiner {
class MinerMonitor : public MinerManager::Observer {
 public:
  MinerMonitor(MinerManager& miner, Logging::ILogger& logger);

  void onSuccessfulBlockSubmission(Crypto::Hash hash) override;
  void onBlockTemplateChanged() override;

  void run();
  void shutdown();

  void setReportInterval(std::chrono::seconds interval);
  std::chrono::seconds reportInterval() const;

  void setTelemetryIdentifier(const std::string& id);
  const std::string& telemetryIdentifier() const;

  std::string status() const;

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

  std::chrono::system_clock::time_point m_lastBlockUpdate;
  std::chrono::system_clock::time_point m_lastStallNotification;

  std::deque<CollectiveHashrateSummary> m_hrTimeline;
  std::string m_minerId;  ///< Used to identify the instance on telemetry services.
};
}  // namespace XiMiner
