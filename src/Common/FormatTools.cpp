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
#include <boost/format.hpp>

namespace Common {

//--------------------------------------------------------------------------------
std::string get_mining_speed(uint32_t hr) {
  if (hr > 1e9) return (boost::format("%.2f GH/s") % (hr / 1e9)).str();
  if (hr > 1e6) return (boost::format("%.2f MH/s") % (hr / 1e6)).str();
  if (hr > 1e3) return (boost::format("%.2f KH/s") % (hr / 1e3)).str();

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
std::string get_status_string(const CryptoNote::COMMAND_RPC_GET_INFO::response& iresp,
                              const CryptoNote::Currency& currency) {
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

}  // namespace Common
