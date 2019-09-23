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

#include "Xi/Algorithm/String.h"

#include <limits>
#include <string>
#include <locale>
#include <stdexcept>
#include <algorithm>
#include <cctype>

#include <Xi/ExternalIncludePush.h>
#include <boost/algorithm/string.hpp>
#include <Xi/ExternalIncludePop.h>

bool Xi::starts_with(const std::string &str, const std::string &prefix) {
  if (prefix.empty())
    return true;
  else if (str.size() < prefix.size())
    return false;
  else
    return std::mismatch(prefix.begin(), prefix.end(), str.begin()).first == prefix.end();
}

bool Xi::ends_with(const std::string &str, const std::string &suffix) {
  if (suffix.empty())
    return true;
  else if (str.size() < suffix.size())
    return false;
  else
    return std::mismatch(suffix.begin(), suffix.end(), std::next(str.begin(), str.size() - suffix.size())).first ==
           suffix.end();
}

std::string Xi::trim_left(std::string str) {
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace(ch); }));
  return str;
}

std::string Xi::trim_right(std::string str) {
  str.erase(std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace(ch); }).base(), str.end());
  return str;
}

std::string Xi::trim(std::string str) {
  return trim_right(trim_left(str));
}

static_assert(std::numeric_limits<uint64_t>::max() == std::numeric_limits<unsigned long long>::max(), "");

namespace Xi {
template <>
uint16_t lexical_cast<uint16_t>(const std::string &value) {
  auto const converted = std::stoul(value);
  if (converted > std::numeric_limits<uint16_t>::max())
    throw std::runtime_error{"provided value is too large"};
  return static_cast<uint16_t>(converted);
}

template <>
uint32_t lexical_cast<uint32_t>(const std::string &value) {
  auto const converted = std::stoull(value);
  if (converted > std::numeric_limits<uint32_t>::max())
    throw std::runtime_error{"provided value is too large"};
  return static_cast<uint32_t>(converted);
}

template <>
uint64_t lexical_cast<uint64_t>(const std::string &value) {
  return std::stoull(value);
}

std::string toLower(const std::string &str) {
  const std::locale loc{};
  std::string reval;
  reval.resize(str.size(), '\0');
  std::transform(str.begin(), str.end(), reval.begin(), [&](auto c) { return std::tolower(c, loc); });
  return reval;
}

std::string toUpper(const std::string &str) {
  const std::locale loc{};
  std::string reval;
  reval.resize(str.size(), '\0');
  std::transform(str.begin(), str.end(), reval.begin(), [&](auto c) { return std::toupper(c, loc); });
  return reval;
}

std::string to_string(time_t timestamp) {
  char buff[20];
  strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&timestamp));
  return buff;
}

std::string replace(std::string_view str, std::string_view toReplace, std::string_view replacement) {
  std::string reval{str};
  std::string stoReplace{toReplace};
  std::string sreplacement{replacement};
  boost::replace_all(reval, toReplace, replacement);
  return reval;
}

std::vector<std::string> split(const std::string &str, const std::string &tokens) {
  std::vector<std::string> reval{};
  boost::split(reval, str, boost::is_any_of(tokens), boost::token_compress_on);
  for (auto &ireval : reval) {
    ireval = Xi::trim(ireval);
  }
  reval.erase(std::remove_if(reval.begin(), reval.end(), [](const auto &i) { return i.empty(); }), reval.end());
  return reval;
}

std::string join(ConstSpan<std::string> values, const std::string &token) {
  if (values.empty()) {
    return "";
  }
  std::stringstream builder{};
  builder << values[0];
  for (size_t i = 1; i < values.size(); ++i) {
    builder << token << values[i];
  }
  return builder.str();
}

}  // namespace Xi
