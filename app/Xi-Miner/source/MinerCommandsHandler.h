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

#include <Logging/ILogger.h>
#include <Logging/LoggerRef.h>
#include <Logging/ConsoleLogger.h>
#include <Logging/LoggerManager.h>
#include <Common/ConsoleHandler.h>

#include "UpdateMonitor.h"
#include "MinerMonitor.h"
#include "MinerManager.h"

namespace XiMiner {
class MinerCommandsHandler : public Common::ConsoleHandler {
 public:
  MinerCommandsHandler(MinerManager& miner, UpdateMonitor& monitor, Logging::LoggerManager& logger);

  MinerMonitor& minerMonitor();

  void showHashrate();
  void hideHashrate();

 private:
  bool exit(const std::vector<std::string>& args);
  bool help(const std::vector<std::string>& args);
  bool version(const std::vector<std::string>& args);

  bool status(const std::vector<std::string>& args);

  bool set_poll_interval(const std::vector<std::string>& args);
  bool set_threads(const std::vector<std::string>& args);

  bool set_log(const std::vector<std::string>& args);
  bool hide_log(const std::vector<std::string>& args);

  bool hide_hashrate(const std::vector<std::string>& args);
  bool show_hashrate(const std::vector<std::string>& args);

  bool set_report_interval(const std::vector<std::string>& args);

 protected:
  void printError(std::string error) override;
  void printWarning(std::string warn) override;
  void printMessage(std::string msg) override;

 private:
  MinerManager& m_miner;
  UpdateMonitor& m_monitor;
  Logging::ConsoleLogger m_monitorlogger;
  Logging::ConsoleLogger m_clogger;
  Logging::LoggerRef m_logger;
  Logging::LoggerManager& m_appLogger;
  MinerMonitor m_minerMonitor;
};
}  // namespace XiMiner
