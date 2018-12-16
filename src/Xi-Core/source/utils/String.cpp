#include "Xi/Utils/String.h"

#include <limits>
#include <string>
#include <stdexcept>
#include <algorithm>

bool Xi::starts_with(const std::string &str, const std::string &prefix) {
  if (prefix.empty())
    return true;
  else if (str.size() < prefix.size())
    return false;
  else
    return std::mismatch(prefix.begin(), prefix.end(), str.begin()).first == prefix.end();
}

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
}  // namespace Xi
