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

#include "MinerCommandsHandler.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <thread>

#include <Xi/ExternalIncludePush.h>
#include <boost/bind.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Common/StringTools.h>
#include <CommonCLI/CommonCLI.h>
#include <rang.hpp>

#define MINER_COMMAND_DEFINE(NAME, HELP) setHandler(#NAME, boost::bind(&MinerCommandsHandler::NAME, this, _1), HELP)

XiMiner::MinerCommandsHandler::MinerCommandsHandler(MinerManager &miner, UpdateMonitor &monitor,
                                                    Logging::LoggerManager &logger)
    : ConsoleHandler(), m_miner{miner}, m_monitor{monitor}, m_appLogger{logger}, m_minerMonitor{miner, m_appLogger} {
  m_minerMonitor.addObserver(this);

  MINER_COMMAND_DEFINE(help, "prints a summary of all commands");
  MINER_COMMAND_DEFINE(version, "prints version information");

  MINER_COMMAND_DEFINE(status, "prints the current miner status");

  MINER_COMMAND_DEFINE(poll_interval_set, "sets the block template polling interval in milliseconds");
  MINER_COMMAND_DEFINE(threads_set, "sets the number of threads to use for mining");

  MINER_COMMAND_DEFINE(log_set, "sets the maximum log level");
  MINER_COMMAND_DEFINE(log_hide, "hides application log, except for this interface");

  MINER_COMMAND_DEFINE(report_show, "prints the current status periodically");
  MINER_COMMAND_DEFINE(report_hide, "disables printing the current status");
  MINER_COMMAND_DEFINE(report_interval_set, "sets the status report interval");
}

XiMiner::MinerCommandsHandler::~MinerCommandsHandler() { m_minerMonitor.removeObserver(this); }

#undef MINER_COMMAND_DEFINE

XiMiner::MinerMonitor &XiMiner::MinerCommandsHandler::minerMonitor() { return m_minerMonitor; }

void XiMiner::MinerCommandsHandler::reportShow() { m_minerMonitor.statusReport(true); }
void XiMiner::MinerCommandsHandler::reportHide() { m_minerMonitor.statusReport(false); }

void XiMiner::MinerCommandsHandler::onBlockSubmission(BlockSubmissionResult submission) {
  printObject(submission, "Block Successfully Submitted");
}

void XiMiner::MinerCommandsHandler::onStatusReport(XiMiner::MinerStatus status) {
  printObject(status, "Status Report");
}

bool XiMiner::MinerCommandsHandler::help(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  std::stringstream ss;
  ss << "Commands: " << ENDL;
  std::string usage = getUsage();
  boost::replace_all(usage, "\n", "\n  ");
  usage.insert(0, "  ");
  ss << usage << ENDL;
  std::cout << ss.str();
  return true;
}

bool XiMiner::MinerCommandsHandler::version(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  printObject(CommonCLI::versionInformation(), "Version");
  return true;
}

bool XiMiner::MinerCommandsHandler::status(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  std::cout << "\n" << std::fixed << std::setprecision(2);

  const auto colorForHrDiff = [](double first, double second) -> rang::fg {
    if (second == 0.0) return rang::fg::green;
    auto percent = (first - second) / second;
    if (percent >= -0.02)
      return rang::fg::green;
    else if (percent < -0.1)
      return rang::fg::red;
    else
      return rang::fg::yellow;
  };

  auto minerStatus = m_minerMonitor.status();
  std::cout << rang::fg::reset << "Current Hashrate   "
            << colorForHrDiff(minerStatus.current_hashrate, minerStatus.average_hashrate)
            << minerStatus.current_hashrate << rang::fg::reset << "H/s\n";
  std::cout << rang::fg::reset << "Average Hashrate   " << minerStatus.average_hashrate << "H/s\n\n";
  std::cout << rang::fg::reset << "Threads In Use     " << minerStatus.active_threads << "\n\n";
  std::cout << rang::fg::reset << "Latest Block Hash  " << minerStatus.top_block.toString() << "\n";
  std::cout << rang::fg::reset << "Current Difficulty " << minerStatus.difficulty << "\n";
  std::cout << rang::fg::reset << "Current Algorithm  " << minerStatus.algorithm << "\n\n";
  std::cout << rang::fg::reset << "Blocks Mined       "
            << (minerStatus.blocks_mined > 0 ? rang::fg::green : rang::fg::reset) << minerStatus.blocks_mined;
  std::cout << rang::fg::reset << std::endl;
  return true;
}

bool XiMiner::MinerCommandsHandler::poll_interval_set(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    return false;
  }

  uint32_t interval = 1000;
  try {
    interval = std::stoul(args[0]);
  } catch (std::invalid_argument &) {
    printError("provided interval is not a valid integer.");
    return false;
  }

  if (interval < 50) {
    printError("interval must be at least 50ms");
    return false;
  }

  m_monitor.setPollInterval(std::chrono::milliseconds{interval});
  return true;
}

bool XiMiner::MinerCommandsHandler::threads_set(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    return false;
  }

  uint32_t threads = 0;
  try {
    threads = std::stoul(args[0]);
  } catch (std::invalid_argument &) {
    printError("provided thread count is not a valid integer.");
    return false;
  }

  if (threads > std::thread::hardware_concurrency()) {
    printError("thread count can not exceed " + std::to_string(std::thread::hardware_concurrency()));
    return false;
  }

  if (threads == 0) {
    threads = static_cast<uint32_t>(std::thread::hardware_concurrency());
  }

  m_miner.setThreads(threads);
  m_minerMonitor.reset();

  std::cout << "changed threads used to " << threads;
  return true;
}

bool XiMiner::MinerCommandsHandler::log_set(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    return false;
  }

  Logging::LevelTranslator trans{};
  auto level = trans.get_value(args[0]);

  if (!level.has_value()) {
    printError("unrecognized log level: " + args[0]);
    return false;
  }

  m_appLogger.setMaxLevel(*level);
  return true;
}

bool XiMiner::MinerCommandsHandler::log_hide(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  reportHide();
  return true;
}

bool XiMiner::MinerCommandsHandler::report_hide(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  m_appLogger.setMaxLevel(Logging::Info);
  return true;
}

bool XiMiner::MinerCommandsHandler::report_show(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  reportShow();
  return true;
}

bool XiMiner::MinerCommandsHandler::report_interval_set(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    return false;
  }

  uint64_t seconds = 0;
  try {
    seconds = std::stoull(args[0]);
  } catch (std::invalid_argument &) {
    printError("provided interval is not a valid integer.");
    return false;
  }

  if (seconds == 0) {
    printError("report interval must be at least one second.");
  }

  m_minerMonitor.setReportInterval(std::chrono::seconds{seconds});
  std::cout << "changed report interval to " << seconds << " seconds.";
  return true;
}

void XiMiner::MinerCommandsHandler::printError(std::string error) {
  std::cout << rang::fg::red << error << rang::fg::reset << std::endl;
}

void XiMiner::MinerCommandsHandler::printWarning(std::string warn) {
  std::cout << rang::fg::yellow << warn << rang::fg::reset << std::endl;
}

void XiMiner::MinerCommandsHandler::printMessage(std::string msg) {
  std::cout << rang::fg::reset << msg << rang::fg::reset << std::endl;
}
