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

#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>
#include <Logging/ConsoleLogger.h>
#include <Logging/LoggerManager.h>
#include <Common/ConsoleHandler.h>

#include "UpdateMonitor.h"
#include "MinerMonitor.h"
#include "MinerManager.h"

namespace XiMiner {
class MinerCommandsHandler : public Common::ConsoleHandler, MinerMonitor::Observer {
 public:
  MinerCommandsHandler(MinerManager& miner, UpdateMonitor& monitor, Logging::LoggerManager& logger);
  ~MinerCommandsHandler() override;

  MinerMonitor& minerMonitor();

  void reportShow();
  void reportHide();

  // Miner Monitor Observer
  void onBlockSubmission(BlockSubmissionResult submission) override;
  void onStatusReport(MinerStatus status) override;

 private:
  bool help(const std::vector<std::string>& args);
  bool version(const std::vector<std::string>& args);

  bool status(const std::vector<std::string>& args);

  bool poll_interval_set(const std::vector<std::string>& args);
  bool threads_set(const std::vector<std::string>& args);

  bool log_set(const std::vector<std::string>& args);
  bool log_hide(const std::vector<std::string>& args);

  bool report_hide(const std::vector<std::string>& args);
  bool report_show(const std::vector<std::string>& args);
  bool report_interval_set(const std::vector<std::string>& args);

 protected:
  void printError(std::string error) override;
  void printWarning(std::string warn) override;
  void printMessage(std::string msg) override;

 private:
  MinerManager& m_miner;
  UpdateMonitor& m_monitor;
  Logging::LoggerManager& m_appLogger;
  MinerMonitor m_minerMonitor;
};
}  // namespace XiMiner
