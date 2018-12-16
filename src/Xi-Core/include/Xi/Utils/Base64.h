#pragma once

#include <string>

namespace Xi {
namespace Base64 {
std::string encode(const std::string& raw);
std::string decode(const std::string& raw);
}  // namespace Base64
}  // namespace Xi
