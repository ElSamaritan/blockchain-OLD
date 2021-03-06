﻿/* ============================================================================================== *
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

#include "MinerMonitor.h"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <cstdlib>

#include <Xi/ExternalIncludePush.h>
#include <boost/asio/ip/host_name.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Common/StringTools.h>

XiMiner::MinerMonitor::MinerMonitor(MinerManager& miner, Logging::ILogger& logger)
    : m_miner{miner}, m_logger{logger, "Monitor"} {
  m_minerId = boost::asio::ip::host_name();
}

void XiMiner::MinerMonitor::onSuccessfulBlockSubmission(CryptoNote::BlockTemplate block) {
  CryptoNote::CachedBlock cblock{block};
  BlockSubmissionResult submission{};
  submission.hash(cblock.getBlockHash());
  submission.reward(emission(block.baseTransaction));
  m_blocksMined += 1;
  m_cumulativeReward += submission.reward();

  m_logger.object(Logging::Info, submission, "Block Submitted");
  m_observer.notify(&Observer::onBlockSubmission, submission);
  if (blockLimit() > 0 && blocksMined() >= blockLimit()) {
    m_logger(Logging::Fatal) << "Blocks limit reached, shutting down.";
    std::this_thread::sleep_for(std::chrono::seconds{5});
    std::exit(EXIT_SUCCESS);  // sorry
  }
}

void XiMiner::MinerMonitor::onBlockTemplateChanged(MinerBlockTemplate block) {
  m_lastBlockUpdate = std::chrono::system_clock::now();
  {
    std::lock_guard<std::mutex> lck{m_statusAccess};
    XI_UNUSED(lck);
    m_status.top_block = block.Template.previousBlockHash;
    m_status.difficulty = block.Difficutly;
    m_status.algorithm = block.Algorithm;
    m_status.reward = emission(block.Template.baseTransaction);
  }
}

void XiMiner::MinerMonitor::run() {
  if (m_shouldRun.load()) {
    return;
  }
  m_lastBlockUpdate = std::chrono::system_clock::now();
  m_lastStallNotification = std::chrono::system_clock::now();
  m_miner.addObserver(this);
  m_shouldRun.store(true);
  m_monitor = std::thread{[this]() { this->monitorLoop(); }};
}

void XiMiner::MinerMonitor::shutdown() {
  m_shouldRun.store(false);
  m_miner.removeObserver(this);
  if (m_monitor.joinable()) {
    m_monitor.join();
  }
}

void XiMiner::MinerMonitor::addObserver(XiMiner::MinerMonitor::Observer* observer) { m_observer.add(observer); }
void XiMiner::MinerMonitor::removeObserver(XiMiner::MinerMonitor::Observer* observer) { m_observer.remove(observer); }

void XiMiner::MinerMonitor::setReportInterval(std::chrono::seconds interval) {
  if (interval.count() < 1) {
    return;
  }
  m_reportSeconds.store(static_cast<uint64_t>(interval.count()));
}

std::chrono::seconds XiMiner::MinerMonitor::reportInterval() const { return std::chrono::seconds{m_reportSeconds}; }

XiMiner::MinerMonitor& XiMiner::MinerMonitor::statusReport(bool enabled) {
  m_statusReport.store(enabled, std::memory_order_release);
  return *this;
}

bool XiMiner::MinerMonitor::statusReport() const { return m_statusReport.load(std::memory_order_consume); }

void XiMiner::MinerMonitor::setPanicExitEnabled(bool enabled) { m_panicExit.store(enabled); }

bool XiMiner::MinerMonitor::isPanicExitEnabled() const { return m_panicExit.load(); }

void XiMiner::MinerMonitor::setTelemetryIdentifier(const std::string& id) { m_minerId = id; }

const std::string& XiMiner::MinerMonitor::telemetryIdentifier() const { return m_minerId; }

void XiMiner::MinerMonitor::setBlocksLimit(uint32_t limit) { m_blocksLimit.store(limit); }

uint32_t XiMiner::MinerMonitor::blockLimit() const { return m_blocksLimit.load(); }

uint32_t XiMiner::MinerMonitor::blocksMined() const { return m_blocksMined.load(); }

uint64_t XiMiner::MinerMonitor::cumulativeReward() const { return m_cumulativeReward.load(); }

XiMiner::MinerStatus XiMiner::MinerMonitor::status() const {
  std::lock_guard<std::mutex> lck{m_statusAccess};
  XI_UNUSED(lck);
  return m_status;
}

void XiMiner::MinerMonitor::reset() {
  {
    std::lock_guard<std::mutex> lck{m_statusAccess};
    XI_UNUSED(lck);
    m_hrTimeline.clear();
  }
}

void XiMiner::MinerMonitor::monitorLoop() {
  auto lastUpdatePush = std::chrono::high_resolution_clock::now();
  while (m_shouldRun.load()) {
    std::this_thread::sleep_for(std::chrono::seconds{1});
    pushHashrateCheckpoint();
    checkForStall();
    auto now = std::chrono::high_resolution_clock::now();
    if (now - lastUpdatePush > reportInterval()) {
      reportHashrate();
      lastUpdatePush = now;
    }
  }
}

void XiMiner::MinerMonitor::reportHashrate() {
  auto currentStatus = status();
  m_logger.object(Logging::Info, currentStatus, "status");
  if (m_statusReport) {
    m_observer.notify(&Observer::onStatusReport, currentStatus);
  }
}

void XiMiner::MinerMonitor::pushHashrateCheckpoint() {
  {
    std::lock_guard<std::mutex> lck{m_statusAccess};
    XI_UNUSED(lck);
    m_hrTimeline.push_back(m_miner.resetHashrateSummary());
    if (m_hrTimeline.size() > 600) {
      m_hrTimeline.pop_front();
    }
    m_status.average_hashrate =
        std::accumulate(m_hrTimeline.rbegin(), m_hrTimeline.rend(), 0.0,
                        [](auto acc, const auto& iCheckpoint) { return acc + iCheckpoint.Hashrate; }) /
        static_cast<double>(m_hrTimeline.size());
    if (m_hrTimeline.size() > 0) {
      m_status.current_hashrate = m_hrTimeline.rbegin()->Hashrate;
    }
    m_status.blocks_mined = blocksMined();
    m_status.cumulative_reward = cumulativeReward();
    m_status.active_threads = m_miner.threads();
    if (m_hrTimeline.size() == 600) {
      const double shortAverage =
          std::accumulate(m_hrTimeline.rbegin(), m_hrTimeline.rbegin() + 60, 0.0,
                          [](auto acc, const auto& iCheckpoint) { return acc + iCheckpoint.Hashrate; }) /
          static_cast<double>(m_hrTimeline.size());
      const auto abs = (shortAverage - m_status.average_hashrate) / m_status.average_hashrate;
      if ((abs < -0.5 || shortAverage < 0.1) && isPanicExitEnabled()) {
        m_logger(Logging::Fatal) << "Hashrate stall detected, panic out.";
        std::this_thread::sleep_for(std::chrono::seconds{5});
        std::exit(EXIT_FAILURE);  // sorry
      }
    }
  }
}

void XiMiner::MinerMonitor::checkForStall() {
  const auto now = std::chrono::system_clock::now();
  if (now - m_lastStallNotification < std::chrono::minutes{1}) {
    return;
  }
  const auto lastUpdateDuration = now - m_lastBlockUpdate;
  if (lastUpdateDuration > std::chrono::minutes{8}) {
    m_logger(Logging::Error) << "last block update is "
                             << std::chrono::duration_cast<std::chrono::minutes>(lastUpdateDuration).count()
                             << " minutes ago.";
    if (isPanicExitEnabled()) {
      std::this_thread::sleep_for(std::chrono::seconds{5});
      std::exit(EXIT_FAILURE);  // sorry
    }
    m_lastStallNotification = now;
  } else if (lastUpdateDuration > std::chrono::minutes{3}) {
    m_logger(Logging::Warning) << "last block update is "
                               << std::chrono::duration_cast<std::chrono::minutes>(lastUpdateDuration).count()
                               << " minutes ago.";
    m_lastStallNotification = now;
  }
}
