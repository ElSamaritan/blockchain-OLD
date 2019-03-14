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

#include "MinerMonitor.h"

#include <algorithm>
#include <numeric>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/asio/ip/host_name.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Common/StringTools.h>

XiMiner::MinerMonitor::MinerMonitor(MinerManager& miner, Logging::ILogger& logger)
    : m_miner{miner}, m_logger{logger, "Monitor"} {
  m_minerId = boost::asio::ip::host_name();
}

void XiMiner::MinerMonitor::onSuccessfulBlockSubmission(Crypto::Hash hash) {
  m_logger(Logging::INFO, Logging::GREEN)
      << "Block successfully submitted: " << Common::toHex(hash.data(), hash.size());
}

void XiMiner::MinerMonitor::onBlockTemplateChanged() { m_lastBlockUpdate = std::chrono::system_clock::now(); }

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

void XiMiner::MinerMonitor::setReportInterval(std::chrono::seconds interval) {
  if (interval.count() < 1) {
    return;
  }
  m_reportSeconds.store(static_cast<uint64_t>(interval.count()));
}

std::chrono::seconds XiMiner::MinerMonitor::reportInterval() const { return std::chrono::seconds{m_reportSeconds}; }

void XiMiner::MinerMonitor::setTelemetryIdentifier(const std::string& id) { m_minerId = id; }

const std::string& XiMiner::MinerMonitor::telemetryIdentifier() const { return m_minerId; }

std::string XiMiner::MinerMonitor::status() const { return ""; }

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
  auto timelineAverage = std::accumulate(m_hrTimeline.rbegin(), m_hrTimeline.rend(), 0.0,
                                         [](auto acc, const auto& iCheckpoint) { return acc + iCheckpoint.Hashrate; }) /
                         (double)m_hrTimeline.size();
  m_logger(Logging::TRACE, Logging::CYAN) << timelineAverage << " H/s";
}

void XiMiner::MinerMonitor::pushHashrateCheckpoint() {
  m_hrTimeline.push_back(m_miner.resetHashrateSummary());
  if (m_hrTimeline.size() > 3600) {
    m_hrTimeline.pop_front();
  }
}

void XiMiner::MinerMonitor::checkForStall() {
  const auto now = std::chrono::system_clock::now();
  if (now - m_lastStallNotification < std::chrono::minutes{1}) {
    return;
  }
  const auto lastUpdateDuration = now - m_lastBlockUpdate;
  if (lastUpdateDuration > std::chrono::minutes{6}) {
    m_logger(Logging::ERROR) << "last block update is "
                             << std::chrono::duration_cast<std::chrono::minutes>(lastUpdateDuration).count()
                             << " minutes ago.";
  } else if (lastUpdateDuration > std::chrono::minutes{3}) {
    m_logger(Logging::WARNING) << "last block update is "
                               << std::chrono::duration_cast<std::chrono::minutes>(lastUpdateDuration).count()
                               << " minutes ago.";
  }
}
