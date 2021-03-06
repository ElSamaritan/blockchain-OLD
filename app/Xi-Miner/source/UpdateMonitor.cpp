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

#include "UpdateMonitor.h"

#include <utility>
#include <stdexcept>

#include <Common/StringTools.h>
#include <Rpc/JsonRpc.h>
#include <Rpc/Commands/Commands.h>
#include <Rpc/CoreRpcServerCommandsDefinitions.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/Transactions/TransactionApiExtra.h>

Xi::Result<std::unique_ptr<XiMiner::UpdateMonitor>> XiMiner::UpdateMonitor::start(
    std::string address, CryptoNote::Currency &currency, const CryptoNote::RpcRemoteConfiguration &remote,
    Logging::ILogger &logger) {
  XI_ERROR_TRY();
  return success(std::unique_ptr<UpdateMonitor>{new UpdateMonitor{std::move(address), currency, remote, logger}});
  XI_ERROR_CATCH();
}

XiMiner::UpdateMonitor::~UpdateMonitor() {
  if (m_keepRunning.load()) {
    shutdown();
  }
}

XiMiner::UpdateMonitor::UpdateMonitor(std::string address, CryptoNote::Currency &currency,
                                      const CryptoNote::RpcRemoteConfiguration &remote, Logging::ILogger &logger)
    : m_address{std::move(address)},
      m_currency{currency},
      m_logger{logger, "UpdateMonitor"},
      m_http{remote.Host, remote.Port, remote.Ssl} {
  CryptoNote::AccountPublicAddress _;
  if (!m_currency.parseAccountAddressString(m_address, _)) {
    throw std::runtime_error{"invalid account address"};
  }

  if (!remote.AccessToken.empty()) {
    m_http.useAuthorization(Xi::Http::BearerCredentials{remote.AccessToken});
  }

  m_pollThread = std::thread{[&, this] { this->updateLoop(); }};
  m_logger(Logging::Info) << "starting update monitor";
}

void XiMiner::UpdateMonitor::shutdown() {
  m_keepRunning.store(false);
  if (m_pollThread.joinable()) {
    m_pollThread.join();
  }
}

void XiMiner::UpdateMonitor::addObserver(XiMiner::UpdateMonitor::Observer *observer) { m_observer.add(observer); }

void XiMiner::UpdateMonitor::removeObserver(XiMiner::UpdateMonitor::Observer *observer) { m_observer.remove(observer); }

void XiMiner::UpdateMonitor::setPollInterval(std::chrono::milliseconds interval) {
  m_pollInterval.store(static_cast<uint64_t>(interval.count()));
}

std::chrono::milliseconds XiMiner::UpdateMonitor::pollInterval() const {
  return std::chrono::milliseconds{m_pollInterval.load()};
}

Xi::Result<std::string> XiMiner::UpdateMonitor::getBlockTemplateState() {
  XI_ERROR_TRY();
  m_logger(Logging::Trace) << "request block template state";
  CryptoNote::RpcCommands::GetBlockTemplateState::request request;
  CryptoNote::RpcCommands::GetBlockTemplateState::response response;
  CryptoNote::JsonRpc::invokeJsonRpcCommand(m_http, CryptoNote::RpcCommands::GetBlockTemplateState::identifier(),
                                            request, response);
  m_logger(Logging::Trace) << "block template state: " << response.template_state;
  return success(std::move(response.template_state));
  XI_ERROR_CATCH();
}

Xi::Result<XiMiner::MinerBlockTemplate> XiMiner::UpdateMonitor::getBlockTemplate() {
  XI_ERROR_TRY();
  m_logger(Logging::Trace) << "request block template state";
  CryptoNote::RpcCommands::GetBlockTemplate::request request;
  CryptoNote::RpcCommands::GetBlockTemplate::response response;
  request.wallet_address = m_address;
  CryptoNote::JsonRpc::invokeJsonRpcCommand(m_http, CryptoNote::RpcCommands::GetBlockTemplate::identifier(), request,
                                            response);
  m_logger(Logging::Trace) << "block template state: " << response.template_state;
  MinerBlockTemplate reval;
  CryptoNote::BinaryArray binaryTemplate = Common::fromHex(response.blocktemplate_blob);
  if (!CryptoNote::fromBinaryArray(reval.Template, binaryTemplate)) {
    throw std::runtime_error{"invalid bock template"};
  }

  CryptoNote::CachedBlock block{reval.Template};
  reval.Difficutly = response.difficulty;
  reval.TemplateState = response.template_state;
  reval.Algorithm = response.proof_of_work_algorithm;
  reval.ProofOfWork = block.getProofOfWorkBlob();

  return success(std::move(reval));
  XI_ERROR_CATCH();
}

void XiMiner::UpdateMonitor::updateLoop() {
  while (m_keepRunning) {
    try {
      const auto now = std::chrono::high_resolution_clock::now();
      if (now < m_lastPoll + pollInterval()) {
        std::this_thread::sleep_for((now + pollInterval()) - m_lastPoll);
        continue;
      }
      if (now < m_lastUpdate + 10 * pollInterval()) {
        const auto nodeState = getBlockTemplateState().takeOrThrow();
        if (nodeState == m_blockTemplateState) {
          m_lastPoll = std::chrono::high_resolution_clock::now();
          continue;
        }
      }
      m_logger(Logging::Debugging) << "template updated, polling...";
      auto block = getBlockTemplate().takeOrThrow();
      m_observer.notify(&Observer::onTemplateChanged, block);
      m_blockTemplateState = block.TemplateState;
      m_lastPoll = std::chrono::high_resolution_clock::now();
      m_lastUpdate = std::chrono::high_resolution_clock::now();
    } catch (std::exception &e) {
      m_logger(Logging::Error) << "update loop failed: " << e.what();
      std::this_thread::sleep_for(std::chrono::seconds{1});
    } catch (...) {
      m_logger(Logging::Error) << "updated loop failed for unknown reason.";
      std::this_thread::sleep_for(std::chrono::seconds{1});
    }
  }
}
