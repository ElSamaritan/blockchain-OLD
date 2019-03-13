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

#include "MinerCommandsHandler.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <thread>

#include <Xi/Utils/ExternalIncludePush.h>
#include <boost/bind.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <Xi/Utils/ExternalIncludePop.h>

#include <Common/StringTools.h>
#include <CommonCLI/CommonCLI.h>

#define MINER_COMMAND_DEFINE(NAME, HELP) setHandler(#NAME, boost::bind(&MinerCommandsHandler::NAME, this, _1), HELP)

XiMiner::MinerCommandsHandler::MinerCommandsHandler(MinerManager &miner, UpdateMonitor &monitor,
                                                    Logging::LoggerManager &logger)
    : ConsoleHandler(),
      m_miner{miner},
      m_monitor{monitor},
      m_monitorlogger{Logging::INFO},
      m_clogger{Logging::INFO},
      m_logger{m_clogger, ""},
      m_appLogger{logger},
      m_minerMonitor{miner, m_monitorlogger} {
  MINER_COMMAND_DEFINE(exit, "shuts down the miner gracefully");
  MINER_COMMAND_DEFINE(help, "prints a summary of all commands");
  MINER_COMMAND_DEFINE(version, "prints version information");

  MINER_COMMAND_DEFINE(status, "prints the current miner status");

  MINER_COMMAND_DEFINE(set_poll_interval, "sets the block template polling interval in milliseconds");
  MINER_COMMAND_DEFINE(set_threads, "sets the number of threads to use for mining");

  MINER_COMMAND_DEFINE(set_log, "sets the maximum log level");
  MINER_COMMAND_DEFINE(hide_log, "hides application log, except for this interface");

  MINER_COMMAND_DEFINE(show_hashrate, "prints the current hashrate to the console");
  MINER_COMMAND_DEFINE(hide_hashrate, "disables printing the current hashrate to the console");

  MINER_COMMAND_DEFINE(set_report_interval, "sets the status report interval of the miner monitor");

  m_clogger.setPattern("");
  m_monitorlogger.setPattern("");
}

XiMiner::MinerMonitor &XiMiner::MinerCommandsHandler::minerMonitor() { return m_minerMonitor; }

void XiMiner::MinerCommandsHandler::showHashrate() { m_monitorlogger.setMaxLevel(Logging::TRACE); }

void XiMiner::MinerCommandsHandler::hideHashrate() { m_appLogger.setMaxLevel(Logging::NONE); }

#undef MINER_COMMAND_DEFINE

bool XiMiner::MinerCommandsHandler::exit(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  requestStop();
  m_miner.shutdown();
  return true;
}

bool XiMiner::MinerCommandsHandler::help(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  std::stringstream ss;
  ss << "Commands: " << ENDL;
  std::string usage = getUsage();
  boost::replace_all(usage, "\n", "\n  ");
  usage.insert(0, "  ");
  ss << usage << ENDL;
  m_logger(Logging::INFO) << ss.str();
  return true;
}

bool XiMiner::MinerCommandsHandler::version(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  m_logger(Logging::INFO) << CommonCLI::verboseVersionInformation();
  return true;
}

bool XiMiner::MinerCommandsHandler::status(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  std::stringstream builder{};
  builder << std::fixed << std::setprecision(2);

  const auto colorForHrDiff = [](double first, double second) -> std::string {
    if (second == 0.0) return Logging::GREEN;
    auto procent = (first - second) / second;
    if (procent >= -0.02)
      return Logging::GREEN;
    else if (procent < -0.1)
      return Logging::RED;
    else
      return Logging::YELLOW;
  };

  auto minerStatus = m_minerMonitor.status();
  builder << Logging::DEFAULT << "Current Hashrate  "
          << colorForHrDiff(minerStatus.CurrentHashrate, minerStatus.AverageHashrate) << minerStatus.CurrentHashrate
          << "H/s\n";
  builder << Logging::DEFAULT << "Average Hashrate  " << minerStatus.AverageHashrate << "H/s\n\n";
  builder << Logging::DEFAULT << "Threads In Use    " << minerStatus.Threads << "\n";
  builder << Logging::DEFAULT << "Latest Block Hash " << Common::toHex(minerStatus.TopBlockHash.data, 32) << "\n\n";
  builder << Logging::YELLOW << "Blocks Mined      " << (minerStatus.BlocksMined > 0 ? Logging::GREEN : Logging::WHITE)
          << minerStatus.BlocksMined;
  m_logger(Logging::INFO, Logging::DEFAULT) << builder.str();
  return true;
}

bool XiMiner::MinerCommandsHandler::set_poll_interval(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    return false;
  }

  uint32_t interval = 1000;
  try {
    interval = std::stoul(args[0]);
  } catch (std::invalid_argument &) {
    m_logger(Logging::ERROR) << "provided interval is not a valid integer.";
    return false;
  }

  if (interval < 50) {
    m_logger(Logging::ERROR) << "interval must be at least 50ms";
    return false;
  }

  m_monitor.setPollInterval(std::chrono::milliseconds{interval});
  return true;
}

bool XiMiner::MinerCommandsHandler::set_threads(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    return false;
  }

  uint32_t threads = 0;
  try {
    threads = std::stoul(args[0]);
  } catch (std::invalid_argument &) {
    m_logger(Logging::ERROR) << "provided thread count is not a valid integer.";
    return false;
  }

  if (threads > std::thread::hardware_concurrency()) {
    m_logger(Logging::ERROR) << "thread count can not exceed " << std::to_string(std::thread::hardware_concurrency());
    return false;
  }

  if (threads == 0) {
    threads = static_cast<uint32_t>(std::thread::hardware_concurrency());
  }

  m_miner.setThreads(threads);
  m_minerMonitor.reset();

  m_logger(Logging::INFO) << "changed threads used to " << threads;
  return true;
}

bool XiMiner::MinerCommandsHandler::set_log(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    return false;
  }

  Logging::LevelTranslator trans{};
  auto level = trans.get_value(args[0]);

  if (!level.has_value()) {
    m_logger(Logging::WARNING) << "unrecognized log level: " << args[0];
    return false;
  }

  m_appLogger.setMaxLevel(*level);
  return true;
}

bool XiMiner::MinerCommandsHandler::hide_log(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  hideHashrate();
  return true;
}

bool XiMiner::MinerCommandsHandler::hide_hashrate(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  m_monitorlogger.setMaxLevel(Logging::INFO);
  return true;
}

bool XiMiner::MinerCommandsHandler::show_hashrate(const std::vector<std::string> &args) {
  XI_UNUSED(args);
  showHashrate();
  return true;
}

bool XiMiner::MinerCommandsHandler::set_report_interval(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    return false;
  }

  uint64_t seconds = 0;
  try {
    seconds = std::stoull(args[0]);
  } catch (std::invalid_argument &) {
    m_logger(Logging::ERROR) << "provided interval is not a valid integer.";
    return false;
  }

  if (seconds == 0) {
    m_logger(Logging::ERROR) << "report interval must be at least one second.";
  }

  m_minerMonitor.setReportInterval(std::chrono::seconds{seconds});
  m_logger(Logging::INFO) << "changed report interval to " << seconds << " seconds.";
  return true;
}

void XiMiner::MinerCommandsHandler::printError(std::string error) { m_logger(Logging::ERROR) << error; }

void XiMiner::MinerCommandsHandler::printWarning(std::string warn) { m_logger(Logging::WARNING) << warn; }

void XiMiner::MinerCommandsHandler::printMessage(std::string msg) { m_logger(Logging::INFO) << msg; }
