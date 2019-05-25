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

#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <future>

#include <Xi/Global.hh>
#include <Xi/Result.h>

#include <Common/ObserverManager.h>
#include <CryptoNoteCore/CryptoNote.h>

#include "MinerBlockTemplate.h"
#include "HashrateSummary.h"

namespace XiMiner {
class MinerWorker {
 public:
  class Observer {
   public:
    virtual ~Observer() = default;

    virtual void onBlockFound(CryptoNote::BlockTemplate block) = 0;
  };

 public:
  MinerWorker();
  XI_DELETE_COPY(MinerWorker);
  XI_DELETE_MOVE(MinerWorker);
  ~MinerWorker();

  void addObserver(Observer* observer);
  void removeObserver(Observer* observer);

  void setInitialNonce(uint32_t nonce);
  void setNonceStep(uint32_t nonceStep);
  void setTemplate(MinerBlockTemplate block);

  void run();
  void pause();
  void resume();
  std::future<void> shutdown();

  HashrateSummary resetHashrateSummary();

 private:
  void mineLoop();
  MinerBlockTemplate acquireTemplate();

 private:
  Tools::ObserverManager<Observer> m_observer;
  std::thread m_thread;
  std::atomic_bool m_running{false};
  std::atomic_bool m_shutdownRequest{false};
  std::atomic_bool m_paused{false};
  std::chrono::high_resolution_clock::time_point m_lastHrSummary;
  std::atomic<uint32_t> m_hashCount{0};
  std::promise<void> m_shutdown;

  uint32_t m_initialNonce = 0;
  uint32_t m_nonceStep = 1;
  std::atomic_bool m_swapTemplate{false};
  std::mutex m_blockBufferAccess;
  MinerBlockTemplate m_blockBuffer;
};
}  // namespace XiMiner
