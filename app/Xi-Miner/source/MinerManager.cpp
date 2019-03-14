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

#include "MinerManager.h"

#include <thread>
#include <chrono>
#include <numeric>
#include <algorithm>

#include <Rpc/JsonRpc.h>
#include <Rpc/Commands/SubmitBlock.h>
#include <Common/StringTools.h>
#include <CryptoNoteCore/CryptoNoteTools.h>

XiMiner::MinerManager::MinerManager(const CryptoNote::RpcRemoteConfiguration remote, Logging::ILogger& logger)
    : m_http{remote.Host, remote.Port, remote.Ssl},
      m_logger{logger, "MinerManager"},
      m_nonceDist{0, std::numeric_limits<uint32_t>::max()} {}

void XiMiner::MinerManager::onTemplateChanged(MinerBlockTemplate newTemplate) {
  m_logger(Logging::TRACE) << "template updated";
  uint32_t nonce = m_nonceDist(m_randomEngine);
  for (uint32_t i = 0; i < m_worker.size(); ++i) {
    auto iWorker = m_worker[i];
    iWorker->setInitialNonce(nonce + i);
    iWorker->setNonceStep(i);
    iWorker->setTemplate(newTemplate);
  }
  m_observer.notify(&MinerManager::Observer::onBlockTemplateChanged);
}

void XiMiner::MinerManager::onBlockFound(CryptoNote::BlockTemplate block) {
  try {
    m_logger(Logging::INFO, Logging::GREEN) << "block found";
    CryptoNote::RpcCommands::SubmitBlock::request request;
    CryptoNote::RpcCommands::SubmitBlock::response response;
    request.hex_binary_template = Common::toHex(CryptoNote::toBinaryArray(block));
    CryptoNote::JsonRpc::invokeJsonRpcCommand(m_http, CryptoNote::RpcCommands::SubmitBlock::identifier(), request,
                                              response);
    CryptoNote::CachedBlock cblock{block};
    m_observer.notify(&MinerManager::Observer::onSuccessfulBlockSubmission, cblock.getBlockLongHash());
    m_logger(Logging::INFO) << "block submission result: " << response.result;
  } catch (std::exception& e) {
    m_logger(Logging::ERROR) << "error on block submission: " << e.what();
  } catch (...) {
    m_logger(Logging::ERROR) << "unknown error on block submission.";
  }
}

void XiMiner::MinerManager::run() {
  m_logger(Logging::INFO) << "starting miner manager";
  m_running.store(true);
  m_shutdownRequest.store(false);

  m_worker.reserve(std::thread::hardware_concurrency());
  uint32_t nonce = m_nonceDist(m_randomEngine);
  for (uint32_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
    auto iWorker = std::make_shared<MinerWorker>();
    iWorker->addObserver(this);
    iWorker->setInitialNonce(nonce + i);
    iWorker->setNonceStep(i);
    if (i >= m_threads) {
      iWorker->pause();
    }
    iWorker->run();
    m_worker.emplace_back(std::move(iWorker));
  }

  while (m_running) {
    std::this_thread::sleep_for(std::chrono::seconds{1});
  }

  std::vector<std::future<void>> shutdowns;
  for (auto& worker : m_worker) {
    shutdowns.emplace_back(worker->shutdown());
  }
  for (auto& shutdownProcess : shutdowns) {
    try {
      shutdownProcess.get();
    } catch (...) {
      /* swallow */
    }
  }
  m_worker.clear();
}

void XiMiner::MinerManager::shutdown() {
  m_shutdownRequest.store(true);
  m_running.store(false);
}

void XiMiner::MinerManager::addObserver(XiMiner::MinerManager::Observer* observer) { m_observer.add(observer); }

void XiMiner::MinerManager::removeObserver(XiMiner::MinerManager::Observer* observer) { m_observer.remove(observer); }

void XiMiner::MinerManager::setThreads(uint32_t threadCount) {
  m_threads = threadCount;
  for (uint32_t i = 0; i < std::thread::hardware_concurrency() && i < m_worker.size(); ++i) {
    if (i >= m_threads) {
      m_worker[i]->pause();
    } else {
      m_worker[i]->resume();
    }
  }
}

uint32_t XiMiner::MinerManager::threads() const { return m_threads; }

XiMiner::CollectiveHashrateSummary XiMiner::MinerManager::resetHashrateSummary() {
  CollectiveHashrateSummary hashrates;

  if (m_shutdownRequest.load()) {
    hashrates.Threads = 0;
    hashrates.Hashrate = 0.0;
    return hashrates;
  }

  hashrates.Threads = threads();
  hashrates.ThreadsSummary.reserve(threads());
  std::transform(m_worker.begin(), m_worker.begin() + threads(), std::back_inserter(hashrates.ThreadsSummary),
                 [](auto iWorker) { return iWorker->resetHashrateSummary(); });
  hashrates.Hashrate = std::accumulate(
      hashrates.ThreadsSummary.begin(), hashrates.ThreadsSummary.end(), 0.0,
      [](auto acc, const auto& iSum) { return acc + (1000.0 * (double)iSum.HashCount) / (double)iSum.Milliseconds; });
  return hashrates;
}