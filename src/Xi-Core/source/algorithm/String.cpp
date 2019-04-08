#include "Xi/Algorithm/String.h"

#include <limits>
#include <string>
#include <locale>
#include <stdexcept>
#include <algorithm>
#include <cctype>

bool Xi::starts_with(const std::string &str, const std::string &prefix) {
  if (prefix.empty())
    return true;
  else if (str.size() < prefix.size())
    return false;
  else
    return std::mismatch(prefix.begin(), prefix.end(), str.begin()).first == prefix.end();
}

std::string Xi::trim_left(std::string str) {
  str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace(ch); }));
  return str;
}

std::string Xi::trim_right(std::string str) {
  str.erase(std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace(ch); }).base(), str.end());
  return str;
}

std::string Xi::trim(std::string str) { return trim_right(trim_left(str)); }

static_assert(std::numeric_limits<uint64_t>::max() == std::numeric_limits<unsigned long long>::max(), "");

namespace Xi {
template <>
uint16_t lexical_cast<uint16_t>(const std::string &value) {
  auto const converted = std::stoul(value);
  if (converted > std::numeric_limits<uint16_t>::max()) throw std::runtime_error{"provided value is too large"};
  return static_cast<uint16_t>(converted);
}

template <>
uint32_t lexical_cast<uint32_t>(const std::string &value) {
  auto const converted = std::stoull(value);
  if (converted > std::numeric_limits<uint32_t>::max()) throw std::runtime_error{"provided value is too large"};
  return static_cast<uint32_t>(converted);
}

template <>
uint64_t lexical_cast<uint64_t>(const std::string &value) {
  return std::stoull(value);
}

std::string to_lower(const std::string &str) {
  const std::locale loc{};
  std::string reval;
  reval.resize(str.size(), '\0');
  std::transform(str.begin(), str.end(), reval.begin(), [&](auto c) { return std::tolower(c, loc); });
  return reval;
}

std::string to_upper(const std::string &str) {
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

}  // namespace Xi
