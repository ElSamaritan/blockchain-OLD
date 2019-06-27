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

#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <atomic>
#include <tuple>

#include <Xi/Global.hh>
#include <Xi/Result.h>
#include <Xi/Http/Client.h>
#include <Common/ObserverManager.h>
#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>
#include <Rpc/RpcRemoteConfiguration.h>
#include <CryptoNoteCore/CryptoNote.h>
#include <CryptoNoteCore/Currency.h>

#include "MinerBlockTemplate.h"

namespace XiMiner {
class UpdateMonitor {
 public:
  class Observer {
   public:
    virtual ~Observer() = default;

    virtual void onTemplateChanged(MinerBlockTemplate newTemplate) = 0;
  };

 public:
  static Xi::Result<std::unique_ptr<UpdateMonitor>> start(std::string address, CryptoNote::Currency& currency,
                                                          const CryptoNote::RpcRemoteConfiguration& remote,
                                                          Logging::ILogger& logger);

 public:
  XI_DELETE_COPY(UpdateMonitor);
  XI_DELETE_MOVE(UpdateMonitor);
  ~UpdateMonitor();

  void shutdown();

  void addObserver(Observer* observer);
  void removeObserver(Observer* observer);

  void setPollInterval(std::chrono::milliseconds interval);
  std::chrono::milliseconds pollInterval() const;

 private:
  UpdateMonitor(std::string address, CryptoNote::Currency& currency, const CryptoNote::RpcRemoteConfiguration& remote,
                Logging::ILogger& logger);

  Xi::Result<std::string> getBlockTemplateState();
  Xi::Result<MinerBlockTemplate> getBlockTemplate();

  void updateLoop();

 private:
  std::string m_address;
  CryptoNote::Currency& m_currency;
  Logging::LoggerRef m_logger;

  Tools::ObserverManager<Observer> m_observer;
  std::thread m_pollThread;
  std::atomic<uint64_t> m_pollInterval{1000};
  std::chrono::high_resolution_clock::time_point m_lastPoll{std::chrono::high_resolution_clock::time_point::min()};
  std::string m_blockTemplateState{""};
  std::atomic_bool m_keepRunning{true};
  Xi::Http::Client m_http;
};
}  // namespace XiMiner
