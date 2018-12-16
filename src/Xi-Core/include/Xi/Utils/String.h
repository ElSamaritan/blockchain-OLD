#pragma once

#include <string>
#include <utility>
#include <cinttypes>
#include <type_traits>
#include <sstream>

#include <boost/optional.hpp>

namespace Xi {
bool starts_with(const std::string& str, const std::string& prefix);

template <typename _T>
inline _T lexical_cast(const std::string& value);

template <>
uint16_t lexical_cast<uint16_t>(const std::string& value);

template <>
uint32_t lexical_cast<uint32_t>(const std::string& value);

template <>
uint64_t lexical_cast<uint64_t>(const std::string& value);

template <typename _T>
inline boost::optional<_T> safe_lexical_cast(const std::string& value) {
  try {
    return boost::optional<_T>{lexical_cast<_T>(value)};
  } catch (...) {
    return boost::optional<_T>{};
  }
}

#define FOWARD_TO_STRING_TO_STD(TYPE) \
  inline std::string to_string(TYPE&& value) { return std::to_string(std::forward<TYPE>(value)); }

FOWARD_TO_STRING_TO_STD(char)
FOWARD_TO_STRING_TO_STD(unsigned char)
FOWARD_TO_STRING_TO_STD(short)
FOWARD_TO_STRING_TO_STD(unsigned short)
FOWARD_TO_STRING_TO_STD(int)
FOWARD_TO_STRING_TO_STD(unsigned int)
FOWARD_TO_STRING_TO_STD(long)
FOWARD_TO_STRING_TO_STD(unsigned long)
FOWARD_TO_STRING_TO_STD(long long)
FOWARD_TO_STRING_TO_STD(unsigned long long)
FOWARD_TO_STRING_TO_STD(float)
FOWARD_TO_STRING_TO_STD(double)
FOWARD_TO_STRING_TO_STD(long double)

#undef FOWARD_TO_STRING_TO_STD

}  // namespace Xi
