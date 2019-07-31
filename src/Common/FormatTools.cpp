// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero Project
// Copyright (c) 2018, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include "FormatTools.h"
#include <cstdio>
#include <ctime>
#include <Xi/Config.h>
#include "CryptoNoteCore/Core.h"
#include "Rpc/CoreRpcServerCommandsDefinitions.h"

#include <Xi/ExternalIncludePush.h>
#include <boost/format.hpp>
#include <rang.hpp>
#include <yaml-cpp/yaml.h>
#include <Xi/ExternalIncludePop.h>

namespace Common {

//--------------------------------------------------------------------------------
std::string get_mining_speed(uint32_t hr) {
  if (hr > 1e9)
    return (boost::format("%.2f GH/s") % (hr / 1e9)).str();
  if (hr > 1e6)
    return (boost::format("%.2f MH/s") % (hr / 1e6)).str();
  if (hr > 1e3)
    return (boost::format("%.2f KH/s") % (hr / 1e3)).str();

  return (boost::format("%.0f H/s") % hr).str();
}

//--------------------------------------------------------------------------------
std::string get_sync_percentage(CryptoNote::BlockHeight height, CryptoNote::BlockHeight target_height) {
  /* Don't divide by zero */
  if (height.isNull() || target_height.isNull()) {
    return "0.00";
  }

  /* So we don't have > 100% */
  if (height > target_height) {
    height = target_height;
  }

  float pc = 100.0f * height.native() / target_height.native();

  if (height < target_height && pc > 99.99f) {
    pc = 99.99f;  // to avoid 100% when not fully synced
  }

  return (boost::format("%.2f") % pc).str();
}

enum ForkStatus { UpToDate, ForkLater, ForkSoonReady, ForkSoonNotReady, OutOfDate };

ForkStatus get_fork_status(CryptoNote::BlockHeight height, std::vector<CryptoNote::BlockHeight> upgrade_heights,
                           CryptoNote::BlockHeight supported_height, const CryptoNote::Currency& currency) {
  /* Allow fork heights to be empty */
  if (upgrade_heights.empty()) {
    return UpToDate;
  }

  CryptoNote::BlockHeight next_fork;

  for (auto upgrade : upgrade_heights) {
    /* We have hit an upgrade already that the user cannot support */
    if (height >= upgrade && supported_height < upgrade) {
      return OutOfDate;
    }

    /* Get the next fork height */
    if (upgrade > height) {
      next_fork = upgrade;
      break;
    }
  }

  auto days = (next_fork - height).native() / currency.coin().expectedBlocksPerDay();

  /* Next fork in < 30 days away */
  if (days < 30) {
    /* Software doesn't support the next fork yet */
    if (supported_height < next_fork) {
      return ForkSoonNotReady;
    } else {
      return ForkSoonReady;
    }
  }

  if (height > next_fork) {
    return UpToDate;
  }

  return ForkLater;
}

std::string get_fork_time(CryptoNote::BlockHeight height, const std::vector<CryptoNote::BlockHeight>& upgrade_heights,
                          const CryptoNote::Currency& currency) {
  CryptoNote::BlockHeight next_fork;

  for (auto upgrade : upgrade_heights) {
    /* Get the next fork height */
    if (upgrade > height) {
      next_fork = upgrade;
      break;
    }
  }

  auto heightOffset = next_fork - height;
  if (heightOffset.native() < 0) {
    return "";
  }
  auto days = heightOffset.native() / currency.coin().expectedBlocksPerDay();
  if (height == next_fork) {
    return " (forking now),";
  } else if (days < 1) {
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(heightOffset.native() *
                                                                      std::chrono::seconds{currency.coin().blockTime()})
                           .count();
    return (boost::format(" (next fork in %.1f hours),") % hours).str();
  } else {
    return (boost::format(" (next fork in %.1f days),") % days).str();
  }
}

std::string get_update_status(ForkStatus forkStatus, CryptoNote::BlockHeight height,
                              const std::vector<CryptoNote::BlockHeight>& upgrade_heights,
                              const CryptoNote::Currency& currency) {
  switch (forkStatus) {
    case UpToDate:
    case ForkLater: {
      return " up to date";
    }
    case ForkSoonReady: {
      return get_fork_time(height, upgrade_heights, currency) + " up to date";
    }
    case ForkSoonNotReady: {
      return get_fork_time(height, upgrade_heights, currency) + " update needed";
    }
    case OutOfDate: {
      return " out of date, likely forked";
    }
    default: {
      throw std::runtime_error("Unexpected case unhandled");
    }
  }
}

//--------------------------------------------------------------------------------
std::string get_upgrade_info(CryptoNote::BlockHeight supported_height,
                             std::vector<CryptoNote::BlockHeight> upgrade_heights,
                             const CryptoNote::Currency& currency) {
  for (auto upgrade : upgrade_heights) {
    if (upgrade > supported_height) {
      return "The network forked at height " + std::to_string(upgrade.native()) +
             ", please update your software: " + currency.general().downloadUrl();
    }
  }

  /* This shouldnt happen */
  return std::string();
}

//--------------------------------------------------------------------------------
std::string get_status_string(const StatusInfo& iresp, const CryptoNote::Currency& currency) {
  std::stringstream ss;
  std::time_t uptime = std::time(nullptr) - iresp.start_time;
  auto forkStatus = get_fork_status(iresp.network_height, iresp.upgrade_heights, iresp.supported_height, currency);

  ss << "Height: " << iresp.height.native() << "/" << iresp.network_height.native() << " ("
     << get_sync_percentage(iresp.height, iresp.network_height) << "%) "
     << "on " << iresp.network << (iresp.synced ? " synced, " : " syncing, ") << "net hash "
     << get_mining_speed(iresp.hashrate) << ", "
     << "" << toString(iresp.version) << ","
     << get_update_status(forkStatus, iresp.network_height, iresp.upgrade_heights, currency) << ", "
     << iresp.outgoing_connections_count << "(out)+" << iresp.incoming_connections_count << "(in) connections, "
     << "uptime " << (unsigned int)floor(uptime / 60.0 / 60.0 / 24.0) << "d "
     << (unsigned int)floor(fmod((uptime / 60.0 / 60.0), 24.0)) << "h "
     << (unsigned int)floor(fmod((uptime / 60.0), 60.0)) << "m " << (unsigned int)fmod(uptime, 60.0) << "s";

  if (forkStatus == OutOfDate) {
    ss << std::endl << get_upgrade_info(iresp.supported_height, iresp.upgrade_heights, currency);
  }

  return ss.str();
}

void printStatus(const StatusInfo& iresp, const CryptoNote::Currency& currency, std::ostream& stream) {
  const auto currentTime = std::time(nullptr);
  const auto startTime = static_cast<std::time_t>(iresp.start_time);
  const auto upTime = currentTime - startTime;

  const auto upMinutes = upTime / 60;
  const auto upHours = upMinutes / 60;
  const auto upDays = upHours / 24;

  const auto syncPercentage = get_sync_percentage(iresp.height, iresp.network_height);
  const auto forkStatus =
      get_fork_status(iresp.network_height, iresp.upgrade_heights, iresp.supported_height, currency);

  YAML::Emitter emitter{stream};
  stream << rang::bg::reset << rang::fg::reset << rang::style::reset;
  emitter << YAML::BeginMap;
  {
    stream << rang::fg::green;
    emitter << YAML::Key << "height";

    std::stringstream builder{};
    builder << iresp.height.native() << "/" << iresp.network_height.native();
    stream << rang::fg::reset;
    emitter << YAML::Value << builder.str();

    if (iresp.height < iresp.network_height) {
      stream << rang::fg::yellow << rang::style::italic;
      stream << " (" << syncPercentage << "%) SYNCING";
      stream << rang::fg::reset << rang::style::reset;
    } else if (iresp.height > iresp.network_height.next(1)) {
      stream << rang::fg::red << rang::style::bold;
      stream << " DETACHED";
      stream << rang::fg::reset << rang::style::reset;
    } else {
      stream << rang::fg::green;
      stream << " SYNCED";
      stream << rang::fg::reset;
    }
  }
  {
    stream << rang::fg::green;
    emitter << YAML::Key << "network";
    stream << rang::fg::reset;
    emitter << YAML::Value << toString(iresp.network);
  }
  {
    stream << rang::fg::green;
    emitter << YAML::Key << "p2p";
    emitter << YAML::Value << YAML::BeginMap;
    emitter << YAML::Key << "outgoing connections";
    stream << rang::fg::reset;
    emitter << YAML::Value << iresp.outgoing_connections_count;
    stream << rang::fg::green;
    emitter << YAML::Key << "incoming connections";
    stream << rang::fg::reset;
    emitter << YAML::Value << iresp.incoming_connections_count;
    emitter << YAML::EndMap;
  }
  {
    stream << rang::fg::green;
    emitter << YAML::Key << "update status";
    stream << rang::fg::reset;
    if (forkStatus == ForkStatus::ForkLater) {
    } else if (forkStatus == ForkStatus::ForkSoonReady) {
      stream << rang::fg::green << rang::style::italic;
    } else if (forkStatus == ForkStatus::ForkSoonNotReady) {
      stream << rang::fg::red << rang::style::bold;
    } else if (forkStatus == ForkStatus::OutOfDate) {
      stream << rang::fg::red << rang::style::bold << rang::bg::yellow;

    } else if (forkStatus == ForkStatus::UpToDate) {
      stream << rang::fg::green << rang::style::underline;
    }
    emitter << YAML::Value
            << get_update_status(forkStatus, iresp.network_height, iresp.upgrade_heights, currency).substr(1);
    stream << rang::fg::reset << rang::style::reset;
  }
  {
    stream << rang::fg::green;
    emitter << YAML::Key << "hashrate";
    stream << rang::fg::reset;
    emitter << YAML::Value << get_mining_speed(iresp.hashrate);
  }
  {
    stream << rang::fg::green;
    emitter << YAML::Key << "uptime";
    stream << rang::fg::reset;
    std::stringstream builder{};
    builder << upDays << "d+" << upHours % 24 << ":" << upMinutes % 60;
    emitter << YAML::Value << builder.str();
  }
  emitter << YAML::EndMap;
  stream << rang::bg::reset << rang::fg::reset << rang::style::reset;
}

}  // namespace Common
