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

#include "Xi/Time.h"

#include <algorithm>
#include <cctype>

#include <Xi/ExternalIncludePush.h>
#include <boost/algorithm/string.hpp>
#include <Xi/ExternalIncludePop.h>

#include <Xi/Exceptional.hpp>

namespace {
std::chrono::microseconds parseUnit(const std::string& amount, const std::string& unit) {
  const auto pamount = std::stoll(amount);
  if (unit == "microseconds" || unit == "microsecond") {
    return std::chrono::microseconds{pamount};
  } else if (unit == "ms" || unit == "milliseconds" || unit == "millisecond") {
    return std::chrono::milliseconds{pamount};
  } else if (unit == "s" || unit == "seconds" || unit == "second") {
    return std::chrono::seconds{pamount};
  } else if (unit == "m" || unit == "minutes" || unit == "minute") {
    return std::chrono::minutes{pamount};
  } else if (unit == "h" || unit == "hours" || unit == "hour") {
    return std::chrono::hours{pamount};
  } else if (unit == "d" || unit == "days" || unit == "day") {
    return std::chrono::hours{pamount * 24};
  } else if (unit == "w" || unit == "weeks" || unit == "week") {
    return std::chrono::hours{pamount * 7 * 24};
  } else {
    throw std::runtime_error{std::string{"unknown time unit: "} + unit};
  }
}
}  // namespace

Xi::Result<std::chrono::microseconds> Xi::Time::parseDuration(std::string str) {
  XI_ERROR_TRY();
  boost::trim(str);
  std::vector<std::string> tokens;
  {
    std::vector<std::string> spacedTokens;
    boost::split(spacedTokens, str, boost::is_any_of(" "));
    spacedTokens.erase(std::remove_if(spacedTokens.begin(), spacedTokens.end(),
                                      [](const auto& str) {
                                        return str.empty() || std::all_of(str.begin(), str.end(),
                                                                          [](const auto c) { return std::isspace(c); });
                                      }),
                       spacedTokens.end());
    for (auto& spacedToken : spacedTokens) {
      boost::trim(spacedToken);
      auto search = std::find_if(spacedToken.begin(), spacedToken.end(),
                                 [](const auto c) { return !std::isdigit(c) && c != '+' && c != '-'; });
      if (search != spacedToken.end()) {
        tokens.emplace_back(spacedToken.begin(), search);
        tokens.emplace_back(search, spacedToken.end());
      } else {
        tokens.push_back(spacedToken);
      }
    }
  }

  tokens.erase(std::remove_if(tokens.begin(), tokens.end(),
                              [](const auto& str) {
                                return str.empty() || std::all_of(str.begin(), str.end(),
                                                                  [](const auto c) { return std::isspace(c); });
                              }),
               tokens.end());

  if (tokens.empty()) {
    exceptional<std::runtime_error>("cannot parse an empty string encoded duration");
  } else if (tokens.size() & 1) {
    exceptional<std::runtime_error>("one duration has no unit or the other way round");
  }

  std::chrono::microseconds total{0};
  for (size_t i = 0; i < tokens.size(); i += 2) {
    total += parseUnit(boost::to_lower_copy(tokens[i]), boost::to_lower_copy(tokens[i + 1]));
  }

  return success(total);
  XI_ERROR_CATCH();
}
