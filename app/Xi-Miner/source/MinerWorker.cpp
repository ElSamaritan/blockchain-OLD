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

#include "MinerWorker.h"

#include <chrono>

#include <Xi/ExternalIncludePush.h>
#include <boost/endian/conversion.hpp>
#include <Xi/ExternalIncludePop.h>

#include <crypto/cnx/cnx.h>
#include <CryptoNoteCore/CheckDifficulty.h>

XiMiner::MinerWorker::MinerWorker() {}

XiMiner::MinerWorker::~MinerWorker() {
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void XiMiner::MinerWorker::addObserver(XiMiner::MinerWorker::Observer *observer) { m_observer.add(observer); }

void XiMiner::MinerWorker::removeObserver(XiMiner::MinerWorker::Observer *observer) { m_observer.remove(observer); }

void XiMiner::MinerWorker::setInitialNonce(uint32_t nonce) {
  m_initialNonce = nonce;
  m_swapTemplate.store(true);
}

void XiMiner::MinerWorker::setNonceStep(uint32_t nonceStep) {
  m_nonceStep = nonceStep;
  m_swapTemplate.store(true);
}

void XiMiner::MinerWorker::setTemplate(XiMiner::MinerBlockTemplate block) {
  std::lock_guard<std::mutex> lck{m_blockBufferAccess};
  m_blockBuffer = std::move(block);
  m_swapTemplate.store(true);
}

void XiMiner::MinerWorker::run() {
  m_shutdownRequest.store(false);
  m_shutdown = std::promise<void>{};
  m_thread = std::thread{[this]() { this->mineLoop(); }};
}

void XiMiner::MinerWorker::pause() {
  if (!m_paused.load()) {
    resetHashrateSummary();
    m_paused.store(true);
  }
}

void XiMiner::MinerWorker::resume() {
  if (m_paused.load()) {
    resetHashrateSummary();
    m_paused.store(false);
  }
}

std::future<void> XiMiner::MinerWorker::shutdown() {
  m_shutdownRequest.store(true);
  return m_shutdown.get_future();
}

XiMiner::HashrateSummary XiMiner::MinerWorker::resetHashrateSummary() {
  HashrateSummary reval;
  reval.HashCount = m_hashCount.load();
  m_hashCount.store(0);
  auto now = std::chrono::high_resolution_clock::now();
  reval.Milliseconds =
      static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastHrSummary).count());
  m_lastHrSummary = now;
  return reval;
}

void XiMiner::MinerWorker::mineLoop() {
  resetHashrateSummary();
  auto block = acquireTemplate();
  uint32_t currentNonce = m_initialNonce;
  uint32_t nonceStep = m_nonceStep;
  while (!m_shutdownRequest.load()) {
    if (m_paused.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds{500});
      continue;
    }

    if (m_swapTemplate.load()) {
      block = acquireTemplate();
      currentNonce = m_initialNonce;
      nonceStep = m_nonceStep;
    }

    if (block.HashArray.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds{500});
      continue;
    }

    const uint32_t batchSize = 10;
    for (uint32_t i = 0; i < batchSize; ++i) {
      *reinterpret_cast<uint32_t *>(block.HashArray.data() + block.NonceOffset) =
          boost::endian::native_to_little(currentNonce);

      Crypto::Hash h;
      Crypto::CNX::Hash_v0{}(block.HashArray.data(), block.HashArray.size(), h);
      if (CryptoNote::check_hash(h, block.Difficutly)) {
        block.Template.nonce = currentNonce;
        m_observer.notify(&Observer::onBlockFound, block.Template);
      }
      currentNonce += nonceStep;
    }

    m_hashCount += batchSize;
  }
  m_shutdown.set_value();
}

XiMiner::MinerBlockTemplate XiMiner::MinerWorker::acquireTemplate() {
  std::lock_guard<std::mutex> lck{m_blockBufferAccess};
  XI_UNUSED(lck);
  m_swapTemplate.store(false);
  return m_blockBuffer;
}
